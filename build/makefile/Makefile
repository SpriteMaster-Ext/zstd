CC ?= gcc

SRC_ROOT := ../../lib

CFILES := \
	common/pool.c \
	common/debug.c \
	common/threading.c \
	common/entropy_common.c \
	compress/hist.c \
	common/error_private.c \
	common/xxhash.c \
	common/zstd_common.c \
	common/fse_decompress.c \
	compress/fse_compress.c \
	compress/huf_compress.c \
	compress/zstd_compress.c \
	compress/zstd_compress_literals.c \
	compress/zstd_compress_sequences.c \
	compress/zstd_compress_superblock.c \
	compress/zstd_fast.c \
	compress/zstd_double_fast.c \
	compress/zstd_lazy.c \
	compress/zstd_opt.c \
	compress/zstd_ldm.c \
	compress/zstdmt_compress.c \
	decompress/huf_decompress.c \
	decompress/zstd_decompress.c \
	decompress/zstd_decompress_block.c \
	decompress/zstd_ddict.c \
	dictBuilder/cover.c \
	dictBuilder/fastcover.c \
	dictBuilder/divsufsort.c \
	dictBuilder/zdict.c \
	decompress/huf_decompress_amd64.S

SOURCE_FILES := $(addprefix $(SRC_ROOT)/, $(CFILES))

DEFINES := \
	ZSTD_DLL_EXPORT=1 \
	ZSTD_LEGACY_SUPPORT=0 \
	ZSTD_STRIP_ERROR_STRINGS=1 \
	ZSTD_NO_ERRORLIB=1 \
	ZSTD_USE_XXH3=1 \
	MEM_FORCE_MEMORY_ACCESS=1
	
CFLAGS += $(addprefix -D, $(DEFINES))

CFLAGS += \
	-std=gnu2x \
	-g0 \
	-flto \
	-fno-exceptions \
	-fvisibility=hidden \
	-fno-semantic-interposition \
	-fmerge-all-constants \
	-mtune=znver3 \
	-fdata-sections \
	-ffunction-sections \
	-fomit-frame-pointer
	
LINK_FLAGS += \
	-Bsymbolic \
	--strip-all \
	--discard-all \
	--gc-sections \
	--relax \
	--exclude-libs,ALL \
	
comma:= ,

LINKER_PREFIX ?= -Wl$(comma)
	
LDFLAGS += $(addprefix $(LINKER_PREFIX), $(LINK_FLAGS))

CFLAGS_gcc := \
	-O3 \
	-fdelete-dead-exceptions \
	-fallow-store-data-races \
	-fdevirtualize-speculatively \
	-fdevirtualize-at-ltrans \
	-fipa-pta \
	-fgraphite-identity \
	-fivopts \
	-ftree-loop-im \
	-ftree-loop-ivcanon \
	-fstdarg-opt

CFLAGS_clang := \
	-Os \
	-Wno-assume \
	-fuse-ld=lld \
	-fvisibility-inlines-hidden \

LINK_FLAGS_gcc := \
	--strip-discarded
	
LDFLAGS_gcc += $(addprefix $(LINKER_PREFIX), $(LINK_FLAGS_gcc))

LINK_FLAGS_clang := \
		--icf=all \
		--ignore-data-address-equality \
		--ignore-function-address-equality
		
LDFLAGS_clang += $(addprefix $(LINKER_PREFIX), $(LINK_FLAGS_clang))

ifeq ($(shell zsh flags.sh $(CC)), gcc)
CFLAGS += $(CFLAGS_gcc)
LDFLAGS += $(LDFLAGS_gcc)
else ifeq ($(shell zsh flags.sh $(CC)), clang)
CFLAGS += $(CFLAGS_clang)
LDFLAGS += $(LDFLAGS_clang)
endif

OUT_NAME ?= libzstd
EXTENSION ?= .so

ALL_EXTENSIONS = .dll .so .dylib .gcc.so .clang.so

shared: $(OUT_NAME)$(EXTENSION)

$(OUT_NAME)$(EXTENSION): Makefile flags.sh $(SOURCE_FILES)
	rm -f $(OUT_NAME)$(EXTENSION); \
	$(CC) -shared -o $(OUT_NAME)$(EXTENSION) -fPIC $(CFLAGS) $(LDFLAGS) $(SOURCE_FILES); \
	strip --strip-debug --strip-unneeded --discard-all $(OUT_NAME)$(EXTENSION)
	
*.c:

*.S:

clean:
	rm -f $(OUT_NAME)$(EXTENSION) $(addprefix $(OUT_NAME), $(ALL_EXTENSIONS))

rebuild: clean shared
