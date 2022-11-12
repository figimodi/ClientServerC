#include "../include/unp.h"

unsigned short checksumAdd(void *newBuffer, int size); // return the sum of the byte of newBuffer
void gremlin(char *packet, int size, double probability); // generate an error in packet with the given probability (0 <= probability <= 1)
void error(char *err);

// ./Client [server IP (dot notation)] [server port number] [probability to generate an error in a packet (0 <= p <= 1)]
int main(int argc, char **argv)
{
    int nByte, socketDesc, serverLen; // Number od bytes received/sent, Socket descriptor, Size of server data structure
    char bufferSend[BUFF_SIZE]; // Buffer to send message
    char bufferReceive[BUFF_SIZE]; // Buffer to receive message 
    struct sockaddr_in server; // Server structures
    unsigned long serverIP; // IP of the server
    FILE *receivedFile; // File that will contain the file sent by the server
    LISTA packetList = LISTAinit(); // Ordered list that will contains all the pakcets in order

    if (argc != 4)
        error("Error: Wrong parameters.\n");

    // Reset the memory to avoid any further problems with server's structure 
    memset(&server, 0, sizeof(server));

    // Set up server's informations
    server.sin_family = AF_INET;
    server.sin_port = htons((short)atoi(argv[2]));
    serverIP = inet_addr(argv[1]);
    bcopy(&serverIP, &(server.sin_addr), sizeof(serverIP));

    // Initialize the socket
    socketDesc = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketDesc < 0)
        error("Error: Couldn't initialize the socket\n");

    // Send the request to the server
    strcpy(bufferSend, "GET TestFile.html HTTP/1.0");
    nByte = sendto(socketDesc, bufferSend, BUFF_SIZE, 0, (const struct sockaddr *)&server, sizeof(server));

    if (nByte < 0)
        error("Error sending the message\n");

    printf("Request sent\n");

    // Keep receiving packet from the server
    while ((nByte = recvfrom(socketDesc, bufferReceive, BUFF_SIZE, 0, (struct sockaddr *)&server, &serverLen)) > 0)
    {
        int statusCode, payloadSize; // The status code of the response, the lenght of the payload received
        unsigned short localChecksum = 0; // Local checksum, locally calculated
        char bufferPayload[BUFF_SIZE]; // Buffer of the payload
        UDPHeader udpHeader; // UDP header structure
        PACKET packet; // Packet structure in Packet.c

        printf("Packet of size %d received\n", nByte);

        // Check if it's the last packet (which contatins only '\0')
        if (nByte == 1 && bufferReceive[0] == '\0')
        {
            printf("Last packet received\n\nCOMPLETE FILE RECEIVED\n\n");
            break;
        } 

        // Retrive the udpHeader data structure from the beginning of the packet
        memcpy(&udpHeader, bufferReceive, sizeof(UDPHeader));

        // Retrive from the HTTP header informations about the status code and the payload size
        sscanf(bufferReceive + sizeof(UDPHeader), "%*s %d %*s %*s %*s %*s %*s %d", &statusCode, &payloadSize);

        // Verify the status code
        if(statusCode != 200)
            error("Error: Status code isn't 200.\n");

        // Damage the packet with the probabilty given as an argument
        gremlin(bufferReceive, nByte, (float)atof(argv[3]));

        // Calculate the checksum of the received packet without the checksum value
        localChecksum = checksumAdd(bufferReceive + sizeof(udpHeader.checksum), nByte - sizeof(udpHeader.checksum));

        // Compare the received and calculated checksum
        if (localChecksum != udpHeader.checksum)
            error("Error: Local packet's checksum differs from the one that has been sent.\n");

        // Initialize bufferPayload by skip the header
        strncpy(bufferPayload, bufferReceive + nByte - payloadSize, payloadSize);

        // Initialize the structure packet to store the information 
        packet = PACKETinit(udpHeader.segNum, bufferPayload);

        // Store the packet in an ordered list
        LISTAinsert(packetList, packet);

        // Clean the buffers
        memset(bufferReceive, 0, sizeof(bufferReceive));
        memset(bufferPayload, 0, sizeof(bufferPayload));
    }

    // Print the received file on a local file
    if ((receivedFile = fopen("../data/ReceivedFile.html", "w")) == NULL)
        error("Error: Fail writing on the output file.\n");

    LISTAprint(receivedFile, packetList);

    // Close the ouput file
    fclose(receivedFile);

    // Free the memory
    LISTAfree(packetList);

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

// generate an error in packet with the given probability (0 < probability < 1)
void gremlin(char *packet, int size, double probability)
{
    int i, j, qb;
    int byteToModify[3]; // Contains the indexes of the bytes to modify
    
    double x = ((double) rand() / (RAND_MAX)); // Generate a number in [0,1)

    if (x < probability)
    {
        qb = rand() % 100 + 1; // Generate a number in [1, 100] range

        if (qb <= 50) // If the random number is in [1, 50] then we modify only one byte
            qb = 1;
        else if (qb <= 80) // If the random number is in [51, 80] then we modify only two bytes
            qb = 2;
        else // If the random number is in [81, 100] then we modify only three bytes
            qb = 3;
    }
    else
        return;

    // qb contains the number of bytes to modify
    for (i = 0; i < qb; i++)
    {
        do 
        {
            // Generate the index of the byte to modify in the range of [0, packetLenght - 1]
            byteToModify[i] = rand() % size;

            // Check that the indexes differ one from the other
            for (j = i; j >= 0; j--)
            {
                // If we generated the same value again we change it
                if (byteToModify[i] == byteToModify[j])
                    continue;
            }

            // A unique value of index has been generated
            break;

        } while(1);

        // Modify the character of index byteToModify[i] in packet
        if (packet[byteToModify[i]] != 'X')
            packet[byteToModify[i]] = 'X';
        else
            packet[byteToModify[i]] = 'Y';
    }
}

void error(char *err)
{
    perror(err);
    exit(0);
}
