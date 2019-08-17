#ifndef __MESSAGE_BUILDER__
#define __MESSAGE_BUILDER__

enum MessageType
  {
    GIVE_INFORMATIONS,
    ASK_INFORMATIONS,
    RECEIVE_INFORMATIONS,
    NONE
  };

typedef enum MessageType MessageType;

/* Generic function */

char* buildMessageHeader(MessageType type, char* pathRequest, char* clientAddress, char* serverAddr, char* message);

#endif 
