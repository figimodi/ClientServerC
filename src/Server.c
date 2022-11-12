#include "../include/unp.h"

unsigned short checksumAdd(void *newBuffer, int size); // return the sum of the byte of newBuffer
void error(char *err);

// ./Server [server port number]
int main(int argc, char **argv)
{
    int nByte, socketDesc, clientLen; // Number od bytes received/sent, Socket descriptor, Size of client data structure
    int HTTPHeaderSize = strlen(HTTPHeader) + 1; // Size of the HTTP header
    int UDPHeaderSize = sizeof(UDPHeader); // Size of the UDP header
    int payloadSize = BUFF_SIZE - HTTPHeaderSize - UDPHeaderSize; // Size of the content
    unsigned short i; // It will rapresent the segment number
    char dataPath[MAX_LINE]; // Path of the html file
    char bufferSend[BUFF_SIZE]; // Buffer to send message
    char bufferReceive[BUFF_SIZE]; // Buffer to receive message
    struct sockaddr_in server, client; // Server and client structures
    Request request; // Contains the information about the request sent by the client
    FILE *html; // The file to send

    if (argc != 2)
        error("Error: Wrong parameters\n");

    // Reset the memory to avoid any further problems with the following structures (server and client) 
    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));

    // Save the size of the structure client
    clientLen = sizeof(client);

    // Set up server's informations
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons((short)atoi(argv[1]));

    // Initialize the socket
    socketDesc = socket(AF_INET, SOCK_DGRAM, 0);

    // Try to bind the socket
    if (bind(socketDesc, (struct sockaddr *)&server, sizeof(server)) < 0)
        error("Error binding the socket\n");

    // Waiting to receive the first packet (request) from the client
    nByte = recvfrom(socketDesc, bufferReceive, BUFF_SIZE, 0, (struct sockaddr *)&client, &clientLen);
    bufferReceive[nByte] = '\0';
    printf("Packet Received:\n%s\n\n", bufferReceive);

    // Store request informations into variables
    sscanf(bufferReceive, "%s %s %s", &(request.method), &(request.url), &(request.protocol));

    if (strcmp(request.method, "GET") != 0)
        error("Error: method request doesn't exist!\n");

    // Set the path to the html file
    strcpy(dataPath, path);
    strcat(dataPath, request.url);

    // Open the file
    if ((html = (FILE *)fopen(dataPath, "r")) == NULL)
        error("Error: file doesn't exist\n");

    for (i = 0; ; i++)
    {
        int j; // Track the lenght of the payload
        int end = 0; // indicate if the file read reach the end
        char bufferHTTPHeader[HTTPHeaderSize]; // Buffer for the HTTP header
        char bufferPayload[payloadSize]; // Buffer of the payload
        UDPHeader udpHeader; // UDP header structure 

        // Set udpHeader to the initial values
        udpHeader.checksum = 0;
        udpHeader.segNum = i;

        // Clear buffers
        memset(bufferSend, 0, sizeof(bufferSend));
        memset(bufferHTTPHeader, 0, sizeof(bufferHTTPHeader));

        for (j = 0; j < payloadSize && end == 0; j++)
        {
            char c;
            if (fscanf(html, "%c", &c) == EOF)
            {
                // End of the file
                end = 1;
                bufferPayload[j] = '\0';

                // Close the file
                fclose(html);
            }
            else
            {
                // Copy the from file to local buffer and calculate the byte value of the char for the chceksum
                bufferPayload[j] = c;
                udpHeader.checksum += (unsigned short)c;
            }
        }

        // Write the size of the payload into the HTTP header 
        sprintf(bufferHTTPHeader, HTTPHeader, j);

        // Add the relative checksum of the HTTP header and the segment number of UDP header bytes
        udpHeader.checksum += checksumAdd(bufferHTTPHeader, strlen(bufferHTTPHeader));
        udpHeader.checksum += checksumAdd(&(udpHeader.segNum), sizeof(udpHeader.segNum));

        // Copy everything into buffer send (UDP header + HTTP header + payload)
        memcpy((void *)bufferSend, (void *)&udpHeader, sizeof(UDPHeader));
        memcpy((void *)bufferSend + sizeof(UDPHeader), (void *)bufferHTTPHeader, strlen(bufferHTTPHeader));
        memcpy((void *)bufferSend + sizeof(UDPHeader) + strlen(bufferHTTPHeader), (void *)bufferPayload, j);

        // Sending the packet ...
        printf("Sending packet number %d...", i);

        // Send the packet
        nByte = sendto(socketDesc, (char *)bufferSend, sizeof(UDPHeader) + strlen(bufferHTTPHeader) + strlen(bufferPayload), 0, (const struct sockaddr *)&client, clientLen);

        if (nByte < 0)
            error("Error sending the packet\n");
        
        printf("Packet sent\n");

        // Code for the last packet
        if (end != 0)
        {
            printf("Sending last packet...\n");

            // We send only '\0' to communicate the transmission is finished
            nByte = sendto(socketDesc, "\0", 1, 0, (const struct sockaddr *)&client, clientLen);

            if (nByte < 0)
                error("Error sending the packet\n");

            printf("Final packet sent\n");

            break;
        }
    }

    // Close the socket
    close(socketDesc);
    return 0;

}

// return the sum of the byte of newBuffer
unsigned short checksumAdd(void *newBuffer, int size)
{
    char localBuffer[size];
    unsigned short checksumToAdd = 0;
    int j;

    // Copy the bytes into a char array
    memcpy(localBuffer, newBuffer, size);

    for (j = 0; j < size; j++)
    {
        // Sum the int ascii value of each buffer's char
        checksumToAdd += (unsigned short)localBuffer[j];
    }

    return checksumToAdd;
}

void error(char *err)
{
    perror(err);
    exit(0);
}