#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

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

static void app(const char *address, const char *name)
{
   SOCKET sock = init_connection(address);
   char buffer[BUF_SIZE];

   fd_set rdfs;

   /* send our name */
   write_server(sock, name);
   int fileMode =0;
   char nameFile[BUF_SIZE];
   FILE *received_file;

   while(1)
   {
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the socket */
      FD_SET(sock, &rdfs);

      if(select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         fgets(buffer, BUF_SIZE - 1, stdin);
         {
            char *p = NULL;
            p = strstr(buffer, "\n");
            if(p != NULL)
            {
               *p = 0;
            }
            else
            {
               /* fclean */
               buffer[BUF_SIZE - 1] = 0;
            }
         }

         if(strncmp(buffer,"#Send #file",11)==0){
            printf("Send file request\n");
            send_file(sock, buffer);

         }
         else{
            write_server(sock, buffer);
         }
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         int n = read_server(sock, buffer);
         /* server down */
         if(n == 0)
         {
            printf("Server disconnected !\n");
            break;
         }
         /* char command[BUF_SIZE];
         strcpy(command,buffer); 
         char ** split_command = str_split(command,' '); */
         if(strncmp(buffer,"#FileBegin",10)==0){    
            if(fileMode==1){
               fileMode=0;
               fclose(received_file);
            }   
              
            char ** split_command = str_split(buffer,' ');
            
            strcpy(nameFile, split_command[1]);
            char filePath[BUF_SIZE];
            strcpy(filePath,"./file_received/");
            strncat(filePath, nameFile, sizeof filePath - strlen(filePath) - 1);
            received_file = fopen(filePath, "w");
            fileMode=1;
            printf("File received : %s\n",nameFile);           
         }
         else if(strncmp(buffer,"#File",5)==0){            
            char strToRemove[BUF_SIZE];
            strcpy(strToRemove,"#File #");
            strncat(strToRemove, name, sizeof strToRemove - strlen(strToRemove) - 1);
            strremove(buffer,strToRemove);
            fprintf(received_file, "%s", buffer);
         }
         else{
            if(fileMode==1){
               fileMode=0;
               fclose(received_file);
            }
            puts(buffer);
         }         
      }
   } 

   end_connection(sock);
}

static void send_file(SOCKET sock, char* buffer){
   
   int sent_bytes = 0;
   
   
   char message[BUF_SIZE];
   char fileName[BUF_SIZE];
   char receiverName [BUF_SIZE];
   message[0] = 0;
   
   char command[BUF_SIZE];
   strcpy(command,buffer);

   size_t count; 

   char ** split_command = str_split_count(buffer,' ',&count);
   
   if(count>3){
      strcpy(fileName,split_command[3]);
      strcpy(receiverName,split_command[2]);

      if(access(fileName, F_OK) == 0){
         FILE *fp = fopen(fileName, "r");
         write_server(sock, command);

         char data[BUF_SIZE]= {0};
         message[0] = 0;
         while(fgets(data, BUF_SIZE-strlen("#File # ")-strlen(receiverName), fp) != NULL) {
            strcpy(message,"#File ");
            strncat(message, receiverName, sizeof message - strlen(message) - 1);
            strncat(message, " ", sizeof message - strlen(message) - 1);
            strncat(message, data, sizeof message - strlen(message) - 1);
            write_server(sock, message);
            bzero(data, BUF_SIZE);
         }
      }
      else{
         printf("There was a problem when opening the file\n");
      }
   }
   else{
      printf("Wrong request\n");
   }
   


}

static int init_connection(const char *address)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };
   struct hostent *hostinfo;

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   hostinfo = gethostbyname(address);
   if (hostinfo == NULL)
   {
      fprintf (stderr, "Unknown host %s.\n", address);
      exit(EXIT_FAILURE);
   }

   sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
   {
      perror("connect()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_server(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      exit(errno);
   }

   buffer[n] = 0;

   return n;
}


static void write_server(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

char** str_split(char* a_str, const char a_delim)
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

    return result;
}

static char** str_split_count(char* a_str, const char a_delim, size_t* num){
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

    *num = count;
    return result;
}

char *strremove(char *str, const char *sub) {
    char *p, *q, *r;
    if (*sub && (q = r = strstr(str, sub)) != NULL) {
        size_t len = strlen(sub);
        while ((r = strstr(p = r + len, sub)) != NULL) {
            memmove(q, p, r - p);
            q += r - p;
        }
        memmove(q, p, strlen(p) + 1);
    }
    return str;
}

int main(int argc, char **argv)
{
   if(argc < 2)
   {
      printf("Usage : %s [address] [pseudo]\n", argv[0]);
      return EXIT_FAILURE;
   }

   init();

   app(argv[1], argv[2]);

   end();

   return EXIT_SUCCESS;
}
