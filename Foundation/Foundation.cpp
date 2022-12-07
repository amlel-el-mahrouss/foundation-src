#include "Foundation.h"

static FILE* stdlog{ nullptr };

CHACHA_EXPORT
FILE* get_log_fd(void) {
	assert(stdlog);
	return stdlog;
}

size_t 
fstrlen(const char* buffer) {
	if (!buffer) return 0;
	if (*buffer == END_OF_BUFFER) return 0;

	size_t index = 0;
	while (buffer[index] != END_OF_BUFFER) ++index;

	return index;
}

time_t 
get_epoch() {
	static time_t curtime{};
	curtime = time(&curtime);
	// do not check anything here, it is source of race conditions.
	
	return curtime;
}

void
log(const char* msg) {
#ifndef NDEBUG
	char buf[sizeof(time_t)];
	snprintf(buf, sizeof(time_t), "%llu", get_epoch());
	fprintf(stdlog, "[%s - LOG] %s", buf, msg);
#endif // !NDEBUG

}

char dbg_filename[256];

bool 
open_log_file() {
#ifndef NDEBUG
	snprintf(dbg_filename, 256, "%llu_dbg.log", get_epoch());
	if (fopen_s(&stdlog, dbg_filename, "w+") != EXIT_SUCCESS) {
		assert(false);
		exit(EXIT_FAILURE);

		return false;
	}

	return true;
#endif // !NDEBUG

	return false;
}