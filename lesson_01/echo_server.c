#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUF_SIZE 128

int main(int argc, char **argv)
{
    struct sockaddr_in svaddr, claddr;
    struct hostent *h;
    int sfd;
    ssize_t num_bytes;
    socklen_t len;
    char buf[BUF_SIZE];
    char claddr_str[INET_ADDRSTRLEN];

    // получаем порт для прослушивания сервером из командной строки
    if (argc != 2) {
        fprintf(stderr, "usage: %s, <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const int port = atoi(argv[1]);

    // дескриптор сокета UDP
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) {
        fprintf(stderr, "socket failure\n");
        return EXIT_FAILURE;
    }

    printf("starting echo server on port %d...\n", port);

    // заполняем структуру sockaddr_in для сервера
    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family = AF_INET;
    svaddr.sin_addr.s_addr = INADDR_ANY;
    svaddr.sin_port = htons(port);

    // привязываем дескриптор сокета к адресу
    if (bind(sfd, (struct sockaddr*)&svaddr, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "bind fials\n");
        return EXIT_FAILURE;
    }

    printf("Running echo server...\n");

    while (true) {
        len = sizeof(struct sockaddr_in);
        // читаем данные от клиента
        num_bytes = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr*)&claddr, &len);
        if (num_bytes == -1) {
            fprintf(stderr, "recvfrom fails\n");
            return EXIT_FAILURE;
        }

        // проверяем на условие выхода
        if (memcmp(buf, "exit", 4) == 0) {
            printf("finishing session...\n");
            break;
        }

        // получаем адрес коиента
        char domain[256];
        h = gethostbyaddr((char*)&claddr.sin_addr.s_addr, sizeof(struct in_addr), AF_INET);
        if (h != (struct hostent *)0) {
            strcpy(domain, h->h_name);
        } else {
            printf("hostbyaddr fails\n");
        }

        printf("Server received %ld bytes from (%s, %u)\n", (long)num_bytes, domain, ntohs(claddr.sin_port));

        // отправляем строку обратно клиенту
        if (sendto(sfd, buf, num_bytes, 0, (struct sockaddr*)&claddr, len) != num_bytes) {
            fprintf(stderr, "sendto fails\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}