#pragma once

#include <cstdarg>

#include <string>
#include <cstdio>

#define LOG_ERROR(_fmt, ...) \
	log.error("(%s:%d) " _fmt, __func__, __LINE__, ##__VA_ARGS__)

#define LOG_DEBUG(_fmt, ...) \
	log.debug("(%s:%d) " _fmt, __func__, __LINE__, ##__VA_ARGS__)

class Log
{
public:
	enum class LogLevel
	{
		LL_ERROR = 0,
		LL_INFO,
		LL_DEBUG,
		LL_MAX
	};

private:
	LogLevel level;
	std::string module_name;
	FILE *ofs;

	void print(LogLevel lvl, const char *lvl_prompt, const char *fmt, va_list ap);
	void fprint(LogLevel lvl, const char *lvl_prompt, const char *fmt, va_list ap);

public:
	Log() = delete;
	Log(const std::string &name, LogLevel lvl);
	~Log();

	void error(const char *fmt, ...);
	void debug(const char *fmt, ...);
	void info(const char *fmt, ...);
};
