#ifndef __UNP_H
#define __UNP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ListaPacket.h"
#include "Packet.h"

#define SERV_PORT 50000
#define MAX_LINE 4096
#define BUFF_SIZE 512

typedef struct
{
    char method[MAX_LINE];
    char url[MAX_LINE];
    char protocol[MAX_LINE];
} Request;

typedef struct 
{
    unsigned short checksum;
    unsigned short segNum;
} UDPHeader;

const char *path = "../data/";
const char *HTTPHeader = "HTTP/1.0 200 Document Follows\r\nContent-Type: text/plain\r\nContent-Length: %d\r\n\r\n";

#endif
