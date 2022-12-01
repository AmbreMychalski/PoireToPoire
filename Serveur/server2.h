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
#define MAX_MEM_GROUP     20

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
static void send_message_to_all_clients(Client **clients, Client client, int actual, const char *buffer, char from_server);
static void remove_client(Client **clients, int to_remove, int *actual);
static void clear_clients(Client **clients, int actual);
static Client * getClient(const char *name, Client *listClient, int nbClient);
static void send_message_to_conversation(Conversation* listConversation, const char* senderName, const char* receiverName, 
                                          const char *buffer, Client*clients, int * nbConversations, int nbClient);
static void send_message_to_group(const char *nomClient, char *nomGroup, Group *listGroup, int nbGroup, const char *buffer, Client*clients, int nbClient);
static void send_message_from_historic(Client *c, Group *listGroup, int nbGroup, Conversation *listConversation );
#endif /* guard */
