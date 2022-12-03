#ifndef GROUP_H
#define GROUP_H

#include "server2.h"

typedef struct
{  
   char name[BUF_SIZE];
   int nbMembers ;
   int nbMessage;
   Client **members;
   Message *historic;
}Group;

#endif /* guard */
