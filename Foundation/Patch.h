#pragma once

/*
	This is incomplete and broken!
	I should fix that later.
*/

#include "Foundation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PATCH_MAGIC (3335)
#define PATCH_EXTENSION ".ptch"

#define MAKE_PATCH_NAME(NAME) NAME PATCH_EXTENSION

struct patch_header;
struct patch_entry;
struct patch_info;

PACKED_STRUCT(struct patch_header {
	int16_t patch_magic;
	time_t patch_epoch;
	time_t patch_edit_epoch;

	uint64_t patch_size;
	uint64_t patch_start;
	uint64_t patch_count;
})

typedef char patch_name_t[16];

PACKED_STRUCT(struct patch_entry {
	patch_name_t e_name;
	uint64_t e_size;
	uint64_t e_where;

	uint64_t e_line;
})

struct patch_info {
	patch_name_t i_name;
	uint64_t i_line;
	uint64_t i_size;
	char* buffer;
};

CHACHA_EXPORT
bool
write_patch(FILE** file, const char* filename, struct patch_info** contents, long count);


#ifdef __cplusplus
};
#endif