#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"
#include <time.h>

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   time_t dateLastCo;
   int connected;
}Client;

#endif /* guard */
