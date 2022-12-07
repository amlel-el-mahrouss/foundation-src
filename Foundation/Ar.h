#pragma once

/*
    Slasher Archive format
    A tiny format made to archive and pack files together.
*/

#include "Foundation.h"

#ifdef __cplusplus
extern "C" {
#endif

constexpr auto AR_MAGIC = 0xBADF00D;

#define AR_EXTENSION ".ar"
#define AR_MAKE_EXTENSION(NAME) NAME AR_EXTENSION


struct ar_extension_table;
struct ar_header;

typedef enum {
    AR_FLAG_EXEC, // one of the files, is a PE+ executable.
    AR_FLAG_ZLIB, // one of the files, is a zlib archive.
    AR_FLAG_PASSWORD, // one of the files has a password.
} AR_FLAGS;

PACKED_STRUCT(struct ar_extension_table {
    uint64_t start;
    uint64_t count;
    uint64_t size;
    uint32_t flags;
})

/*
    Stripped down tar header
*/

PACKED_STRUCT(struct ar_header {
    uint64_t magic;
    time_t edit_epoch;
    time_t create_epoch;
    size_t size;
    struct ar_extension_table extension_table;
})

#define AR_HDR_SZ (sizeof(struct ar_header))

typedef enum {
    AR_COMPRESSION_TYPE_NETCOMP, // Network compressed.
    AR_COMPRESSION_TYPE_COUNT,
} AR_COMPRESSION_TYPE;

typedef FILE ar_file_t;

PACKED_STRUCT(struct ar_context{
    ar_file_t * fp;
    struct ar_header header;
})

/* We provide our own way to tell in a file what is the end of a record. */

#define AR_FILE_RESTRICT_W ("wb")
#define AR_FILE_RESTRICT_R ("rb")

#define AR_END_OF_RECORD ('\0')

CHACHA_EXPORT size_t
ar_fast_strlen(const unsigned char* deflated, size_t sz);

CHACHA_EXPORT void
ar_update_epoch(struct ar_context* ctx);

CHACHA_EXPORT struct ar_context*
ar_new(const char* path, const char* res);

CHACHA_EXPORT size_t
ar_len(const unsigned char* bytes, size_t sz);

CHACHA_EXPORT void
ar_update_header_info(struct ar_context* ctx, const unsigned char* bytes, size_t sz);

CHACHA_EXPORT bool
ar_write(struct ar_context* ctx, const unsigned char* bytes, fpos_t off, size_t sz);

CHACHA_EXPORT bool
ar_read(struct ar_context* ctx, unsigned char* bytes, size_t off, size_t sz);

CHACHA_EXPORT bool
ar_flush(struct ar_context* ctx);

CHACHA_EXPORT bool
ar_close(struct ar_context* ctx);

#ifdef __cplusplus
};
#endif