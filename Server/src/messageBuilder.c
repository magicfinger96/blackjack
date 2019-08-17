#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "messageBuilder.h"
#include "error.h"

#define TYPE_MESSAGE_LENGTH 5
#define TYPE_PROTOCOL_LENGTH 10
#define TYPE_PATH_LENGTH 100
#define TYPE_AGENT_LENGTH 20

#define BASE_SIZE_NB_CHILD 5



/* Generic function */

char* buildMessageHeader(MessageType type, char* pathRequest, char* addressClient, char* addressServer, char* datas)
{
  char* finalMessage;
  int finalSize;
  int sizeDatas;
  
  char typeMessage[TYPE_MESSAGE_LENGTH];
  char protocol[TYPE_PROTOCOL_LENGTH];
  char path[TYPE_PATH_LENGTH];
  char host[TYPE_AGENT_LENGTH];
  char userAgent[TYPE_AGENT_LENGTH];
  
  strcpy(protocol,"HTTP/1.1");
  strcpy(host,addressServer);
  strcpy(userAgent,addressClient);
  strcpy(path,pathRequest);
  sizeDatas = strlen(datas)+1;
  
  if(type == GIVE_INFORMATIONS)
    {
      strcpy(typeMessage,"POST");
    }
  else if(type == ASK_INFORMATIONS)
    {
      strcpy(typeMessage,"GET");
    }

  /* + 30 for spacing, \n, etc.. */
  finalSize = TYPE_MESSAGE_LENGTH + TYPE_PROTOCOL_LENGTH + TYPE_PATH_LENGTH + (TYPE_AGENT_LENGTH * 2) + sizeDatas + 50;
  finalMessage = calloc(finalSize, sizeof(char));
  if(finalMessage == NULL)
    {
      return NULL;
    }

  if(type == GIVE_INFORMATIONS)
    {
      sprintf(finalMessage,"%s /%s %s\n"
	      "host: %s\n"
	      "user-agent: %s\n"
	      "content-length: %lu\n"
	      "\n"
	      "%s",
	      typeMessage,path,protocol,
	      host,
	      userAgent,
	      strlen(datas),
	      datas);

    }
  else
    {
      sprintf(finalMessage,"%s /%s %s\n"
	      "host: %s\n"
	      "user-agent: %s\n"
	      "\n"
	      "%s",
	      typeMessage,path,protocol,
	      host,
	      userAgent,
	      datas);

    }
  

  return finalMessage;
}
