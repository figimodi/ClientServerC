#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ListaPacket.h"

typedef struct node* linkL;
struct node{
    PACKET item;
    linkL next;
};

struct lista{
    linkL head;
    int size;
};

LISTA LISTAinit()
{
    LISTA l = malloc(sizeof(*l));
    l->head = NULL;
    l->size = 0;
    return l;
}

static linkL NEW(PACKET item,linkL next)
{
    linkL x = malloc(sizeof(*x));
    x->item = item;
    x->next = next;
    return x;
}

void LISTAinsert(LISTA l,PACKET p)
{
    linkL x, f;
    if(l->head == NULL || PACKcmp(p,l->head->item) < 0)
    {
        l->head = NEW(p, l->head);
        l->size++;
        return;
    }
    for(x = l->head->next, f = l->head; x != NULL && PACKcmp(p, f->item) > 0; f = x, x = x->next);
    f->next = NEW(p, x);
    l->size++;
    return;
}

void LISTAremove(LISTA l,PACKET p)
{
    linkL x, f;
    if(l->head == NULL)
        return;
    for(x = l->head, f = NULL; x != NULL; f = x, x = x->next)
    {
        if(PACKcmp(p, x->item) == 0)
        {
            if(x == l->head)
                l->head = x->next;
            else
                f->next = x->next;

            free(x);
        }
    }
}

void LISTAprint(FILE *fp,LISTA l)
{
    linkL x;
    for(x = l->head; x != NULL; x = x->next)
    {
        PACKETprint(fp, x->item);
    }
}

void LISTAfree(LISTA l)
{
    linkL x, n;

    for(x = l->head; x != NULL; x = n)
    {
        n = x->next;
        free(x);
    }

}
