#include "Socket.h"

#include <system_error>

#include <cerrno>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace GSock
{
Socket::Socket(std::string_view address, unsigned int port)
{
    if (fd = socket(AF_INET, SOCK_STREAM, 0); fd == -1) {
        throw std::system_error(errno, std::generic_category());
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.data());
    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        close(fd);
        throw std::system_error(errno, std::generic_category());
    }
}

Socket::~Socket()
{
    close(fd);
}

ssize_t Socket::send(std::string_view msg)
{
    while (true) {
        ssize_t bytesSent = ::send(fd, msg.data(), msg.size(), 0);
        if (bytesSent == -1) {
            if (errno == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::generic_category());
        }
        return bytesSent;
    }
}

std::string Socket::recv(size_t size)
{
    std::string buffer(size, '\0');

    while (true) {
        ssize_t bytesRead = ::recv(fd, buffer.data(), size, 0);
        if (bytesRead == 0) {
            throw std::system_error(EIO, std::generic_category(),
                "The connection was terminated before data transfer was completed");
        }
        else if (bytesRead == -1) {
            if (errno == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::generic_category());
        }
        buffer.resize(bytesRead);
        return buffer;
    }
}

std::string Socket::recvAll(size_t size)
{
    std::string buffer(size, '\0');
    char* bufferPtr = buffer.data();

    while (size > 0) {
        ssize_t bytesRead = ::recv(fd, bufferPtr, size, 0);
        if (bytesRead == 0) {
            throw std::system_error(EIO, std::generic_category(),
                "The connection was terminated before data transfer was completed");
        }
        else if (bytesRead == -1) {
            if (errno == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::generic_category());
        }

        size -= bytesRead;
        bufferPtr += bytesRead;
    }

    return buffer;
}
}
