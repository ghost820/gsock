#pragma once

#include <string>
#include <string_view>

namespace GSock
{
class Socket {
public:
    Socket(std::string_view address, unsigned int port);
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&&) = default;
    Socket& operator=(Socket&&) = default;

    ssize_t send(std::string_view msg);
    std::string recv(size_t size);
    std::string recvAll(size_t size);

private:
    int fd;
};
}
