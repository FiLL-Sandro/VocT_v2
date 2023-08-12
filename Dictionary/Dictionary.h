#pragma once

#include <common.h>

#include <msq_ops.h>

#include "cJSON.h"
#include "Db.h"

class Dictionary
{
private:
	std::string filepath;
	Db db;

	void save_dictionary();
	static char* read_file(const std::string &filepath, size_t &size);

	int open_handler(char *data, msgbody_t *reply);
	int close_handler(msgbody_t *reply);
	int add_handler(char *data, msgbody_t *reply);
	int rem_handler(char *data, msgbody_t *reply);
	int dump_handler(msgbody_t *reply);

public:
	int check_message(msgbuf_t *msg);
};
