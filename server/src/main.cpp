#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

int main()
{
    int serverFd, clientFd;

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1)
    {
        perror("Error creating socket");
        return 1;
    }

    int opt = 1;
    if (-1 == setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("Error setting socket");
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if (bind(serverFd, reinterpret_cast<struct sockaddr*>(&address), addrlen) < 0)
    {
        perror("Bind failed");
        close(serverFd);
        return 1;
    }

    if (listen(serverFd, 3) < 0)
    {
        perror("listen failed");
        close(serverFd);
        return 1;
    }
    std::cout << "Server listening on port " << PORT << "...\n";

    clientFd = accept(serverFd, reinterpret_cast<struct sockaddr*>(&address), &addrlen);
    if (clientFd < 0)
    {
        perror("accept failed");
        close(serverFd);
        return 1;
    }
    std::cout << "Client connected\n";

    int bytesRead = 0;
    while ((bytesRead = read(clientFd, &buffer, BUFFER_SIZE)) > 0)
    {
        std::cout << "Received: " << buffer;
        send(clientFd, buffer, bytesRead, 0);
        memset(buffer, 0, BUFFER_SIZE);
    }

    std::cout << "Client disconnected\n";

    close(clientFd);
    close(serverFd);

    return 0;
}