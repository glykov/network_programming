#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUF_SIZE 256

int main(int argc, char **argv)
{
    struct sockaddr_in svaddr;
    int sfd;
    int j = 0;
    size_t msg_len;
    ssize_t num_bytes;
    char resp[BUF_SIZE];
    char msg[BUF_SIZE];

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s host-address msg...\n", argv[0]);
        exit(1);
    }

    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) {
        fprintf(stderr, "socket\n");
        exit(1);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0) {
        fprintf(stderr, "inet_pton failed for address '%s'\n", argv[1]);
        exit(1);
    }

    while (true) {
        fgets(msg, BUF_SIZE, stdin);
        msg_len = strlen(msg);
        if (sendto(sfd, msg, msg_len, 0, (struct sockaddr*)&svaddr, sizeof(svaddr)) != msg_len) {
            fprintf(stderr, "sendto\n");
            exit(1);
        }

        num_bytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        if (num_bytes == -1) {
            fprintf(stderr, "recvfrom\n");
            exit(1);
        }

        printf("Response %d: %.*s\n", ++j, (int)num_bytes, resp);
    }

    return 0;
}