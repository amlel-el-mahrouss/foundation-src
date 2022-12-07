#include <iostream>
#include <Ar.h>

int main(int argc, char** argv)
{
    if (argc < 2)
        return -1;

    if (argc > 16)
        return -2;

#ifndef NDEBUG
    open_log_file();

    atexit([]() -> void {
        if (get_log_fd()) {
            fflush(get_log_fd());
            fclose(get_log_fd());
        }
        });
#endif

    std::cout << "Creating addonArchive...\n";
    struct ar_context* ctx = ar_new(AR_MAKE_EXTENSION("addonArchive"), AR_FILE_RESTRICT_W);
    size_t off = AR_HDR_SZ;

    for (size_t i = 1; i < argc; i++)
    {
        FILE* file = fopen(argv[i], "r");
        if (!file) goto free_res;
        std::cout << "Opened!  :" << argv[i] << "\n";
        fseek(file, 0L, SEEK_END);
        size_t sz = ftell(file);
        assert(sz != 0);

        unsigned char* bytes = new unsigned char[sz];
        std::cout << "Size of file :" << sz << "\n";
        memset(bytes, 0, sz);

        fread((void*)bytes, sz, SEEK_SET, file);

        if (!ar_write(ctx, bytes, off, sz)) {
            std::cout << "Could not write archive?\n";
            goto free_res;
        }

        ar_flush(ctx);
        off += sz;

        fclose(file);
        delete bytes;
    }

    std::cout << "Done!\n";

    goto free_res;

free_res:
    ar_close(ctx);
    return -1;


    return 0;
}
