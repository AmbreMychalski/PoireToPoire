#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string.h>
#include <assert.h>

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
   int nbTotalClient =0;
   /* an array for all clients */
   Client * clients[MAX_CLIENTS];
   Client   allClient[MAX_ALLCLIENTS];
   Group *listGroup = (Group *) malloc(50*sizeof(Group)); 

   Conversation *conversations = (Conversation *) malloc(100*sizeof(Conversation));
   fd_set rdfs;

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
                  
                  int command = 0;
                  char nomGr[BUF_SIZE];
                  char nomC[BUF_SIZE];
                  char message[BUF_SIZE];
                  char *listClient[100];
                  int nbClient=0;
                  strcpy(message,"");
                  strcpy(nomC,"");
                  strcpy(nomGr,"");
                  command = analyse(buffer, nomGr, nomC, message,listClient, &nbClient);
                  switch (command){
                  case 1: //Send Group
                     
                     break;
                  case 2: //Send Client
                     
                     break;
                  case 3: //Create
                     
                     break;
                  case 4: //Add
                     
                     break;
                  case 5: //Remove
                  
                     break;
                  default:
                     break;
                  }
                  printf("Command: %d \n NameGroup:%s \n NameClient:%s \n Message: %s\n nbclient: %d \n", command, nomGr, nomC, message,nbClient);
                  for(int i =0; i<nbClient;i++){
                     printf(" %s ",listClient[i]);
                  }
                  printf("\n");
                  //char nom[]="Laura";
                  //char nomGr[]="Grp";
                  
                  //Pour executer ce code que 1 fois (c'est du test)
                  if(count==0)  {                

                     Group gr = {.members=(Client **)malloc(sizeof(Client*)*20) ,.nbMembers=0, .historic = (Message *)malloc(sizeof(Message)*20), .nbMessage = 0 };
                     strcpy(gr.name, nomGr);
                     gr.members[0]=&allClient[0];
                     gr.members[1]=&allClient[1];
                     gr.nbMembers=2;
                     listGroup[0]=gr;
                     nbGroup++;

                     count++;
                  }
                  
                  //send_message_to_conversation(conversations,client->name,nom,buffer,allClient,&nbConversations,nbTotalClient);
                  send_message_to_group(client->name,nomGr,listGroup,nbGroup, buffer, allClient, nbTotalClient);
                  
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

void sendedText(const char *buffer, int indexCommand, char *text){
   char message[BUF_SIZE];
   int indexText=0;
   message[0] = 0;
   while(buffer[indexCommand] != '\0'){
      message[indexText] = buffer[indexCommand];
      indexText++;
      indexCommand++;
   }
   strcpy(text, message);
}

void NameGroup(const char *buffer, int indexCommand, char *nameGroup){
   char nGroup[BUF_SIZE];
   int indexName=0;
   nGroup[0] = 0;
   while (buffer[indexCommand] != ' '){
      nGroup[indexName]=buffer[indexCommand];
      indexName++;
      indexCommand++;
   }
   strcpy(nameGroup,nGroup);
}

static int analyse(const char *buffer, char *nameGroup, char *nameClient, char *text, char**listClient, int* nbClients){
   /*#Send #NameClient blablabla
    #Send #Group #NameGroup blablabla
    #Create #NameGroup nameClient1, nameClient2 
    #Add #NameGroup nameClient
    #Remove
   */
   char nGroup[BUF_SIZE];
   char nClient[BUF_SIZE];
   nGroup[0] = 0;
   nClient[0] = 0;
   strcpy(nClient,"");
   strcpy(nGroup,"");
   int indexCommand=0;
   int indexName=0;
   
   size_t countWord =0;
   char command[BUF_SIZE];
   strcpy(command,buffer);
   char ** splitCommand = str_split(buffer,' ', &countWord);
   countWord--;

   printf("la : %s\n", splitCommand[indexCommand]);
   
   if( strcmp(splitCommand[indexCommand],"#Send")==0){
      indexCommand++;
      if(countWord>3 && strcmp(splitCommand[indexCommand],"#Group")==0){
         indexCommand++;
         strcpy(nameGroup, splitCommand[indexCommand]);
         memmove(nameGroup, nameGroup+1, strlen(nameGroup));
         size_t startOfText = strlen("#Send #Group ")+strlen(splitCommand[indexCommand]);
         strcpy(text, substr(command,startOfText,strlen(command)-startOfText));
         return 1;
      } else if (countWord>2){
         
         strcpy(nameClient,splitCommand[indexCommand]);
         
         memmove(nameClient, nameClient+1, strlen(nameClient));
         size_t startOfText = strlen("#Send ")+strlen(splitCommand[indexCommand]);
         strcpy(text, substr(command,startOfText,strlen(command)-startOfText));
         return 2;
      }
      //}
   } else if (strncmp(buffer,"#Create",6)==0){
      printf("created \n");

      indexCommand++;      
      strcpy(nameGroup, splitCommand[indexCommand]);
      indexCommand++;
      memmove(nameGroup, nameGroup+1, strlen(nameGroup));
      *nbClients=((int) countWord)-indexCommand;
      int j=0;
      for(; indexCommand<countWord; indexCommand++){
         listClient[j]=(char*)malloc(BUF_SIZE);
         strcpy(listClient[j],splitCommand[indexCommand]);
         j++;
      }      
      return 3;
      
   } else if (strncmp(buffer,"#Add",4)==0){
      printf("add \n");
      return 4;
   } else if (strncmp(buffer,"#Remove",7)==0){
      printf("remove \n");
      return 5;
   } else{
      printf("Ce n'est pas une commande possible \n");
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
         

         for(int j = 0; j < listGroup[i].nbMembers; j++)
         {
            /* we don't send message to the sender */
            if(strcmp(listGroup[i].members[j]->name, nomClient)!=0 && listGroup[i].members[j]->connected == 1 )
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

static Client * getClient(const char *name, Client *listClient, int nbClient){
   Client * res=NULL;
   for (int i=0; i<nbClient; i++){
      if(strcmp(listClient[i].name,name)==0){
         res=&listClient[i];
      }
   }
   return res;
}

char** str_split(char* a_str, const char a_delim, size_t *size)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

   *size = count;
    return result;
}

char *substr(char const *input, size_t start, size_t len) { 
    char *ret = malloc(len+1);
    memcpy(ret, input+start, len);
    ret[len]  = '\0';
    return ret;
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
