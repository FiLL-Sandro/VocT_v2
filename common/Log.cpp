#include <cstdlib>

#include "Log.h"

#define ERROR_PROMPT "ERROR > "
#define DEBUG_PROMPT "DEBUG > "
#define INFO_PROMPT  "INFO  > "

Log::Log(const std::string &name, LogLevel lvl):
	level(lvl),
	module_name(name)
{
	std::string filepath{ "/tmp/" };

	filepath += module_name + ".log";
	ofs = fopen(filepath.c_str(), "w");
	if (!ofs)
	{
		printf("FATAL ERROR: cannot create a log file!!!\n");
		exit(1);
	}
}

Log::~Log()
{
	fclose(ofs);
}

void Log::fprint(LogLevel lvl, const char *lvl_prompt, const char *fmt, va_list ap)
{
	if (lvl <= level)
	{
		fprintf(ofs, "%s", lvl_prompt);
		vfprintf(ofs, fmt, ap);
		fflush(ofs);
	}
}

void Log::print(LogLevel lvl, const char *lvl_prompt, const char *fmt, va_list ap)
{
	if (lvl <= level)
	{
		printf("%s:%s", module_name.c_str(), lvl_prompt);
		vprintf(fmt, ap);
	}
}

void Log::error(const char *fmt, ...)
{
	va_list ap;
	va_list ap1;

	va_start(ap, fmt);
	va_copy(ap1, ap);
	print(LogLevel::LL_ERROR, ERROR_PROMPT, fmt, ap);
	fprint(LogLevel::LL_ERROR, ERROR_PROMPT, fmt, ap1);
	va_end(ap);
	va_end(ap1);
}

void Log::debug(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprint(LogLevel::LL_DEBUG, DEBUG_PROMPT, fmt, ap);
	va_end(ap);
}

void Log::info(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprint(LogLevel::LL_INFO, INFO_PROMPT, fmt, ap);
	va_end(ap);
}
