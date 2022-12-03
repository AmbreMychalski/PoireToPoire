#ifndef CONVERSATION_H
#define CONVERSATION_H

#include "server2.h"

typedef struct
{  
   Client *clientA;
   Client *clientB;
   int nbMessage;
   Message *historic;
}Conversation;

#endif /* guard */
