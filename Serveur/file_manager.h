#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "server2.h"

void save_historic(Group *listGroup, int nbGroup, Conversation *listConversation, int nbConversations);
void load_historic(Group *listGroup, int *nbGroup, Conversation *listConversation, int *nbConversations, const int nbTotalClient, Client *allClient);
void save_clients(const int nbTotalClient,  Client *allClient);
int load_clients(int *nbTotalClient, Client *allClient);
#endif