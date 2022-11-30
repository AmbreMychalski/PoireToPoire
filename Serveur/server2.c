#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server2.h"
#include "client2.h"



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
   /* an array for all clients */
   Client clients[MAX_CLIENTS];
   Client AllClient[MAX_ALLCLIENTS];
   Group *listGroup = (Group *) malloc(50*sizeof(Group)); 

   Conversation *conversations = (Conversation *) malloc(100*sizeof(Conversation));
   fd_set rdfs;

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
         FD_SET(clients[i].sock, &rdfs);
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

         Client c = { csock };
         strncpy(c.name, buffer, BUF_SIZE - 1);
         clients[actual] = c;
         actual++;
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client.name, BUF_SIZE - 1);                 
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, client, actual, buffer, 1);
               }
               else
               {
                  printf("oui : %s : %s \n",getClient(client.name,clients, actual)->name, buffer);

                  //send_message_to_all_clients(clients, client, actual, buffer, 0);
                  
                  int command = 0;
                  char nomGr[BUF_SIZE];
                  char nomC[BUF_SIZE];
                  char message[BUF_SIZE];
                  strcpy(message,"");
                  strcpy(nomC,"");
                  strcpy(nomGr,"");
                  printf("%s : %s \n",client.name, buffer);
                  command = analyse(buffer, &nomGr, &nomC, &message);
                  printf("Command: %d \n NameGroup: %s \n NameClient:%s \n Message: %s \n", command, nomGr, nomC, message);

                  //char nom[]="Laura";
                  //char nomGr[]="Grp";

                  Group gr = {.members=(Client **)malloc(sizeof(Client*)*20) ,.nbMembers=0, .historic = (Message *)malloc(sizeof(Message)*20), .nbMessage = 0 };
                  strcpy(gr.name, nomGr);
                  gr.members[0]=&clients[0];
                  gr.members[1]=&clients[1];
                  gr.nbMembers=2;
                  listGroup[0]=gr;
                  nbGroup++;
                  //send_message_to_conversation(conversations,client.name,nom,buffer,clients,&nbConversations,actual);
                  send_message_to_group(client.name,nomGr,listGroup,nbGroup, buffer);
                  
                  //send_message_to_group(group, client, text);
               }
               break;
            }
         }
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
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

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      strcpy(message,"");
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

static void send_message_from_historic(Client *c, Group *listGroup, int nbGroup, Conversation *listConversation ){
   char message[BUF_SIZE];
   message[0] = 0;
   for(int i =0; i<nbGroup; i++){
       for(int j = 0; j < listGroup[i].nbMembers; j++)
         {
            if(strcmp(listGroup[i].members[i]->name, c->name)==0)
            {
               for(int w=0; w<listGroup[i].nbMessage; i++){
                  if(listGroup[i].historic[w].date>c->dateLastCo){
                     strncpy(message,listGroup[i].historic[w].text , BUF_SIZE - 1);
                     write_client(c->sock, message);
                  }
               }               
            }
         }
   }
}

static void send_message_to_group(const char *nomClient, char *nomGroup, Group *listGroup, int nbGroup, const char *buffer){
   char message[BUF_SIZE];
   message[0] = 0;
   int groupFound =0;
   for( int i =0; i<nbGroup; i++){
      const char * nomGroupI = listGroup[i].name;
      groupFound = 1;
      if(strcmp(nomGroupI, nomGroup)==0){
         for(int j = 0; j < listGroup[i].nbMembers; j++)
         {
            printf("ici : %d\n", i);
            /* we don't send message to the sender */
            if(strcmp(listGroup[i].members[j]->name, nomClient)!=0)
            {   
               strcpy(message,"(Group) ");
               strncat(message, nomClient, sizeof message - strlen(message) - 1);
               strncat(message, " : ", sizeof message - strlen(message) - 1);
               strncat(message, buffer, sizeof message - strlen(message) - 1);
               write_client(listGroup[i].members[j]->sock, message);
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
   Message newMsg = { .sender = getClient(senderName,clients,nbClient), .date = 0};
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

static Client * getClient(const char *name, Client *listClient, int nbClient){
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
