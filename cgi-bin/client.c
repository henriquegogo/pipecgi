#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>

#define REGISTRATION "channels"
#define TIMEOUT 5

int main() {
    char channel[20];
    char body[1024]; // Adjust size as needed
    int fifo_fd, bytes_read;

    // Create unique channel (FIFO)
    snprintf(channel, sizeof(channel), "%d", getpid());
    mkfifo(channel, 0666);

    // Register the channel
    FILE *reg = fopen(REGISTRATION, "w");
    if (reg) {
        fprintf(reg, "%s\n", channel);
        fclose(reg);
    }

    // Read request body from stdin
    bytes_read = read(STDIN_FILENO, body, sizeof(body) - 1);
    body[bytes_read] = '\0'; // Null-terminate the body

    // Write HTTP-like request to FIFO
    fifo_fd = open(channel, O_WRONLY);
    if (fifo_fd >= 0) {
        dprintf(fifo_fd, "%s %s %s\n", getenv("REQUEST_METHOD"), getenv("REQUEST_URI"), getenv("SERVER_PROTOCOL"));
        dprintf(fifo_fd, "Host: %s\n", getenv("HTTP_HOST"));
        dprintf(fifo_fd, "User-Agent: %s\n", getenv("HTTP_USER_AGENT"));
        dprintf(fifo_fd, "Accept: %s\n", getenv("HTTP_ACCEPT"));
        dprintf(fifo_fd, "Accept-Language: %s\n", getenv("HTTP_ACCEPT_LANGUAGE"));
        dprintf(fifo_fd, "Accept-Encoding: %s\n", getenv("HTTP_ACCEPT_ENCODING"));
        dprintf(fifo_fd, "Content-Type: %s\n", getenv("CONTENT_TYPE"));
        dprintf(fifo_fd, "Content-Length: %lu\n\n", strlen(body));
        dprintf(fifo_fd, "%s\n", body);
        close(fifo_fd);
    }

    // Open FIFO for reading
    fifo_fd = open(channel, O_RDONLY | O_NONBLOCK);
    if (fifo_fd >= 0) {
        fd_set set;
        struct timeval timeout;
        FD_ZERO(&set);
        FD_SET(fifo_fd, &set);

        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;

        // Wait for data to become available with timeout
        int rv = select(fifo_fd + 1, &set, NULL, NULL, &timeout);
        if (rv > 0) { // Data is available to read
            // Switch to blocking mode to read the full response
            fcntl(fifo_fd, F_SETFL, fcntl(fifo_fd, F_GETFL) & ~O_NONBLOCK);

            // Read and print the full response until EOF or no more data
            while ((bytes_read = read(fifo_fd, body, sizeof(body) - 1)) > 0) {
                body[bytes_read] = '\0';
                printf("%s", body); // Output the response
            }
        } else if (rv == 0) { // Timeout occurred
            printf("Request timed out after %d seconds.\n", TIMEOUT);
        }
        close(fifo_fd);
    }

    // Clean up
    unlink(channel);

    return 0;
}
