
#include <cassert>
#include <iostream>
#include "libsocket.h"

namespace sock {

    const int Socket::minBuff = 32;

    Socket::Socket(int type, ADDRESS_FAMILY family, int protocol)
        : _sock(0), _isValid(createSocket(type, protocol)), _sockStruct(sockaddr_in()) {
        _sockStruct.sin_family = family;
    }
    Socket::Socket(SOCKET socket, sockaddr_in &sockStruct)
        : _sock(socket), _isValid(socket != INVALID_SOCKET), _sockStruct(sockStruct) {}
    Socket::Socket(const Socket &other)
        : _sock(other._sock), _isValid(other._isValid), _sockStruct(other._sockStruct) {}
    Socket::Socket(Socket &&other)
        : _sock(other._sock), _isValid(other._isValid), _sockStruct(other._sockStruct) {
            other._sock = 0;
            other._isValid = false;
            other._sockStruct = sockaddr_in();
        }

    bool Socket::createSocket(int type, int protocol) {
        _sock = socket(_sockStruct.sin_family, type, protocol);
        return _sock != INVALID_SOCKET;
    }

    void Socket::sbind(const char *address, int port) {
        validateSocket();
        
        _sockStruct.sin_addr.s_addr = inet_addr(address);
        _sockStruct.sin_port = htons(port);

        if (bind(_sock, (struct sockaddr *)&_sockStruct, sizeof(_sockStruct)) == SOCKET_ERROR)
            throw socket_action_error();
    }
    void Socket::slisten(int backlog) {
        validateSocket();

        if (listen(_sock, backlog) == SOCKET_ERROR)
            throw socket_action_error();
    }
    Socket Socket::saccept() {
        validateSocket();

        struct sockaddr_in client = sockaddr_in();
        socklen_t c = sizeof(struct sockaddr_in);
        SOCKET new_socket = accept(_sock, (struct sockaddr *)&client, &c);

        if (new_socket == INVALID_SOCKET)
            throw socket_action_error();

        return Socket(new_socket, client);
    }

    void Socket::sconnect(const char *address, int port) {
        validateSocket();
        
        _sockStruct.sin_addr.s_addr = inet_addr(address);
        _sockStruct.sin_port = htons(port);

        if (connect(_sock, (const struct sockaddr *)&_sockStruct, sizeof(_sockStruct)) < 0)
            throw socket_action_error();
    }
    void Socket::ssend(const char *message, size_type size) const {
        validateSocket();

        if (send(_sock, message, size, 0) < 0)
            throw socket_action_error();
    }
    const char *Socket::srecv(int bufferSize) const {
        assert(bufferSize > Socket::minBuff && "buffer cannot be less than Socket::minBuff");

        char *data = new char[bufferSize];
        try { srecv(data, bufferSize); }
        catch(...) { delete[] data; throw; }
        delete[] data;

        return data;
    }
    int Socket::srecv(char *buffer, size_type size) const {
        validateSocket();

        int recvSize = recv(_sock, buffer, size - 1, 0);

        if (recvSize == SOCKET_ERROR) 
            throw socket_action_error();
        
        buffer[recvSize] = '\0';
        return recvSize;
    }
    void Socket::sclose() {
        if (_isValid) {
    #ifdef _WINDOWS
            closesocket(_sock);
    #else
            close(_sock);
    #endif // _WINDOWS
            _isValid = false;
        }
    }

    Socket &Socket::operator=(const Socket &rhs) {
        if (this != &rhs) {
            _sock = rhs._sock;
            _isValid = rhs._isValid;
            _sockStruct = rhs._sockStruct;
        }
        return *this;
    }
    Socket &Socket::operator=(Socket &&rhs) {
        if (this != &rhs) {
            _sock = rhs._sock;
            _isValid = rhs._isValid;
            _sockStruct = rhs._sockStruct;

            rhs._sock = 0;
            rhs._isValid = false;
            rhs._sockStruct = sockaddr_in();
        }
        return *this;
    }

    bool Socket::operator==(const Socket &rhs) const {
        return _sockStruct.sin_family == rhs._sockStruct.sin_family
            && _sockStruct.sin_port == rhs._sockStruct.sin_port
            && strcmp(getAddress(), rhs.getAddress()) == 0;
    }

    std::ostream &operator<<(std::ostream &output, const Socket &source) {
        output << "(";
        output << source.getAddress();
        output << ":";
        output << source.getPort();
        output << ")";

        return output;
    }
}