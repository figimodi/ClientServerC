#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/Packet.h"


struct packet{
    int num;
    char buffer[BUFFSIZE];
};

PACKET PACKETinit(int num, char *buffer)
{
    PACKET p = malloc(sizeof(*p));
    p->num = num;
    strcpy(p->buffer, buffer);
    return p;
}

PACKET PACKsetNULL()
{
    PACKET p = malloc(sizeof(*p));
    p->num = -1;
    return p;
}

int PACKcheckNULL(PACKET p)
{
    if(p->num == -1)
        return 1;
    return 0;
}

int PACKcmp(PACKET p1, PACKET p2)
{
	if(p1->num == p2->num)
		return 0;
	if(p1->num < p2->num)
		return -1;
	return 1;
}

void PACKETprint(FILE *fp, PACKET p)
{
    fprintf(fp, "%s", p->buffer);
}
