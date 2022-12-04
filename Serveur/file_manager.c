#include "file_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void save_clients(const int nbTotalClient,  Client *allClient){
   FILE *historicClient;
   int i;
   historicClient = fopen("historicClient.dat", "w");
   if (historicClient==NULL) {
      fprintf(stderr, "\nError while opening file in save clients\n");
      exit (1);
   } 
   fwrite(&nbTotalClient, sizeof(int), 1, historicClient);
   for(i=0; i<nbTotalClient; i++){
      printf("name : %s, datelastco : %ld, connected : %d\n", allClient[i].name, allClient[i].dateLastCo, allClient[i].connected);
      fwrite(&(allClient[i].name), BUF_SIZE*sizeof(char), 1, historicClient);
      fwrite(&(allClient[i].dateLastCo), sizeof(time_t), 1, historicClient);
      fwrite(&(allClient[i].connected), sizeof(int), 1, historicClient);
      
      if(fwrite != 0)
         printf("Contents to file historicClient.dat written successfully\n");
      else
         printf("Error writing file historicClient.dat\n");
   }
   fclose (historicClient);
}

int load_clients(int *nbTotalClient, Client *allClient){
   FILE *historicClient;
   int i;
   historicClient = fopen("historicClient.dat", "r");
   if (historicClient==NULL) {
      fprintf(stderr, "\nError while opening file in save clients\n");
      return 0;
   } else {
      if(fread(nbTotalClient, sizeof(int), 1, historicClient)!=1){
         strerror(errno);
         printf("nbTotalClient not read\n");
         return 0;
      }
      
      for(i=0; i<(*nbTotalClient); i++){
         fread(&(allClient[i].name), BUF_SIZE*sizeof(char), 1, historicClient);
         fread(&(allClient[i].dateLastCo), sizeof(time_t), 1, historicClient);
         fread(&(allClient[i].connected), sizeof(int), 1, historicClient);
         
         if(fread != 0){
            printf("Contents to file historicClient.dat read successfully\n");
         } else {
            printf("Error reading file historicClient.dat\n");
            return 0;
         }
      }
      /* for(i=0; i<(*nbTotalClient); i++){
         printf("Client n°%d : \n- name : %s\n- dateLastCo : %ld\n- connected : %d\n\n", i, allClient[i].name, allClient[i].dateLastCo, allClient[i].connected);
      } */
      fclose (historicClient);
   }
   return 1;
}

void save_historic(Group *listGroup, const int nbGroup, Conversation *listConversation, const int nbConversations){
   FILE *historicGroup;
   FILE *historicConversation;
   
   historicGroup = fopen ("historicGroup.dat", "w");
   if (historicGroup == NULL) {
      fprintf(stderr, "\nError while opening file in save historic (Group)\n");
      exit (1);
   }
   historicConversation = fopen ("historicConversation.dat", "w");
   if (historicConversation == NULL) {
      fprintf(stderr, "\nError while opening file in save historic (Conversation)\n");
      exit (1);
   }
   fwrite(&nbGroup, sizeof(int), 1, historicGroup);
   //on parcourt les groupes un à un
   for(int i=0; i<nbGroup; i++){
      fwrite(&(listGroup[i].name), BUF_SIZE*sizeof(char), 1, historicGroup);
      fwrite(&(listGroup[i].nbMembers), sizeof(int), 1, historicGroup);
      fwrite(&(listGroup[i].nbMessage), sizeof(int), 1, historicGroup);
      int j;
      for(j=0;j<listGroup[i].nbMembers; j++){
         fwrite(&((listGroup[i].members[j])->name), BUF_SIZE*sizeof(char), 1, historicGroup);
      }
      for(j=0; j<listGroup[i].nbMessage; j++){
         fwrite(&((listGroup[i].historic[j]).text), BUF_SIZE*sizeof(char), 1, historicGroup);
         fwrite(&((listGroup[i].historic[j].sender)->name), BUF_SIZE*sizeof(char), 1, historicGroup);
         fwrite(&((listGroup[i].historic[j]).date), sizeof(int), 1, historicGroup);
      }
      
      if(fwrite != 0)
         printf("Contents to file historicGroup.dat written successfully\n");
      else
         printf("Error writing file historicGroup.dat\n");
   }
   fclose (historicGroup);

   fwrite(&nbConversations, sizeof(int), 1, historicConversation);
   for(int i=0; i<nbConversations; i++){
      fwrite (&((listConversation[i].clientA)->name), BUF_SIZE*sizeof(char), 1, historicConversation);
      fwrite (&((listConversation[i].clientB)->name), BUF_SIZE*sizeof(char), 1, historicConversation);
      int j;
      fwrite(&(listConversation[i].nbMessage), sizeof(int), 1, historicConversation);
      for(j=0; j<listConversation[i].nbMessage; j++){
         fwrite(&((listConversation[i].historic[j]).text), BUF_SIZE*sizeof(char), 1, historicConversation);
         fwrite(&((listConversation[i].historic[j].sender)->name), BUF_SIZE*sizeof(char), 1, historicConversation);
         fwrite(&((listConversation[i].historic[j]).date), sizeof(int), 1, historicConversation);
      }
      
      if(fwrite != 0)
         printf("Contents to file historicConversation.dat written successfully\n");
      else
         printf("Error writing file historicConversation.dat\n");
   }
   fclose (historicConversation); 
}

void load_historic(Group *listGroup, int *nbGroup, Conversation *listConversation, int *nbConversations, const int nbTotalClient, Client *allClient){
   // Driver program
   FILE *historicGroup;
   FILE *historicConversation;
   
   // Open person.dat for reading
   historicGroup = fopen ("historicGroup.dat", "r");
   historicConversation = fopen ("historicConversation.dat", "r");
   if (historicGroup == NULL){
      fprintf(stderr, "\nError while opening file in load historic (Group)\n");
   } else if (historicConversation == NULL){
      fprintf(stderr, "\nError while opening file in load conversation (Conversation)\n");
   } else {
   
      if(fread(nbGroup, sizeof(int), 1, historicGroup)!=1){
         strerror(errno);
         printf("nbGroup not read\n");
      }
      if(fread(nbConversations, sizeof(int), 1, historicConversation)!=1){
         strerror(errno);
         printf("nbConversations not read\n");
      }

      int i,j; 
      for(i=0; i<(*nbGroup); i++){
         fread(&((listGroup)[i].name), BUF_SIZE*sizeof(char), 1, historicGroup);
         fread(&((listGroup)[i].nbMembers), sizeof(int), 1, historicGroup);
         fread(&((listGroup)[i].nbMessage), sizeof(int), 1, historicGroup);
         (listGroup)[i].members = (Client**)malloc(((listGroup)[i].nbMembers+(listGroup)[i].nbMembers%INCR_MEM_GROUP)*sizeof(Client*));
         for(j=0; j<(listGroup)[i].nbMembers; j++){
            /* (*listGroup)[i].members[j] = (Client*)malloc(sizeof(Client)); 
            fread(&((*listGroup)[i].members[j]->name), BUF_SIZE*sizeof(char), 1, historicGroup); */
            //(*listGroup)[i].members[j] = getClient(nameClient, liste de tous les clients, nbClietns total);
            char nameActualClient[BUF_SIZE];
            fread(&(nameActualClient), BUF_SIZE*sizeof(char), 1, historicGroup);
            (listGroup)[i].members[j] = getClient(nameActualClient, allClient, nbTotalClient);
         }
         (listGroup)[i].historic = (Message*)malloc(((listGroup)[i].nbMessage+listGroup[i].nbMessage%INCR_MEM_MESSAGE)*sizeof(Message));
         for(j=0; j<(listGroup)[i].nbMessage; j++){
            fread(&((listGroup)[i].historic[j].text), BUF_SIZE*sizeof(char), 1, historicGroup);
            /* (*listGroup)[i].historic[j].sender = (Client*)malloc(sizeof(Client)); */
            char nameActualClient[BUF_SIZE];
            fread(&(nameActualClient), BUF_SIZE*sizeof(char), 1, historicGroup);
            (listGroup)[i].historic[j].sender = getClient(nameActualClient, allClient, nbTotalClient);

            fread(&((listGroup)[i].historic[j].date), sizeof(int), 1, historicGroup);
         }
         //test print
         printf("name : %s, nbMembre : %d, nbMessage : %d\n", (listGroup)[i].name, (listGroup)[i].nbMembers, (listGroup)[i].nbMessage);
         for(j=0; j<(listGroup)[i].nbMembers; j++){
            printf("membre : %s, \n", ((listGroup)[i].members[j])->name);
         }
         printf("\n");
         for(j=0; j<(listGroup)[i].nbMessage; j++){
            printf("message : %s, sender %s, date %ld \n", (listGroup)[i].historic[j].text, ((listGroup)[i].historic[j].sender)->name, (listGroup)[i].historic[j].date);
         }
      }

      for(i=0; i<(*nbConversations); i++){
         /* (*listConversation)[i].clientA = (Client*)malloc(sizeof(Client));
         (*listConversation)[i].clientB = (Client*)malloc(sizeof(Client));
         fread(&(((*listConversation)[i].clientA)->name), BUF_SIZE*sizeof(char), 1, historicConversation);
         fread(&(((*listConversation)[i].clientB)->name), BUF_SIZE*sizeof(char), 1, historicConversation); */
         
         char nameActualClientA[BUF_SIZE];
         char nameActualClientB[BUF_SIZE];
         fread(&(nameActualClientA), BUF_SIZE*sizeof(char), 1, historicConversation);
         fread(&(nameActualClientB), BUF_SIZE*sizeof(char), 1, historicConversation);
         (listConversation)[i].clientA = getClient(nameActualClientA, allClient, nbTotalClient);
         (listConversation)[i].clientB = getClient(nameActualClientB, allClient, nbTotalClient);

         fread(&((listConversation)[i].nbMessage), sizeof(int), 1, historicConversation);

         (listConversation)[i].historic = (Message*)malloc(((listConversation)[i].nbMessage+listConversation[i].nbMessage%INCR_MEM_MESSAGE)*sizeof(Message));
         for(j=0; j<(listConversation)[i].nbMessage; j++){
            fread(&((listConversation)[i].historic[j].text), BUF_SIZE*sizeof(char), 1, historicConversation);
            /* (*listConversation)[i].historic[j].sender = (Client*)malloc(sizeof(Client)); 
            fread(&(((*listConversation)[i].historic[j].sender)->name), BUF_SIZE*sizeof(char), 1, historicConversation); */
            char nameActualClient[BUF_SIZE];
            fread(&(nameActualClient), BUF_SIZE*sizeof(char), 1, historicConversation);
            (listConversation)[i].historic[j].sender = getClient(nameActualClient, allClient, nbTotalClient);

            fread(&((listConversation)[i].historic[j].date), sizeof(int), 1, historicConversation);
         }
         //test print
         printf("nameA : %s, nameB : %s, nbMessage : %d\n", ((listConversation)[i].clientA)->name, ((listConversation)[i].clientB)->name, (listConversation)[i].nbMessage);
         for(j=0; j<(listConversation)[i].nbMessage; j++){
            printf("message : %s, sender %s, date %ld\n", (listConversation)[i].historic[j].text, ((listConversation)[i].historic[j].sender)->name, (listConversation)[i].historic[j].date);
         }
      }
      fclose (historicGroup);
      fclose (historicConversation);
   }
}