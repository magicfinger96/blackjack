#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

#include "networkToolbox.h"
#include "error.h"


/* ====================== */
/*     HANDLING SIGNAL    */
/* ====================== */

void handleSigPipe()
{
  printf("Client deconnection\n");
  signal(SIGPIPE,handleSigPipe);
}

void handleNoMessage()
{
  printf("TIMEOUT");
  signal(SIGALRM,handleNoMessage);
  exit(0);
}

/* ====================== */
/*           UDP          */
/* ====================== */

int openSocketUDP()
{
  int fd;

  if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      return -1;
    }

  /* fprintf(stdout,"Socket UDP opened at %d\n",fd); */
  return fd;
}

int closeSocketUDP(int socket)
{
  int res;
  if((res = close(socket)) < 0)
    {
      return CLOSE_SOCKET_ERROR;
    }

  return 0;
  /* fprintf(stdout,"Socket UDP closed\n"); */
}

int sendUDPMessage(int sock, char* host, int port, char* msg)
{
  struct sockaddr_in dest;
  struct hostent* hostent;

  int totalSend = 0;
  int totalSize = strlen(msg);
  
  if((hostent = gethostbyname(host)) == NULL)
    {
      return GET_HOST_BY_NAME_ERROR;
    };

  memset(&dest,0,sizeof(dest));
  dest.sin_family = hostent->h_addrtype;
  dest.sin_port = htons(port);
  memcpy(&(dest.sin_addr.s_addr),hostent->h_addr_list[0], hostent->h_length);

  /* fprintf(stdout, "Sending message \"%s\" to host \"%s\" on port %d\n",msg,host,port); */

  for(;totalSend < totalSize;)
    {
      int nbSend = sendto(sock,msg,strlen(msg),0,(struct sockaddr*)&dest, sizeof(dest));
      if(nbSend == -1)
	{
	  return SEND_TO_ERROR;
	}
      totalSend += nbSend;
    }

  /* fprintf(stdout,"Message sent\n"); */
  return 0;
}

int receiveUDPMessage(int sock, int port, char** message, char** IPClient)
{
  struct hostent* hostp;
  struct sockaddr_in client;

  int allocStrSize = 128;
  char* totalStr = calloc(128,sizeof(char));
  int strSize = 1;
  char buffer[128];

    
  socklen_t clientSize = sizeof(client);
 
  char* host_ip;

  totalStr[0] = '\0';
  memset(&client,0,sizeof(struct sockaddr_in));

  for(;;)
    {
      int msgSize;
  
      msgSize = recvfrom(sock,buffer,128,0,(struct sockaddr*)&client, &clientSize);
      
      if(msgSize == -1)
	{
	  return RECVFROM_ERROR;
	}

      buffer[msgSize] = '\0';

      if(msgSize > 0)
	{
	  strSize += msgSize;
	  if(strSize > allocStrSize)
	    {
	      totalStr = realloc(totalStr,strSize * sizeof(char));
	    }
	  strcat(totalStr,buffer);
	  if(msgSize < 128)
	    {
	      break;
	    }
	}
      else
	{
	  break;
	}
    }

  if((hostp = gethostbyaddr(&client.sin_addr.s_addr,sizeof(client.sin_addr.s_addr), AF_INET)) == NULL)
    {
      return GET_HOST_BY_ADDR_ERROR;
    }

  if((host_ip = inet_ntoa(client.sin_addr)) == NULL)
    {
      return INET_TOA_ERROR;
    }
 
  printf("Receive UDP message \"%s\" (%lu bytes) from host \"%s\" (%s) on port \"%d\".\n",
	 totalStr, strlen(totalStr), hostp->h_name, host_ip, port);
  
  *IPClient = calloc(strlen(host_ip)+1,sizeof(char));
  if(*IPClient != NULL)
    {
      strcpy(*IPClient,host_ip);
    }

  *message = calloc(strlen(totalStr)+1,sizeof(char));
  if(*message != NULL)
    {
      strcpy(*message,totalStr);
    }
  
  free(totalStr);
  return 0;
}

int bindUDP(int socket, int port)
{
  struct sockaddr_in server;
  int optval = 1;

  if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int)) < 0)
    {
      return SETSOCKOPT_ERROR;
    }
  
  memset(&server,0,sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if(bind(socket,(struct sockaddr*)&server,sizeof(server)) < 0)
    {
      return BIND_ERROR;
    }

  return 0;
}

/* ====================== */
/*           TCP          */
/* ====================== */

int openSocketTCP()
{
  int fd;

  if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      return -1;
    }

  /* fprintf(stdout, "Socket opening at %d\n",fd); */
  return fd;
}

int connectTCP(int sock, char* host, int port)
{
    struct sockaddr_in addr;
    struct hostent* he;

    if((he = gethostbyname(host)) == NULL)
    { 
      return GET_HOST_BY_NAME_ERROR;
    }

    memset(&addr,0,sizeof(addr));
    addr.sin_port = htons(port);
    addr.sin_family = he->h_addrtype;
    memcpy(&(addr.sin_addr.s_addr),he->h_addr_list[0], he->h_length);
        
    if(connect(sock,(struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
      return CONNECT_ERROR;
    }

    /* printf("Successfull connected to port %u\n",port); */
    return 0;
}

int sendDataTCP(int sock, char* msg, Server* server)
{
  int total_send;
  int total_size;

  signal(SIGPIPE,handleSigPipe);

  strcat(msg,"\n");
  total_send = 0;
  total_size = strlen(msg);

  for(;total_send < total_size;)
    {
      errno = 0;
      int nb_send = send(sock,msg+total_send,total_size - total_send, 0);
      if(errno == EPIPE)
	{
	  int pos;
	  printf("ERROR AT SENDING MESSAGE TO SOCKET BECAUSE DISCONNECTED  %d\n",sock);
	  pos = getClientPositionFromSocketID(server,sock);
	  if(pos > -1)
	    {
	      removeClientFromList(server,pos);
	      sendDeconnectionTo(server,-1,pos,"general");
	    }
	  errno = 0;
	  return 1;
	}
      else
	{
	  total_send += nb_send;
	  if(nb_send == -1)
	    {
	      return SEND_ERROR;
	    }

	  if(strcmp(msg,"ping\n") != 0)
	    {
	      printf("Send TCP message \"%s\" (%lu bytes)\n",msg,strlen(msg));
	    }
	}
    }
  return 0;
}

char* receiveTCPMessage(int sock, int* status)
{
  char* str = malloc(128 * sizeof(char));
  int str_size = 1;
  int alloc_str_size = 128;
  char buffer[128];

  str[0] = '\0';

  for(;;)
    {
      int msg_size;

      msg_size = recv(sock,buffer,128,0);
      if(msg_size == -1)
	{
	  printf("ERROR AT RECEIVING MESSAGE\n");
	  free(str);
	  *status = RECV_ERROR;
	  return NULL;
	}
      else if(msg_size == 0)
	{
	  /* On a reçu rien, on doit deconnecter 
	     le client qui vient de fermer sa socket,
	     On renvoie ainsi null ,et le thread déconnectera le client */
	  free(str);
	  *status = RECV_ERROR;
	  return NULL;
	}
      
      buffer[msg_size] = '\0';

      if(msg_size > 0)
	{
	  str_size += msg_size;
	  if(str_size > alloc_str_size)
	    str = realloc(str,str_size * sizeof(char));
	  strcat(str,buffer);
	  if(msg_size < 128)
	    {
	      break;
	    }
	} 
    }
  
  *status = 0;
  return str;
	  
}

int bindTCP(int sock, int port)
{
  int optval = 1;
  struct sockaddr_in server;
  
  if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(const void*)&optval,sizeof(int)) == -1)
    {
      return SETSOCKOPT_ERROR;
    }
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  if(bind(sock,(struct sockaddr*)&server, sizeof(server)) < 0)
    {
      return BIND_ERROR;
    }

  if(listen(sock,5) < 0)
    {
      return LISTEN_ERROR;
    }

  return 0;
}

int waitTCP(int sock)
{
  int socketClient = -1;
  struct sockaddr_in incoming;  
  socklen_t sizeOfInc = sizeof(incoming);
  
  memset(&incoming,0,sizeof(incoming));

  if((socketClient=accept(sock,(struct sockaddr*)&incoming, &sizeOfInc)) < 0)
    {
      return -1;
    }

  return socketClient;
}

int closeSocketTCP(int socket)
{
  int res;
  if((res = close(socket)) < 0)
    {
      return CLOSE_SOCKET_ERROR;
    }

  return 0;
  /* fprintf(stdout,"Socket TCP closed\n"); */
}
