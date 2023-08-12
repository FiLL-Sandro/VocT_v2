#include "common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static Log log("utility", Log::LogLevel::LL_DEBUG);

std::vector<std::string> common::split_string(const std::string &line, const std::string &delim)
{
	std::vector<std::string> result;
	char *tok = NULL, *save_ptr = NULL;
	char *line2 = strdup(line.c_str());

	if (line2)
	{
		for (tok = strtok_r(line2, delim.c_str(), &save_ptr);
		     tok;
		     tok = strtok_r(NULL, delim.c_str(), &save_ptr))
		{
			std::string string(tok);

			LOG_DEBUG("string == %s\n", string.c_str());
			result.push_back(string);
		}
		free(line2);
	}

	return result;
}

const char* common::ModuleID2str(ModuleID id)
{
	switch(id)
	{
		case ModuleID::MENU:
			return "MENU";
		case ModuleID::DICTIONARY:
			return "DICTIONARY";
		case ModuleID::RECEIVER:
			return "RECEIVER";
		case ModuleID::TEST1:
			return "TEST1";
		case ModuleID::TEST2:
			return "TEST2";
		default:
			return "unknown";
	}
	return "unknown";
}

const char* common::MessageID2str(MessageID id)
{
	switch(id)
	{
		case MessageID::CommandLineRequest:
			return "CommandLineRequest";
		case MessageID::CommandLineReply:
			return "CommandLineReply";
		case MessageID::CommandRequest:
			return "CommandRequest";
		case MessageID::CommandReply:
			return "CommandReply";
		default:
			return "unknown";
	}
	return "unknown";
}

bool common::file_exist(const std::string &filepath)
{
	if (filepath.empty())
	{
		LOG_ERROR("filepath is empty!!!\n");
		return false;
	}

	struct stat sb;
	if (!stat(filepath.c_str(), &sb))
		return true;
	return false;
}

int common::create_file(const std::string &filepath)
{
	if (file_exist(filepath))
		return 1;

	std::ofstream f(filepath);
	if (f.good())
	{
		f.close();
		return 0;
	}
	return -1;
}

int common::put_pid(const std::string &filepath)
{
	if (filepath.empty())
	{
		LOG_ERROR("filepath cannot empty\n");
		return -1;
	}

	pid_t pid = getpid();

	std::ofstream f(filepath);
	if (!f.good())
	{
		LOG_ERROR("cannot create file: %s\n", filepath.c_str());
		return -1;
	}

	f << pid;

	return 0;
}
