#pragma once

#include <utility>
#include <functional>

#include <common.h>

class MenuHandlers
{
private:
	Log &log;
public:
	MenuHandlers(Log &_log):
		log(_log)
			{};

	int open_handler(std::ostream& out, const std::string &filepath);
	int close_handler(std::ostream& out);
	int add_handler(std::ostream& out, const std::string &w, const std::string &t);
	int rem_handler(std::ostream& out, const std::string &w);
	int dump_handler(std::ostream& out);
};
