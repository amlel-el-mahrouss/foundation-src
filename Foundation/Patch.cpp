#include "Patch.h"

bool
write_patch(FILE** file, const char* filename, struct patch_info** contents, long count) {
	assert(count > 1);
	assert(filename != nullptr);
	assert(*file == nullptr);

	if (count < 1) return false;
	if (contents[0] == nullptr) return false; // better return there.
	if (filename == nullptr) return false;

	assert(*file == nullptr); // we want this.
	assert(fopen_s(file, filename, "w") == 0);

	log("Writing down the patch header->.\r\n");

	// step one.
	// write the header ->
	patch_header* header = new patch_header();
	assert(header);

	header->patch_epoch = get_epoch();
	assert(header->patch_epoch != 0);
	header->patch_edit_epoch = header->patch_epoch;

	long len{};
	for (long i = 0; i < count; i++)
	{
		assert(contents[i]);
		assert(contents[i]->buffer);

		len += (long)strlen(contents[i]->buffer);
		header->patch_count += 1;
	}

	header->patch_magic = PATCH_MAGIC;
	header->patch_start = sizeof(struct patch_header);
	header->patch_size = len;

	fwrite((const void*)header, sizeof(struct patch_entry), SEEK_CUR, *file);
	fflush(*file);
	
	const fpos_t start = sizeof(struct patch_header);
	fsetpos(*file, &start);

	struct patch_entry* entry = new patch_entry();
	assert(entry);

	// then write the entries
	for (int i = 0; i < count; i++)
	{
		memcpy(entry->e_name, contents[i]->i_name, 16);
		entry->e_line = contents[i]->i_line;
		entry->e_size = contents[i]->i_size;
		entry->e_where = (start + i) * sizeof(struct patch_header);
		
		fwrite((const void*)entry, sizeof(struct patch_entry), SEEK_CUR, *file);
		fflush(*file);
		log("Wrote entry..\r\n");
	}

	// after that write the raw content
	long end = (count + len) * sizeof(struct patch_header);
	for (long i = 0; i < count; i++)
	{
		end += i;
		fseek(*file, end, SEEK_END);
		for (long y = 0; y < strlen(contents[i]->buffer); y++)
		{
			fputc(contents[i]->buffer[y], *file);

		}
		
		fflush(*file);
		log("Wrote content..\r\n");
	}

	delete header;
	delete entry;

	return true;
}
