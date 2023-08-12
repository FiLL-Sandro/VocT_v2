#include "Db.h"

static Log log("Db", Log::LogLevel::LL_DEBUG);

#define FORMAT_JSON \
	"{\n\t\"word\": [ \"translate1\", \"translate2\" ... \"translaten\" ]\n}"

cJSON* Db::to_json()
{
	cJSON *root = cJSON_CreateObject();
	if (!root)
	{
		LOG_ERROR("cannot create root JSON object\n");
		return NULL;
	}

	for (auto i : db)
	{
		cJSON *a = cJSON_AddArrayToObject(root, i.first.c_str());
		if (!a)
		{
			LOG_ERROR("cannot create JSON array");
			goto fail;
		}

		for (auto j : i.second)
		{
			if (!cJSON_AddItemToArray(a, cJSON_CreateString(j.c_str())))
			{
				LOG_ERROR("cannot add item to JSON array\n");
				goto fail;
			}
		}
	}
	return root;
fail:
	cJSON_Delete(root);
	return NULL;
}

int Db::from_json(const cJSON *root)
{
	if (!root)
	{
		return -1;
	}

	cJSON *a;
	cJSON_ArrayForEach(a, root)
	{
		if (!cJSON_IsArray(a))
		{
			LOG_ERROR("wrong format of JSON:\n%s\n", FORMAT_JSON);
			return -1;
		}

		std::vector<std::string> ts;
		cJSON *element;
		cJSON_ArrayForEach(element, a)
		{
			char *t = cJSON_GetStringValue(element);
			if (t)
				ts.push_back(std::string{t});
		}
		db.emplace(std::string{a->string}, ts);
	}
	return 0;
}

Db::Db(const cJSON *root)
{
	from_json(root);
}

int Db::dump(const std::string &filepath)
{
	cJSON *root = to_json();
	if (!root)
	{
		LOG_ERROR("cannot dump the Dictionary to %s\n", filepath.c_str());
		return -1;
	}

	int rc = -1;
	std::ofstream ofs{filepath};
	if (!ofs.is_open())
	{
		LOG_ERROR("cannot open: '%s'\n", filepath.c_str());
		goto exit;
	}
	ofs << cJSON_Print(root) << "\n";
	rc = 0;
exit:
	if (root)
		cJSON_Delete(root);
	return rc;
}

void Db::add(const std::string &w, const std::string &t)
{
	auto it = db.find(w);
	if (it != db.end())
	{
		it->second.push_back(t);
	}
	else
	{
		db.emplace(w, std::vector<std::string>{t});
	}
}

void Db::rem(const std::string &w)
{
	auto it = db.find(w);
	if (it != db.end())
	{
		db.erase(it);
	}
}
