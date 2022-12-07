#pragma once

#include <WinSock2.h>
#include "Foundation.h"

#pragma comment(lib, "Ws2_32.lib")

namespace Foundation {
    constexpr int HTTP_PORT = 80;

    class mime_factory;
    class http_exception;
    class http_helpers;
    class http_writer;

    class mime_factory final {
    public:
        struct mime final {
            std::string t_name;
            std::string t_mime;
        };

    public:
        static mime_factory::mime get(char* name) {
            if (!name) return { .t_name = "Any File", .t_mime = "*/*" };
            std::string extension = strchr(name, '.');

            if (!strcmp(extension.c_str(), ".png"))
                return { .t_name = "PNG Image", .t_mime = "Content-Type: image/png" };
            else if (!strcmp(extension.c_str(), ".bmp"))
                return { .t_name = "Bitmap Image", .t_mime = "Content-Type: image/bmp" };
            else if (!strcmp(extension.c_str(), ".jpg"))
                return { .t_name = "JPEG", .t_mime = "Content-Type: image/jpeg" };
            else if (!strcmp(extension.c_str(), ".ar"))
                return { .t_name = "Package Slasher", .t_mime = "Content-Type: application/slasher-ar" };
            else if (!strcmp(extension.c_str(), ".exe") || !strcmp(extension.c_str(), ".dll"))
                return { .t_name = "PE+", .t_mime = "Content-Type: application/msexecutable" };

            return { .t_name = "File Not Allowed", .t_mime = "Content-Type: */not-allowed" };
        }

    };

    namespace http {
        class http_socket final {
        private:
            struct sockaddr_in m_Addr{ };
            std::string m_Dns{ "" };
            SOCKET m_Socket{ INVALID_SOCKET };

            friend Foundation::http_writer;

        public:
            http_socket() = default;
            ~http_socket() = default;

            http_socket& operator=(const http_socket&) = default;
            http_socket(const http_socket&) = default;


        };

        enum class http_request_type {
            GET,
            POST,
            NOT_ALLOWED,
        };

        struct http_header final {
            http_request_type Type;
            const char* Bytes;
            int64_t SizeOfBytes;

        };

    }

    class http_exception : public std::runtime_error {
    public:
        http_exception(const std::string& what) : std::runtime_error(what) {}
        ~http_exception() = default; // let the ABI define that.

        http_exception& operator=(const http_exception&) = default;
        http_exception(const http_exception&) = default;

    };

    class http_helpers {
    public:

        static std::string make_get(const std::string& path, 
            const std::string& host,
            const char* user_agent = "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36\n"
        ) {
            if (path.empty()) return "";
            if (host.empty()) return "";

            std::string request = "GET " + path + " HTTP/1.1\n";
            request += "Host: " + host + "\n";
            request += user_agent;
            request += "\n";
            return request;
        }

        static bool has_field(std::string& http, std::string & r) {
            if (http.empty()) return false;
            if (r.empty()) throw std::runtime_error("Bad restrict");

            return http.find(r) != std::string::npos;
        }

        template <int Base>
        static int content_length(std::string& http) {
            size_t at = http.find("Content-Length: ");
            if (at == std::string::npos) return -1;

            std::string final;

            for (size_t first = at; first < http.size(); ++first)
            {
                if (http[first] == '\r')
                    break;

                if (http[first] >= '0' && http[first] <= '9') {
                    final += http[first];
                }
            }

            return std::stoi(final, nullptr, Base);
        }

    };

    using http_ptr = std::shared_ptr<http::http_socket>;
    using http_vector = std::vector<std::shared_ptr<http::http_socket>>;
    using http_priv_socket_data = WSADATA;

    class http_writer final {
    public:
        explicit http_writer() {
            int result = WSAStartup(MAKEWORD(2, 2), &m_WsaData);
            if (result != 0) {
#ifndef NDEBUG
                assert(false);
#endif

                throw http_exception("Windows Socket startup failed!");
            }
        }

        ~http_writer() noexcept {
            for (size_t i = 0; i < m_Socket.size(); i++)
            {
                assert(shutdown(m_Socket[i]->m_Socket, SD_SEND) == 0);
                assert(closesocket(m_Socket[i]->m_Socket) == 0);

                printf("%s Has been closed!", m_Socket[i]->m_Dns.data());
            }
        }

        http_writer& operator=(const http_writer&) = default;
        http_writer(const http_writer&) = default;

        http_ptr create_socket_and_connect(const std::string& dns) {
            if (dns.empty()) throw http_exception("Invalid DNS! string is empty!");

            http_ptr sock = std::make_unique<http::http_socket>();
            if (!sock) return nullptr;
            
            sock->m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock->m_Socket == INVALID_SOCKET) throw http_exception("Invalid Socket, check at call trace.");

            ZeroMemory(&sock->m_Addr, sizeof(struct sockaddr_in));

            sock->m_Addr.sin_family = AF_INET;
            sock->m_Addr.sin_addr.s_addr = inet_addr(dns.c_str());

            if (sock->m_Addr.sin_addr.s_addr == INADDR_NONE)
            {
                struct hostent* host = gethostbyname(dns.c_str());
                if (!host)
                {
                    closesocket(sock->m_Socket);
   
                    return nullptr;
                }

                sock->m_Addr.sin_addr.s_addr = *((u_long*)host->h_addr);
            }

            sock->m_Addr.sin_port = htons(HTTP_PORT);
            sock->m_Dns = std::string{ dns.data() };

            int result = connect(sock->m_Socket, reinterpret_cast<SOCKADDR*>(&sock->m_Addr), sizeof(sock->m_Addr));
            if (result == SOCKET_ERROR) throw http_exception("Invalid Socket! Unknown host.");

            m_Socket.push_back(sock);
            return sock;
        }

        int64_t send_from_socket(http_ptr& sock, non_nil<http::http_header*>& hdr) {
#ifndef NDEBUG
            assert(sock);
            assert(hdr);
            assert(!sock->m_Dns.empty());
            assert(hdr->Type != http::http_request_type::NOT_ALLOWED);
#endif

            int64_t result{ -1 };
            if (result = send(sock->m_Socket, hdr->Bytes, hdr->SizeOfBytes, 0) != hdr->SizeOfBytes)
                throw http_exception("Bad send.");

            return result;
        }

        bool read_from_socket(http_ptr& sock, char* bytes, size_t len) {
            if (!sock) throw http_exception("Bad socket");
            if (!bytes) throw http_exception("Bad buffer");

#ifndef NDEBUG
            assert(sock);
            assert(!sock->m_Dns.empty());
#endif

            int64_t data_length{ -1 };
            data_length = recv(sock->m_Socket, bytes, len, 0);

            return data_length != 0;
        }

    private:
        http_vector m_Socket;
        http_priv_socket_data m_WsaData;

    };
}