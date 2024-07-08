#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Function to check if three numbers form a Pythagorean triple
int isPythagoreanTriple(int a, int b, int c) {
    // Sort the numbers to ensure a^2 + b^2 = c^2
    if (a > b) {
        int temp = a;
        a = b;
        b = temp;
    }
    if (b > c) {
        int temp = b;
        b = c;
        c = temp;
    }
    if (a > b) {
        int temp = a;
        a = b;
        b = temp;
    }

    return (a * a + b * b == c * c);
}

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // Initialize socket structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Bind the host address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    // Start listening for the clients
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    printf("Server listening on port %d\n", PORT);

    // Accept actual connection from the client
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");

    // Read data from client
    int counter = 0;
    while (1) {
        printf("iteration count %d\n", counter);
        counter++;
        bzero(buffer, BUFFER_SIZE);
        n = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) error("ERROR reading from socket");

        if (n > 0) {
            // Convert received data to integers
            int num = atoi(buffer);
            printf("Received number from client: %d\n", num);
            
            // Store the number in an array to keep track of the last three numbers
            static int last_three[3] = {0,0,0};

            // Shift elements in the array to make space for the new number
            last_three[0] = last_three[1];
            last_three[1] = last_three[2];
            last_three[2] = num;

            // Check if the last three numbers form a Pythagorean triple
            if (last_three[0] != 0 && last_three[1] != 0 && last_three[2] != 0) {
                if (isPythagoreanTriple(last_three[0], last_three[1], last_three[2])) {
                    n = write(newsockfd, "true", 4); // Respond with "true"
                } else {
                    n = write(newsockfd, "false", 5); // Respond with "false"
                }
                if (n < 0) error("ERROR writing to socket");
            }
            else{
                write(newsockfd, "enter next number", 17);
            }
            printf("iteration count %d finish\n", counter);

        }
    }

    // Close sockets
    close(newsockfd);
    close(sockfd);
    return 0;
}
