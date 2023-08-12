#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>

#include "msq_ops.h"

#define MSQ_FILEPATH "/tmp/VocT.queue"

#define LOG_ERR(_fmt, ...) \
	printf("FATAL|%s:%d|" _fmt, __func__, __LINE__, ##__VA_ARGS__)

static int qid = -1;
static long id = -1;
static key_t key = -1;

static key_t get_key()
{
	if (key == -1)
	{
		key = ftok(MSQ_FILEPATH, 'x');
		if (key == -1)
			LOG_ERR("cannot generate the key for queue\n");
	}
	return key;
}

int send_msg_sync(long _id, msgbuf_t *msg)
{
	if (send_msg(_id, msg) == -1)
	{
		LOG_ERR("cannot send message\n");
		return -1;
	}

	if (recv_msg_sync(msg) == -1)
	{
		LOG_ERR("cannot receive message\n");
		return -1;
	}

	return 0;
}

int send_msg(long _id, msgbuf_t *msg)
{
	if (qid == -1)
	{
		LOG_ERR("message queue aren't initialized\n");
		return -1;
	}
	if (!msg)
	{
		LOG_ERR("msg cannot be NULL\n");
		return -1;
	}

	msg->mtype = _id;
	if (msgsnd(qid, msg, MSQ_MESSAGE_SIZE, IPC_NOWAIT) == -1)
	{
		LOG_ERR("cannot send message");
		return -1;
	}
	return 0;
}

int recv_msg_sync(msgbuf_t *msg)
{
	if (qid == -1)
	{
		LOG_ERR("message queue aren't initialized\n");
		return -1;
	}
	if (!msg)
	{
		LOG_ERR("msg cannot be NULL\n");
		return -1;
	}

	if (msgrcv(qid, msg, MSQ_MESSAGE_SIZE, id, 0) == -1)
	{
		if (errno != ENOMSG)
		{
			LOG_ERR("cannot take message\n");
			return -1;
		}
	}
	return 0;
}

int recv_msg(msgbuf_t *msg)
{
	if (qid == -1)
	{
		LOG_ERR("message queue aren't initialized\n");
		return -1;
	}
	if (!msg)
	{
		LOG_ERR("msg cannot be NULL\n");
		return -1;
	}

	if (msgrcv(qid, msg, MSQ_MESSAGE_SIZE, id, MSG_NOERROR | IPC_NOWAIT) == -1)
	{
		if (errno != ENOMSG)
		{
			LOG_ERR("cannot take message\n");
			return -1;
		}
	}
	return 0;
}

int msq_rm(void)
{
	key_t key = get_key();
	if (msq_exist() && key != -1)
	{
		int _qid = msgget(key, IPC_CREAT | 0770);
		if (_qid != -1 && msgctl(_qid, IPC_RMID, NULL) == -1)
		{
			LOG_ERR("cannot remove message queue(%s)\n", strerror(errno));
			return -1;
		}
		id = -1;
		qid = -1;
		return 0;
	}
	return 0;
}

int msq_exist(void)
{
	key_t key = get_key();
	if (key == -1)
		return 0;
	return msgget(key, IPC_CREAT | IPC_EXCL | 0770) == -1 && errno == EEXIST;
}

int msq_init(long _id)
{
	if (qid != -1)
	{
		LOG_ERR("message queue already inited\n");
		return 0;
	}

	key_t key = get_key();
	if (key == -1)
	{
		goto fail;
	}

	qid = msgget(key, IPC_CREAT | 0770);
	if (qid == -1)
	{
		LOG_ERR("cannot create message queue\n");
		goto fail;
	}

	id = _id;

	return 0;
fail:
	qid = -1;
	id = -1;
	return -1;
}
