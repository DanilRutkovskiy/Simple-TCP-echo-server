#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

int main()
{
    int serverFd;
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1)
    {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) 
    {
        perror("Invalid address");
        close(serverFd);
        return 1;
    }

    if (connect(serverFd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0)
    {
        perror("Connection failed");
        close(serverFd);
        return 1;
    }

    char buffer[BUFFER_SIZE] = {0};

    while(true)
    {
        std::cout << "> ";
        std::cin.getline(buffer, BUFFER_SIZE);

        if (strlen(buffer) == 0)
        {
            break;
        }

        if (send(serverFd, buffer, strlen(buffer), 0) < 0)
        {
            perror("Send error");
            close(serverFd);
            return 1;
        }

        auto bytesReceived = recv(serverFd, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived < 0)
        {
            perror("Receive error");
            close(serverFd);
            return 1;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Echo: " <<  buffer << "\n";
    }

    close(serverFd);

    return 0;
}