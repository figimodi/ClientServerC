#ifndef PACKET_H_INCLUDED
#define PACEKT_H_INCLUDED

#include "ListaPacket.h"
#define BUFFSIZE 512

typedef struct packet *PACKET;

int PACKcmp(PACKET p1, PACKET p2);
PACKET PACKETinit();
PACKET PACKsetNULL();
int PACKcheckNULL(PACKET p);
void PACKETprint(FILE *fp,PACKET p);
void PACKETfree(PACKET p);

#endif // PACKET_H_INCLUDED
