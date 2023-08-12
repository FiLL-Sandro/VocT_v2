#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MSQ_MESSAGE_SIZE 0x1000

typedef enum module_id
{
	MENU = 0x10,
	DICTIONARY,
	MAX,
	UNKNOWN = MAX
} module_id_e;

typedef enum message_id
{
	MESSAGE_CLI_REQ = 0,
	MESSAGE_CLI_REP,
	MESSAGE_CMD_REQ,
	MESSAGE_CMD_REP,
	MESSAGE_MAX
} message_id_e;

typedef enum command_id
{
	COMMAND_ADD = 0,
	COMMAND_REM,
	COMMAND_OPEN,
	COMMAND_CLOSE,
	COMMAND_DUMP,
	COMMAND_MAX
} command_id_e;

typedef enum command_status
{
	STATUS_SUCCESS = 0,
	STATUS_IN_PROGRESS,
	STATUS_FAILED,
	STATUS_MAX
} command_status_e;

typedef struct msgbody_s
{
	message_id_e id;
	command_status_e status;
	command_id_e command;
	char data[];
} msgbody_t;

typedef struct msgbuf_s
{
	long mtype;
	char mtext[MSQ_MESSAGE_SIZE];
} msgbuf_t;

int msq_init(long _id);
int msq_exist(void);
int msq_rm(void);
int recv_msg_sync(msgbuf_t *msg);
int recv_msg(msgbuf_t *msg);
int send_msg_sync(long _id, msgbuf_t *msg);
int send_msg(long _id, msgbuf_t *msg);

#ifdef __cplusplus
}
#endif
