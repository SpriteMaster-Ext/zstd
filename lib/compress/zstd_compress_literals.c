/*
 * Copyright (c) Yann Collet, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */

 /*-*************************************
 *  Dependencies
 ***************************************/
#include "zstd_compress_literals.h"


/* **************************************************************
*  Debug Traces
****************************************************************/
#if DEBUGLEVEL >= 2

static size_t showHexa(const void* src, size_t srcSize)
{
    const BYTE* const ip = (const BYTE*)src;
    size_t u;
    for (u=0; u<srcSize; u++) {
        RAWLOG(6, " %02X", ip[u]); (void)ip;
    }
    RAWLOG(6, " \n");
    return srcSize;
}

#endif


/* **************************************************************
*  Literals compression - special cases
****************************************************************/
size_t ZSTD_noCompressLiterals (void* dst, size_t dstCapacity, const void* src, size_t srcSize)
{
    BYTE* const ostart = (BYTE*)dst;
    U32   const flSize = 1 + (srcSize>31) + (srcSize>4095);

    DEBUGLOG(5, "ZSTD_noCompressLiterals: srcSize=%zu, dstCapacity=%zu", srcSize, dstCapacity);

    RETURN_ERROR_IF(srcSize + flSize > dstCapacity, dstSize_tooSmall, "");

    switch(flSize)
    {
        case 1: /* 2 - 1 - 5 */
            ostart[0] = (BYTE)((U32)set_basic + (srcSize<<3));
            break;
        case 2: /* 2 - 2 - 12 */
            MEM_writeLE16(ostart, (U16)((U32)set_basic + (1<<2) + (srcSize<<4)));
            break;
        case 3: /* 2 - 2 - 20 */
            MEM_writeLE32(ostart, (U32)((U32)set_basic + (3<<2) + (srcSize<<4)));
            break;
        default:   /* not necessary : flSize is {1,2,3} */
            _assume(0);
    }

    ZSTD_memcpy(ostart + flSize, src, srcSize);
    DEBUGLOG(5, "Raw (uncompressed) literals: %u -> %u", (U32)srcSize, (U32)(srcSize + flSize));
    return srcSize + flSize;
}

size_t ZSTD_compressRleLiteralsBlock (void* dst, size_t dstCapacity, const void* src, size_t srcSize)
{
    BYTE* const ostart = (BYTE*)dst;
    U32   const flSize = 1 + (srcSize>31) + (srcSize>4095);

    (void)dstCapacity;  /* dstCapacity already guaranteed to be >=4, hence large enough */

    switch(flSize)
    {
        case 1: /* 2 - 1 - 5 */
            ostart[0] = (BYTE)((U32)set_rle + (srcSize<<3));
            break;
        case 2: /* 2 - 2 - 12 */
            MEM_writeLE16(ostart, (U16)((U32)set_rle + (1<<2) + (srcSize<<4)));
            break;
        case 3: /* 2 - 2 - 20 */
            MEM_writeLE32(ostart, (U32)((U32)set_rle + (3<<2) + (srcSize<<4)));
            break;
        default:   /* not necessary : flSize is {1,2,3} */
            _assume(0);
    }

    ostart[flSize] = *(const BYTE*)src;
    DEBUGLOG(5, "RLE literals: %u -> %u", (U32)srcSize, (U32)flSize + 1);
    return flSize+1;
}

size_t ZSTD_compressLiterals (ZSTD_hufCTables_t const* prevHuf,
                              ZSTD_hufCTables_t* nextHuf,
                              ZSTD_strategy strategy, int disableLiteralCompression,
                              void* dst, size_t dstCapacity,
                        const void* src, size_t srcSize,
                              void* entropyWorkspace, size_t entropyWorkspaceSize,
                        const int bmi2,
                        unsigned suspectUncompressible)
{
    size_t const minGain = ZSTD_minGain(srcSize, strategy);
    size_t const lhSize = 3 + (srcSize >= 1 KB) + (srcSize >= 16 KB);
    BYTE*  const ostart = (BYTE*)dst;
    U32 singleStream = srcSize < 256;
    symbolEncodingType_e hType = set_compressed;
    size_t cLitSize;

    DEBUGLOG(5,"ZSTD_compressLiterals (disableLiteralCompression=%i, srcSize=%u, dstCapacity=%zu)",
                disableLiteralCompression, (U32)srcSize, dstCapacity);

    DEBUGLOG(6, "Completed literals listing (%zu bytes)", showHexa(src, srcSize));

    /* Prepare nextEntropy assuming reusing the existing table */
    ZSTD_memcpy(nextHuf, prevHuf, sizeof(*prevHuf));

    if (disableLiteralCompression)
        return ZSTD_noCompressLiterals(dst, dstCapacity, src, srcSize);

    /* small ? don't even attempt compression (speed opt) */
#   define COMPRESS_LITERALS_SIZE_MIN 63
    {   size_t const minLitSize = (prevHuf->repeatMode == HUF_repeat_valid) ? 6 : COMPRESS_LITERALS_SIZE_MIN;
        if (srcSize <= minLitSize) return ZSTD_noCompressLiterals(dst, dstCapacity, src, srcSize);
    }

    RETURN_ERROR_IF(dstCapacity < lhSize+1, dstSize_tooSmall, "not enough space for compression");
    {   HUF_repeat repeat = prevHuf->repeatMode;
        int const preferRepeat = (strategy < ZSTD_lazy) ? srcSize <= 1024 : 0;
        typedef size_t (*huf_compress_f)(void*, size_t, const void*, size_t, unsigned, unsigned, void*, size_t, HUF_CElt*, HUF_repeat*, int, int, unsigned);
        huf_compress_f huf_compress;
        if (repeat == HUF_repeat_valid && lhSize == 3) singleStream = 1;
        huf_compress = singleStream ? HUF_compress1X_repeat : HUF_compress4X_repeat;
        cLitSize = huf_compress(ostart+lhSize, dstCapacity-lhSize,
                                src, srcSize,
                                HUF_SYMBOLVALUE_MAX, LitHufLog,
                                entropyWorkspace, entropyWorkspaceSize,
                                (HUF_CElt*)nextHuf->CTable,
                                &repeat, preferRepeat,
                                bmi2, suspectUncompressible);
        if (repeat != HUF_repeat_none) {
            /* reused the existing table */
            DEBUGLOG(5, "Reusing previous huffman table");
            hType = set_repeat;
        }
    }

    if ((cLitSize==0) || (cLitSize >= srcSize - minGain) || ERR_isError(cLitSize)) {
        ZSTD_memcpy(nextHuf, prevHuf, sizeof(*prevHuf));
        return ZSTD_noCompressLiterals(dst, dstCapacity, src, srcSize);
    }
    if (cLitSize==1) {
        ZSTD_memcpy(nextHuf, prevHuf, sizeof(*prevHuf));
        return ZSTD_compressRleLiteralsBlock(dst, dstCapacity, src, srcSize);
    }

    if (hType == set_compressed) {
        /* using a newly constructed table */
        nextHuf->repeatMode = HUF_repeat_check;
    }

    /* Build header */
    switch(lhSize)
    {
    case 3: /* 2 - 2 - 10 - 10 */
        {   U32 const lhc = hType + ((U32)(!singleStream) << 2) + ((U32)srcSize<<4) + ((U32)cLitSize<<14);
            MEM_writeLE24(ostart, lhc);
            break;
        }
    case 4: /* 2 - 2 - 14 - 14 */
        {   U32 const lhc = hType + (2 << 2) + ((U32)srcSize<<4) + ((U32)cLitSize<<18);
            MEM_writeLE32(ostart, lhc);
            break;
        }
    case 5: /* 2 - 2 - 18 - 18 */
        {   U32 const lhc = hType + (3 << 2) + ((U32)srcSize<<4) + ((U32)cLitSize<<22);
            MEM_writeLE32(ostart, lhc);
            ostart[4] = (BYTE)(cLitSize >> 10);
            break;
        }
    default:  /* not possible : lhSize is {3,4,5} */
        _assume(0);
    }
    DEBUGLOG(5, "Compressed literals: %u -> %u", (U32)srcSize, (U32)(lhSize+cLitSize));
    return lhSize+cLitSize;
}
