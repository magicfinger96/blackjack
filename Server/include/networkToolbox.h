#ifndef __NETWORK_TOOLBOX__
#define __NETWORK_TOOLBOX__

#include "server.h"

/* ====================== */
/*     HANDLING SIGNAL    */
/* ====================== */
void handleNoMessage();

/* ====================== */
/*           UDP          */
/* ====================== */

int openSocketUDP();
int closeSocketUDP(int socket);
int sendUDPMessage(int sock, char* host, int port, char* msg);
int receiveUDPMessage(int sock, int port, char** message, char** IPClient);
int bindUDP();

/* ====================== */
/*           TCP          */
/* ====================== */

int openSocketTCP();
int closeSocketTCP(int socket);
int connectTCP(int sock, char* host, int port);
int sendDataTCP(int sock, char* msg, Server* server);
char* receiveTCPMessage(int sock, int* status);
int bindTCP(int sock, int port);
int waitTCP(int sock);


#endif
