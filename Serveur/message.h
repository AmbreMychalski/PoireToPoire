#ifndef MESSAGE_H
#define MESSAGE_H

#include "server2.h"

typedef struct
{  
   char text[BUF_SIZE];
   Client *sender;
   unsigned long int date;
}Message;

#endif /* guard */
