#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "Dictionary.h"

static Log log("Dictionary", Log::LogLevel::LL_DEBUG);
static const module_id module_id = DICTIONARY;

void Dictionary::save_dictionary()
{
	std::string _filepath{"/tmp/tmp.json"};

	if (filepath.empty())
	{
		LOG_DEBUG("filepath aren't set -- dictionary will be saved to \"%s\"\n",
		          _filepath.c_str());
	}
	else
		_filepath = filepath;

	db.dump2file(_filepath);
}

char* Dictionary::read_file(const std::string &filepath, size_t &size)
{
	struct stat st;

	if (!stat(filepath.c_str(), &st))
	{
		int fd = open(filepath.c_str(), O_RDONLY);
		if (fd >= 0)
		{
			LOG_DEBUG("size of file: %ld\n", st.st_size);
			void *content = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
			close(fd);
			if (content)
			{
				size = st.st_size;
				return static_cast<char*>(content);
			}
		}
		else
		{
			LOG_ERROR("open: %s\n", strerror(errno));
		}
	}
	else
	{
		LOG_ERROR("stat: %s\n", strerror(errno));
	}
	return NULL;
}

/*
{
	"filepath": "<filepath>"
}
*/
static int parse_open_handler(const char *data, std::string &filepath)
{
	cJSON *root = cJSON_Parse(data);
	if (root)
	{
		cJSON *_filepath = cJSON_GetObjectItemCaseSensitive(root, "filepath");
		if (_filepath)
		{
			filepath = cJSON_GetStringValue(_filepath) ?: "";
			return 0;
		}
	}
	return -1;
}

int Dictionary::open_handler(char *data, msgbody_t *reply)
{
	std::string _filepath;
	if (parse_open_handler(data, _filepath) == -1)
	{
		LOG_ERROR("wrong format of OPEN command:\n%s\n", data);
		return -1;
	}

	if (common::create_file(_filepath) < 0)
	{
		LOG_ERROR("cannot create file: \"%s\"\n", _filepath.c_str());
		return -1;
	}

	filepath = _filepath;

	size_t size;
	char *content = read_file(filepath, size);
	if (content)
	{
		const char *_content = !size ? "{}" : content;

		if (db.from_json(cJSON_Parse(_content)))
		{
			munmap(content, size);
			LOG_ERROR("cannot parse the Dictionary\n");
			goto error;
		}
		munmap(content, size);
	}

	reply->id = MESSAGE_CLI_REP;
	reply->status = STATUS_SUCCESS;
	return 0;
error:
	filepath.clear();
	db.clear();
	return -1;
}

int Dictionary::close_handler(msgbody_t *reply)
{
	save_dictionary();
	filepath.clear();
	db.clear();
	reply->id = MESSAGE_CLI_REP;
	reply->status = STATUS_SUCCESS;
	return 0;
}

static int parse_add_handler(const char *data, std::string &w, std::string &t)
{
	cJSON *root = cJSON_Parse(data);
	if (root)
	{
		cJSON *word = cJSON_GetObjectItemCaseSensitive(root, "word");
		cJSON *translate = cJSON_GetObjectItemCaseSensitive(root, "translate");
		if (word && translate)
		{
			w = cJSON_GetStringValue(word) ?: "";
			t = cJSON_GetStringValue(translate) ?: "";
			return 0;
		}
	}
	return -1;
}

/*
{
	"word": "<word>",
	"translate": "<translate>"
}
*/
int Dictionary::add_handler(char *data, msgbody_t *reply)
{
	std::string w, t;
	if (parse_add_handler(data, w, t) == -1)
	{
		LOG_ERROR("wrong format of ADD command:\n%s\n", data);
		return -1;
	}

	LOG_DEBUG("try to add %s:%s\n", w.c_str(), t.c_str());

	db.add(w, t);
	reply->id = MESSAGE_CLI_REP;
	reply->status = STATUS_SUCCESS;
	return 0;
}

int Dictionary::dump_handler(msgbody_t *reply)
{
	printf("Dictionary content:\n");
	db.dump2screen();
	reply->id = MESSAGE_CLI_REP;
	reply->status = STATUS_SUCCESS;
	return 0;
}

static int parse_rem_handler(char *data, std::string &w)
{
	cJSON *root = cJSON_Parse(data);
	if (root)
	{
		cJSON *word = cJSON_GetObjectItemCaseSensitive(root, "word");
		if (word)
		{
			w = cJSON_GetStringValue(word) ?: "";
			return 0;
		}
	}
	return -1;
}

/*
{
	"word": "<word>"
}
*/
int Dictionary::rem_handler(char *data, msgbody_t *reply)
{
	std::string w;
	if (parse_rem_handler(data, w) == -1)
	{
		LOG_ERROR("wrong format of REM command:\n%s\n", data);
		return -1;
	}

	LOG_DEBUG("try to remove %s\n", w.c_str());
	db.rem(w);
	reply->id = MESSAGE_CLI_REP;
	reply->status = STATUS_SUCCESS;
	return 0;
}

int Dictionary::check_message(msgbuf_t *msg)
{
	msgbody_t *data = reinterpret_cast<msgbody_t*>(&msg->mtext);
	bool send_answer = false;
	int rc = 0;

	switch (data->id)
	{
		case MESSAGE_CLI_REQ:
			{
				switch (data->command)
				{
					case COMMAND_ADD:
						rc = add_handler(data->data, data);
						break;
					case COMMAND_REM:
						rc = rem_handler(data->data, data);
						break;
					case COMMAND_OPEN:
						send_answer = true;
						rc = open_handler(data->data, data);
						break;
					case COMMAND_CLOSE:
						rc = close_handler(data);
						break;
					case COMMAND_DUMP:
						send_answer = true;
						rc = dump_handler(data);
						break;
					default:
						LOG_ERROR("unknown command: id == %d\n", data->command);
						rc = -1;
						break;
				}

				if (rc == -1)
					goto MESSAGE_CLI_REQ_fail;

				if (send_answer)
					send_msg(MENU, msg);
				break;
MESSAGE_CLI_REQ_fail:
				data->id = MESSAGE_CLI_REP;
				data->status = STATUS_FAILED;
				send_msg(MENU, msg);
				break;
			}
			break;
		default:
			LOG_ERROR("unknown message: id == %d\n", data->id);
			break;
	}
	return 0;
}

static inline void usage(void)
{
	printf("Usage:\n"
	       "dictionary <filapath>\n"
	       "	<filepath> -- path to JSON-dictionary file\n"
	       );
}

// int main()
int main(int argc, char const *argv[])
{
	if (common::put_pid("/tmp/Dictionary.pid") == -1)
	{
		LOG_ERROR("cannot create pid-file\n");
		exit(1);
	}

	if (msq_init(module_id))
	{
		LOG_ERROR("cannot initialize IPC\n");
		exit(1);
	}

	Dictionary dictionary;

	while(1)
	{
		msgbuf_t msg;
		if (!recv_msg_sync(&msg))
		{
			dictionary.check_message(&msg);
		}
		else
		{
			LOG_ERROR("error occured when IPC working\n");
			exit(1);
		}
	}

	return 0;
}
