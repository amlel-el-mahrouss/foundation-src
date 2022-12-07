#include <iostream>
#include <Https.h>

#define EHTTPALLOC (-3)
#define EUNIMPL (-2)

int main(int argc, char** argv)
{
    using namespace Foundation;

    if (argc == 4) {
        if (!strcmp(argv[1], "/GET")) {
            std::string path = argv[2];
            std::string host = argv[3];
            std::string request = http_helpers::make_get(path, host);

            http_writer writer;
            http::http_header* hdr = new http::http_header{
                .Type = http::http_request_type::GET,
                .Bytes = request.c_str(),
                .SizeOfBytes = (int64_t)request.size(),
            };

            if (!hdr) std::exit(EHTTPALLOC);

            non_nil<http::http_header*> hdr_wrapper{ hdr };

            auto sock = writer.create_socket_and_connect(host);
            writer.send_from_socket(sock, hdr_wrapper);

            std::ofstream output{ "output.http" };

            char* bytes = new char[1000000];
            memset(bytes, 0, 1000000);
            
            writer.read_from_socket(sock, bytes, 1000000);

            output << bytes;

#ifdef _DEBUG
            std::cout << bytes << "\n";
#endif

            output.flush();
            output.close();

            delete hdr;
        } else if (!strcmp(argv[1], "/POST")) {
            std::cout << "POST is unsupported for now.\n";
            std::exit(EUNIMPL);
        }
    }
}
