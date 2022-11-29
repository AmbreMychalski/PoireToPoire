#ifndef GROUP_H
#define GROUP_H

#include "server2.h"

typedef struct
{  
   char name[BUF_SIZE];
   Client *members;
   int nbMembers ;
   Message *historic;
   int nbMessage;
}Group;

#endif /* guard */
