/*Ankeet Presswala
* CMSC417 
* Proj0 - UDP Client
*/
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<time.h>

#define SERVER_IP   "128.8.126.92"
#define DELIM       "#"

int main(int argc, char *argv[])
{
    
    int sockfd;
    struct sockaddr_in serv_addr, recv_addr;
    char *username, *message, *nonce, *raw_timestamp, *digest;
    char response[85];
    char data[256];
    socklen_t sin_size;
    time_t timestamp;
    char *timestring;
    
    //parse arguments
    if (argc<3){
        printf("usage: %s <username> <message>\n", argv[0]);
        exit(1);
    }
    username = argv[1];
    message = argv[2];

    //should ensure that the final message size is no larger than 256
    if ((strlen(username) + strlen(message)) > 255){
        printf("Combined user/message size too large.\n");
        exit(1);
    }
 
    /* Create a UDP socket */
    // Remember to use SOCK_DGRAM and IPPROTO_UDP
    if ((sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
        printf("Error creating socket.");
        exit(1);    
    }

    /* Create the server's address structure */
    // family macro, htons port (turns from host-encoding to network encoding),
    // inet_pton for the server address and memset the zero field
    
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_port = htons(4567);  
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr); 
    memset(&(serv_addr.sin_zero), '\0', 8);
    sin_size = sizeof(struct sockaddr_in);
    
//    printf("About to send %s#%s to %s - %lu\n", username, message, SERVER_IP, l_ip);

    strcpy(data, username);
    strcat(data, "#");
    strcat(data, message);
    
//    printf("Sending %s...\n", data);
    if (sendto(sockfd, &data, strlen(data), 0, (struct sockaddr *)&serv_addr, sin_size) == -1){
        printf("Error sending %s to %s.", data, SERVER_IP);
    }

    /* Receive the server's response */
    // have to use recvfrom 
//    printf("Receiving...\n");
    recvfrom(sockfd, response, 84, 0, (struct sockaddr *)&recv_addr, &sin_size);
    
    //should this have to check server response for length?
//    printf("%s\n", response);
    
    //parsing response
    //remember that strtok is destructive
    nonce = strtok(response, DELIM);
    digest = strtok(NULL, DELIM);
    raw_timestamp = strtok(NULL, DELIM);
    timestamp = (time_t) atoi(raw_timestamp);
    timestring = ctime(&timestamp);
    printf("nonce:%s timestamp:%s digest:%s\n", nonce, timestring, digest);
    close(sockfd);
    return 0;
}

