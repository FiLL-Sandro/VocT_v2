#pragma once

#include <common.h>
#include <cJSON.h>

// typedef std::map< std::string,std::vector<std::string> > Db;
typedef std::pair< std::string,std::vector<std::string> > DbEntry;

class Db
{
private:
	std::map< std::string,std::vector<std::string> > db;

	int dump(const std::string &filepath);

public:
	Db() = default;
	Db(const cJSON *root);

	inline int dump2file(const std::string &filepath) { return dump(filepath); };
	inline int dump2screen(void) { return dump("/dev/stdout"); };

	void add(const std::string &w, const std::string &t);
	void rem(const std::string &w);
	inline void clear(void) { db.clear(); };

	cJSON* to_json();
	int from_json(const cJSON *root);
};
