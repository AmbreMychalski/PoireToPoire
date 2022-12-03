#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server2.h"
#include "client2.h"
#include "file_manager.h"


static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   SOCKET sock = init_connection();  // ouvre une interface réseau sur un port en particulier pour dire au serveur c'est ici que les clients se connecteront
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   int nbConversations = 0;
   int nbGroup=0;
   int nbTotalClient =0;
   /* an array for all clients */
   Client * clients[MAX_CLIENTS];
   Client allClient[MAX_ALLCLIENTS];
   Group *listGroup = (Group *) malloc(MAX_GROUP*sizeof(Group)); 
   Conversation *conversations = (Conversation *) malloc(MAX_CONVERSATION*sizeof(Conversation));
   fd_set rdfs;
   if(load_clients(&nbTotalClient, allClient)){
      load_historic(listGroup, &nbGroup, conversations, &nbConversations, nbTotalClient, allClient);
   }
   printf("nb TotalClient : %d\n", nbTotalClient);
   for(int i=0; i<nbTotalClient; i++){
      printf("client n°%d, name : %s, dateLastCo : %ld, connected : %d\n", i, allClient[i].name,  allClient[i].dateLastCo,  allClient[i].connected);
   }
   //test variable
   int count =0;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i]->sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard on the server*/
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client sur le port ouvert de l'interface*/
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client *c;
         int clientFound=0;
         for(int i =0; i<nbTotalClient; i++){
            if(strcmp(allClient[i].name,buffer)==0){
               clientFound=1;
               c=&allClient[i];
               c->sock=csock;
               printf("Client connected : %s\n", allClient[i].name);
               send_message_from_historic(c, listGroup, nbGroup, conversations);
            }
         }

         if(clientFound==0){   
            allClient[nbTotalClient].sock=csock ;
            strncpy(allClient[nbTotalClient].name, buffer, BUF_SIZE - 1);
            c=&allClient[nbTotalClient];
            nbTotalClient++;
         }

         
         //strncpy(c->name, buffer, BUF_SIZE - 1);
         c->connected=1;
         clients[actual] = c;
         actual++;
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i]->sock, &rdfs))
            {
               Client *client = clients[i];
               int c = read_client(clients[i]->sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  time (&clients[i]->dateLastCo);
                  clients[i]->connected=0;
                  closesocket(clients[i]->sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client->name, BUF_SIZE - 1);                 
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, *client, actual, buffer, 1);
               }
               else
               {
                  
                  //send_message_to_all_clients(clients, client, actual, buffer, 0);
                  
                  /* int command = 0;
                  char nomGr[BUF_SIZE];
                  char nomC[BUF_SIZE];
                  char message[BUF_SIZE];
                  strcpy(message,"");
                  strcpy(nomC,"");
                  strcpy(nomGr,"");
                  printf("%s : %s \n",client.name, buffer);
                  command = analyse(buffer, &nomGr, &nomC, &message);
                  printf("Command: %d \n NameGroup: %s \n NameClient:%s \n Message: %s \n", command, nomGr, nomC, message); */

                  char nom[]="Laura";
                  char nomGr[]="Grp";
                  
                  //Pour executer ce code que 1 fois (c'est du test)
                  /* if(count==0)  {                
                     
                     Group gr = {.members=(Client **)malloc(sizeof(Client*)*20) ,.nbMembers=0, 
                                 .historic = (Message *)malloc(sizeof(Message)*20), .nbMessage = 0 };
                     strcpy(gr.name, nomGr);
                     gr.members[0]=&(allClient[0]);
                     gr.members[1]=&(allClient[1]);
                     gr.nbMembers=2;
                     listGroup[0]=gr;
                     nbGroup++;
                     printf("membre group1 : %s\n",allClient[0].name);
                     Client * cli = &allClient[0];
                     printf("membre group2 : %s\n",cli->name);
                     printf("membre group3 : %s\n",(&(allClient[0]))->name);
                     printf("membre group4 : %s\n",(gr.members[0]->name));
                     printf("oui1\n");
                     count++;
                  } */
                  
                  send_message_to_conversation(conversations,client->name,nom,buffer,allClient,&nbConversations,nbTotalClient);
                  
                  printf("nb conv : %d\n", nbConversations);
                  //send_message_to_group(client->name,nomGr,listGroup,nbGroup, buffer, allClient, nbTotalClient);
                  
                  //send_message_to_group(group, client, text);
               }
               break;
            }
         }
      }
   }
   printf("nb conv : %d\n", nbConversations);
   //printf("nom clientA : %s\n", conversations[0].clientA->name);
   save_clients(nbTotalClient, allClient);
   save_historic(listGroup, nbGroup, conversations, nbConversations);
   clear_clients(clients, actual);
   end_connection(sock);
}

static void clear_clients(Client **clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i]->sock);
   }
}

static void remove_client(Client **clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client*));
   /* number client - 1 */
   (*actual)--;
}


int analyse(const char *buffer, char *nameGroup, char *nameClient, char *text){
   /*#Send #NameClient blablabla
    #Send #Group #NameGroup blablabla
    #Create 
    #Add
    #Remove
   */
   char nGroup[BUF_SIZE];
   char nClient[BUF_SIZE];
   char message[BUF_SIZE];
   strcpy(message,"");
   strcpy(nClient,"");
   strcpy(nGroup,"");
   int indexCommand=0;
   int indexName=0;
   int indexText=0;
   
   if(strncmp(buffer,"#Send", 5)==0){
      printf("send \n");
      indexCommand=5;
      if(buffer[indexCommand]==' ' && buffer[indexCommand+1] == '#'){
         indexCommand = indexCommand + 2;
         if(buffer[indexCommand]=='G' && buffer[indexCommand+1]=='r' &&  buffer[indexCommand+2]=='o' &&  buffer[indexCommand+3]=='u' 
            &&  buffer[indexCommand+4]=='p' && buffer[indexCommand+5]==' ' && buffer[indexCommand+6] == '#'){
               indexCommand = indexCommand + 7;
               while (buffer[indexCommand] != ' '){
                  nGroup[indexName]=buffer[indexCommand];
                  indexName++;
                  indexCommand++;
               }
               strcpy(nameGroup,nGroup);
               indexCommand++;
               while(buffer[indexCommand] != '\0'){
                  message[indexText] = buffer[indexCommand];
                  indexText++;
                  indexCommand++;
               }
               strcpy(text, message);
               return 1;
         } else{
            //printf("%s\n", buffer);
           // printf("%s\n", nClient);
            while (buffer[indexCommand] != ' '){
                  //printf("%s\n", buffer[indexCommand]);
                  nClient[indexName]=buffer[indexCommand];
                  //printf("%d\n", buffer[indexCommand]);
                  indexName++;
                  indexCommand++;
                  //printf("%s %d %d\n", nClient, indexCommand, indexName);
                  //printf("%s\n", buffer);
               
               }
               //printf("%s\n", nClient);
               strcpy(nameClient,nClient);
               indexCommand++;
               while(buffer[indexCommand] != '\0'){
                  message[indexText] = buffer[indexCommand];
                  indexText++;
                  indexCommand++;
               }
               strcpy(text, message);
               return 2;
         }
      }
   } else if (strncmp(buffer,"#Create",6)==0){
      printf("created \n");
      
       return 3;
      
   } else if (strncmp(buffer,"#Add",4)==0){
      printf("add \n");
      return 4;
   } else if (strncmp(buffer,"#Remove",7)==0){
      printf("remove \n");
      return 5;
   } else{
      printf("Ce n'est pas un commande possible \n");
   }

   return 0;
}

static void send_message_to_all_clients(Client **clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      strcpy(message,"");
      /* we don't send message to the sender */
      if(sender.sock != (*clients)[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client((*clients)[i].sock, message);
      }
   }
}

static void send_message_from_historic(Client *c, Group *listGroup, int nbGroup, Conversation *listConversation ){
   char message[BUF_SIZE];
   message[0] = 0;
   for(int i =0; i<nbGroup; i++){
       for(int j = 0; j < listGroup[i].nbMembers; j++)
         {
            if(strcmp(listGroup[i].members[j]->name, c->name)==0)
            {
               for(int w=0; w<listGroup[i].nbMessage; w++){
                  
                  strcpy(message,"(Group) ");
                  strncat(message, listGroup[i].historic[w].sender->name, sizeof message - strlen(message) - 1);
                  strncat(message, " : ", sizeof message - strlen(message) - 1);
                  strncat(message, listGroup[i].historic[w].text, sizeof message - strlen(message) - 1);
                  strncat(message, " \n ", sizeof message - strlen(message) - 1);
                  write_client(c->sock, message);
                  
               }               
            }
         }
   }
}

static void send_message_to_group(const char *nomClient, char *nomGroup, Group *listGroup, int nbGroup, const char *buffer,
                                 Client*clients, int nbClient){
   char message[BUF_SIZE];
   message[0] = 0;
   int groupFound =0;
   for( int i =0; i<nbGroup; i++){
      const char * nomGroupI = listGroup[i].name;
      groupFound = 1;
      if(strcmp(nomGroupI, nomGroup)==0){
         
         // update la conversation
         Message newMsg = { .sender = getClient(nomClient,clients,nbClient)};
         time ( &newMsg.date );
         strcpy(newMsg.text, buffer);
         listGroup[i].historic[listGroup[i].nbMessage] = newMsg;
         listGroup[i].nbMessage=listGroup[i].nbMessage+1;
         
         
         printf("in message to group1, nbmemnbers : %d\n", listGroup[i].nbMembers);
         for(int j = 0; j < listGroup[i].nbMembers; j++)
         {
            printf("group1\n");
            printf(" name member : %s\n", nomClient);
            printf("j : %d\n", j);
            printf("connected %d\n",listGroup[i].members[j]->connected);
            printf("name member : %s\n", listGroup[i].members[j]->name);//seg fault
            /* we don't send message to the sender */
            if(strcmp(listGroup[i].members[j]->name, nomClient)!=0 && listGroup[i].members[j]->connected == 1 )
            {  printf("group2\n");
               strcpy(message,"(Group) ");
               printf("group3\n");
               strncat(message, nomClient, sizeof message - strlen(message) - 1);
               printf("group4\n");
               strncat(message, " : ", sizeof message - strlen(message) - 1);
               printf("group5\n");
               strncat(message, buffer, sizeof message - strlen(message) - 1);
               printf("group6\n");
               write_client(listGroup[i].members[j]->sock, message);
               printf("group7\n");
            }
         }
      }     
   }

   //si le groupe n'est pas trouvé
   if(groupFound == 0){
      strcpy(message,"Error : the group doesn't exist");
   }
}

static void send_message_to_conversation(Conversation* listConversation, const char* senderName, const char* receiverName, 
                                          const char *buffer, Client*clients, int * nbConversations, int nbClient){
   // vérifier si la conversation existe déjà
   int exist = 0;
   int i=0;
   Conversation *convActuelle;

   Client *conversationClientA;
   Client *conversationClientB;
   while(i<(*nbConversations) && exist==0){
      conversationClientA=(listConversation[i].clientA);
      conversationClientB=(listConversation[i].clientB);
      if((strcmp(senderName, conversationClientA->name)==0 && strcmp(receiverName, conversationClientB->name)==0) 
      || (strcmp(senderName, conversationClientB->name)==0 && strcmp(receiverName, conversationClientA->name)==0)){
         exist=1;
         convActuelle = &(listConversation[i]);
         printf("conversation already exist\n");
      }
   }
   // si elle n'existe pas : la créer
   if(exist==0 && (*nbConversations)<=100){
      printf("conversation needs to be create\n");
      Conversation conv = { .clientA = getClient(senderName,clients,nbClient), .clientB = getClient(receiverName,clients,nbClient), 
                           .historic = (Message *)malloc(sizeof(Message)*20), .nbMessage = 0 };
      listConversation[*nbConversations]= conv;
      convActuelle = &(listConversation[*nbConversations]);
      (*nbConversations)++;
   }
   // update la conversation
   Message newMsg = { .sender = getClient(senderName,clients,nbClient)};
   time ( &newMsg.date );
   strcpy(newMsg.text, buffer);
   convActuelle->historic[convActuelle->nbMessage] = newMsg;
   convActuelle->nbMessage++;

   // envoyer le message au destinataire
   char message[BUF_SIZE];
   message[0] = 0;
   strcpy(message,"");
   strncpy(message, senderName, BUF_SIZE - 1);
   strncat(message, " : ", sizeof message - strlen(message) - 1);
   strncat(message, buffer, sizeof message - strlen(message) - 1);
   write_client( getClient(receiverName,clients,nbClient)->sock, message);
}



static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

Client * getClient(const char *name, Client *listClient, int nbClient){
   Client * res=NULL;
   for (int i=0; i<nbClient; i++){
      if(strcmp(listClient[i].name,name)==0){
         res=&listClient[i];
      }
   }
   return res;
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
