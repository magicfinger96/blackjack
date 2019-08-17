#ifndef __ERROR
#define __ERROR

/* Memory */
#define ALLOCATION_ERROR 1
#define REALLOCATION_ERROR 2
#define FORK_ERROR 3
#define SHMGET_ERROR 4
#define SHMAT_ERROR 5
#define SHMDT_ERROR 6
#define SHMCTL_ERROR 7
#define THREAD_ERROR 8

/* Blackjack table */
#define NO_MORE_SPACE_ON_TABLE 10
#define PLAYER_IS_NOT_ON_TABLE 11

/* Blackjack player */
#define TRYING_BET_OVER_MONEY 30
#define TRYING_HIT_OVER_21 31
#define TRYING_DOUBLE_WHEN_ALREADY_HIT 32
#define TRYING_DOUBLE_OVER_MONEY 33
#define TRYING_DOUBLE_WITH_BLACKJACK 34
#define TRYING_GET_UNCREATE_HAND 35
#define TRYING_DOUBLE_WHILE_SPLIT 36
#define TRYING_SPLIT_WHEN_ALREADY_HIT 37
#define TRYING_SPLIT_WITHOUT_DOUBLE 38
#define TRYING_SPLIT_WITH_3_ALREADY 39
#define TRYING_DOUBLE_WHEN_ALREADY_DONE 40
#define TRYING_SPLIT_OVER_MONEY 41
#define TRYING_SURREND_ALREADY_ACTION_MADE 42

/* Blackjack game */
#define NOT_EVERY_PLAYER_HAS_END 50

/*=============================*/
/*            NETWORK          */
/*=============================*/

#define GET_HOST_BY_NAME_ERROR 80
#define GET_HOST_BY_ADDR_ERROR 81
#define INET_TOA_ERROR 82
#define BIND_ERROR 83
#define SETSOCKOPT_ERROR 84
#define SEND_TO_ERROR 85
#define RECVFROM_ERROR 86
#define CLOSE_SOCKET_ERROR 87
#define CONNECT_ERROR 88
#define SEND_ERROR 89
#define RECV_ERROR 90
#define LISTEN_ERROR 91
#define OPEN_ERROR 92

#define NO_SPACE_ON_SERVER_ERROR 100
#define UNFIND_CLIENT_ON_SERVER_ERROR 101

#endif