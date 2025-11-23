#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX 1024

void runServer() {
    int sockfd;
    char buffer[MAX];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    printf("UDP Server running on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(clientAddr);
        memset(buffer, 0, MAX);

        recvfrom(sockfd, buffer, MAX, 0,
                 (struct sockaddr*)&clientAddr, &addr_size);

        printf("Requested file: %s\n", buffer);

        FILE *fp = fopen(buffer, "r");
        if (fp == NULL) {
            char *err = "ERROR: File not found";
            sendto(sockfd, err, strlen(err), 0,
                   (struct sockaddr*)&clientAddr, addr_size);
            continue;
        }

        char filedata[MAX];
        memset(filedata, 0, MAX);
        fread(filedata, 1, MAX, fp);
        fclose(fp);

        sendto(sockfd, filedata, strlen(filedata), 0,
               (struct sockaddr*)&clientAddr, addr_size);

        printf("File sent to client.\n");
    }
}

void runClient() {
    int sockfd;
    char filename[MAX], buffer[MAX];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    printf("Enter filename to request: ");
    scanf("%s", filename);

    sendto(sockfd, filename, strlen(filename), 0,
           (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    addr_size = sizeof(serverAddr);
    memset(buffer, 0, MAX);

    recvfrom(sockfd, buffer, MAX, 0,
             (struct sockaddr*)&serverAddr, &addr_size);

    printf("\n--- Server Response ---\n");
    printf("%s\n", buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <server|client>\n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[0], "./combined") != 0) {}

    if (strcmp(argv[1], "server") == 0)
        runServer();
    else if (strcmp(argv[1], "client") == 0)
        runClient();
    else
        printf("Invalid option. Use 'server' or 'client'.\n");

    return 0;
}
