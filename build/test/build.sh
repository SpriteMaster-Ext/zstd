[ -z ${CC+x} ] && CC=gcc

src=../../lib/

src_files=( \
  "common/pool.c" \
  "common/debug.c" \
  "common/threading.c" \
  "common/entropy_common.c" \
  "compress/hist.c" \
  "common/error_private.c" \
  "common/xxhash.c" \
  "common/zstd_common.c" \
  "common/fse_decompress.c" \
  "compress/fse_compress.c" \
  "compress/huf_compress.c" \
  "compress/zstd_compress.c" \
  "compress/zstd_compress_literals.c" \
  "compress/zstd_compress_sequences.c" \
  "compress/zstd_compress_superblock.c" \
  "compress/zstd_fast.c" \
  "compress/zstd_double_fast.c" \
  "compress/zstd_lazy.c" \
  "compress/zstd_opt.c" \
  "compress/zstd_ldm.c" \
  "compress/zstdmt_compress.c" \
  "decompress/huf_decompress.c" \
  "decompress/zstd_decompress.c" \
  "decompress/zstd_decompress_block.c" \
  "decompress/zstd_ddict.c" \
  "dictBuilder/cover.c" \
  "dictBuilder/fastcover.c" \
  "dictBuilder/divsufsort.c" \
  "dictBuilder/zdict.c" \
	"decompress/huf_decompress_amd64.S" \
)

src_files=("${src_files[@]/#/${src}}")

defines=( \
	"ZSTD_DLL_EXPORT=1" \
	"ZSTD_LEGACY_SUPPORT=0" \
	"ZSTD_STRIP_ERROR_STRINGS=1" \
	"ZSTD_NO_ERRORLIB=1" \
	"ZSTD_USE_XXH3=1" \
	"MEM_FORCE_MEMORY_ACCESS=1" \
)

defines=("${defines[@]/#/-D}")

# -fms-extensions

flags=( \
	"-std=gnu2x" \
	"-g0" \
	"-O3" \
	"-flto" \
	"-fno-exceptions" \
	"-fdelete-dead-exceptions" \
	"-fvisibility=hidden" \
	"-fallow-store-data-races" \
	"-fno-semantic-interposition" \
	"-fmerge-all-constants" \
	"-fdevirtualize-speculatively" \
	"-fdevirtualize-at-ltrans" \
	"-fipa-pta" \
	"-fgraphite-identity" \
	"-fivopts" \
	"-ftree-loop-im" \
	"-ftree-loop-ivcanon" \
	"-fstdarg-opt" \
	"-mtune=znver3" \
	"-fdata-sections" \
	"-ffunction-sections" \
)

ld_flags=( \
	"--strip-all" \
	"--discard-all" \
	"--gc-sections" \
	"--relax" \
)

ld_flags=("${ld_flags[@]/#/-Wl,}")

"${CC}" -shared -o libzstd.so -fPIC "${flags[@]}" "${ld_flags[@]}" "${defines[@]}" "${src_files[@]}"