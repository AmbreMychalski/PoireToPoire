#ifndef SERVER_H
#define SERVER_H

#ifdef WIN32

#include <winsock2.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF        "\r\n"
#define PORT         1977
#define MAX_CLIENTS     100
#define MAX_ALLCLIENTS     100
#define INCR_MEM_GROUP     20
#define INCR_MEM_MESSAGE     4
#define MAX_MEM_GROUP     20
#define MAX_GROUP     50
#define MAX_CONVERSATION     100

#define BUF_SIZE    1024



#include "client2.h"
#include "message.h"
#include "group.h"
#include "conversation.h"

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static int analyse(const char *buffer, char *nameGroup, char *nameClient, char *text, char**listClient, int* nbClients, char *nameFile);
static char** str_split(char* a_str, const char a_delim, size_t* size);
static char *substr(char const *input, size_t start, size_t len);
static void send_message_to_all_clients(Client **clients, Client sender, int actual, const char *buffer, char from_server);
static void remove_client(Client **clients, int to_remove, int *actual);
static void clear_clients(Client **clients, int actual);
Client * getClient(const char *name, Client *listClient, int nbClient);
static void send_message_to_conversation(Conversation* listConversation, const char* senderName, const char* receiverName, 
                                          const char *buffer, Client*clients, int * nbConversations, int nbClient);
static void send_message_to_group(const char *nomClient, char *nomGroup, Group *listGroup, int nbGroup, const char *buffer, Client*clients, int nbClient);
static void send_message_from_historic(Client *c, Group *listGroup, int nbGroup, Conversation *listConversation, int nbConv );
static int create_group(Client *client,char *nomGroup, int nbMembers, char** clientNames, Group *listallGroup, Client *allclients, int nbClient,  int nbGroup);
static void add_client_group(char *nomClient, char* nameA,Group *listGroup, char *nomGroup, int nbGroup, Client *clients, int nbClient);
static void remove_client_group(char *nomClient, char* nameD,Group *listGroup, char *nomGroup, int nbGroup, Client *clients, int nbClient);
static void send_file(Client * client,char* receiverName, char* fileName, Client * listClient, int nbClient);
static void get_client_from_group(const char *nomGroup,  Group *listallGroup, const int nbGroup, Client *receiver);
static void get_group_from_client(Client *client,  Group *listallGroup, const int nbGroup);
#endif /* guard */
