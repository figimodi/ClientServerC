#ifndef LISTAPACKET_H_INCLUDED
#define LISTAPACKET_H_INCLUDED

#include "Packet.h"

typedef struct lista *LISTA;

LISTA LISTAinit();
void LISTAinsert(LISTA l,PACKET p);
void LISTAprint(FILE *fp,LISTA l);
void LISTAfree(LISTA l);

#endif // LISTAPACKET_H_INCLUDED
