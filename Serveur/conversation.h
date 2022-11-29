#ifndef CONVERSATION_H
#define CONVERSATION_H

#include "server2.h"

typedef struct
{  
   Client *clientA;
   Client *clientB;
   Message *historic;
   int nbMessage;
}Conversation;

#endif /* guard */
