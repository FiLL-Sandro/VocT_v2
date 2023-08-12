#include <cJSON.h>
#include <msq_ops.h>

#include "Menu.h"

static int get_open_message(char *buf, size_t size, const std::string &filepath)
{
	int rc = 0;
	cJSON *root = cJSON_CreateObject();
	if (root)
	{
		cJSON_AddStringToObject(root, "filepath", filepath.c_str());

		rc = cJSON_PrintPreallocated(root, buf, static_cast<int>(size), 1);
		cJSON_Delete(root);
	}

	return rc;
}

int MenuHandlers::open_handler(std::ostream& out, const std::string &filepath)
{
	msgbuf_t msg;
	msgbody_t *data = reinterpret_cast<msgbody_t*>(&msg.mtext);
	int rc = get_open_message(data->data, MSQ_MESSAGE_SIZE, filepath);
	if (!rc)
	{
		LOG_ERROR("cannot generate OPEN message\n");
		return -1;
	}

	data->id = MESSAGE_CLI_REQ;
	data->status = STATUS_IN_PROGRESS;
	data->command = COMMAND_OPEN;

	rc = send_msg_sync(DICTIONARY, &msg);
	if (!rc
	    && data->id == MESSAGE_CLI_REP
	    && data->status == STATUS_SUCCESS
	    && data->command == COMMAND_OPEN
	    )
	{
		out << "Dictionary " << filepath << " are openned\n";
	}
	else
		LOG_ERROR("cannot handle OPEN command\n");
	return 0;
}

int MenuHandlers::close_handler(std::ostream& out)
{
	msgbuf_t msg;
	msgbody_t *data = reinterpret_cast<msgbody_t*>(&msg.mtext);

	data->id = MESSAGE_CLI_REQ;
	data->status = STATUS_IN_PROGRESS;
	data->command = COMMAND_CLOSE;

	int rc = send_msg(DICTIONARY, &msg);
	if (!rc)
	{
		out << "Dictionary are closed\n";
	}
	else
		LOG_ERROR("cannot handle CLOSE command\n");
	return 0;
}

static int get_add_message(char *buf, size_t size, const std::string &w, const std::string &t)
{
	int rc = 0;
	cJSON *root = cJSON_CreateObject();
	if (root)
	{
		cJSON_AddStringToObject(root, "word", w.c_str());
		cJSON_AddStringToObject(root, "translate", t.c_str());

		rc = cJSON_PrintPreallocated(root, buf, static_cast<int>(size), 1);
		cJSON_Delete(root);
	}

	return rc;
}

int MenuHandlers::add_handler(std::ostream& out, const std::string &w, const std::string &t)
{
	msgbuf_t msg;
	msgbody_t *data = reinterpret_cast<msgbody_t*>(&msg.mtext);
	int rc = get_add_message(data->data, MSQ_MESSAGE_SIZE, w, t);
	if (!rc)
	{
		LOG_ERROR("cannot generate ADD message\n");
		return -1;
	}

	data->id = MESSAGE_CLI_REQ;
	data->status = STATUS_IN_PROGRESS;
	data->command = COMMAND_ADD;

	rc = send_msg(DICTIONARY, &msg);
	if (!rc)
	{
		out << "Entry " << w << ":" << t << " are added\n";
	}
	else
		LOG_ERROR("cannot handle ADD command\n");
	return 0;
}

static int get_rem_message(char *buf, size_t size, const std::string &w)
{
	int rc = 0;
	cJSON *root = cJSON_CreateObject();
	if (root)
	{
		cJSON_AddStringToObject(root, "word", w.c_str());

		rc = cJSON_PrintPreallocated(root, buf, static_cast<int>(size), 1);
		cJSON_Delete(root);
	}

	return rc;
}

int MenuHandlers::rem_handler(std::ostream& out, const std::string &w)
{
	msgbuf_t msg;
	msgbody_t *data = reinterpret_cast<msgbody_t*>(&msg.mtext);
	int rc = get_rem_message(data->data, MSQ_MESSAGE_SIZE, w);
	if (!rc)
	{
		LOG_ERROR("cannot generate REM message\n");
		return -1;
	}

	data->id = MESSAGE_CLI_REQ;
	data->status = STATUS_IN_PROGRESS;
	data->command = COMMAND_REM;

	rc = send_msg(DICTIONARY, &msg);
	if (!rc)
	{
		out << "Entry " << w << " are removed\n";
	}
	else
		LOG_ERROR("cannot handle REM command\n");
	return 0;
}

int MenuHandlers::dump_handler(std::ostream& out)
{
	msgbuf_t msg;
	msgbody_t *data = reinterpret_cast<msgbody_t*>(&msg.mtext);

	data->id = MESSAGE_CLI_REQ;
	data->status = STATUS_IN_PROGRESS;
	data->command = COMMAND_DUMP;

	int rc = send_msg_sync(DICTIONARY, &msg);
	if (!rc
	    && data->id == MESSAGE_CLI_REP
	    && data->status == STATUS_SUCCESS
	    && data->command == COMMAND_DUMP
	    )
	{
		out << "Dictionary are dumped\n";
	}
	else
		LOG_ERROR("cannot handle DUMP command\n");
	return 0;
}
