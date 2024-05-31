#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

struct kv {
	std::string key;
	std::string value;

	kv(std::string key, std::string value) {
		this->value = value;
		this->key = "\"" + key + "\"";
	}
};

std::string trim(std::string& s) {
	std::string::iterator b_it = s.begin();
	std::string::iterator e_it = s.end();

	while(*b_it == ' ') {
		++b_it;
	}

	while(*e_it == ' ') {
		--e_it;
	}

	return std::string(b_it, e_it);
}

const char* dumpJSON(std::vector<kv>& pairs) {
	char json[2048] = "{";

	int it = 1;

	std::string entry;
	for(int i = 0; i < pairs.size(); i++) {
		entry = pairs[i].key + ":" + pairs[i].value;
		if(i < pairs.size() - 1) {
			entry += ",";
		}

		std::strcat(json, entry.c_str());
	}

	std::strcat(json, "}");

	json[strlen(json) + 1] = '\0';
	char* result = (char*)malloc(strlen(json));
	// EXTREME DANGER
	std::strcpy(result, json);
	return result;
}


void parseLine(std::string& line, std::vector<kv>& pairs) {
	std::string::iterator it = line.begin();

	while(it != line.end()) {
		if(*it == ':') {
			break;
		}
		++it;
	}
	auto key = std::string(line.begin(), it);
	++it;
	auto value = std::string(it, line.end());

	kv *result = new kv(trim(key), trim(value));

	pairs.push_back(*result);
}

extern "C" {
	const char* parse_metadata(const char* filename) {
		if(!fs::exists(filename)) {
			return "Error: File does not exist";
		}

		std::vector<kv>	pairs;

		std::ifstream in(filename);
		std::string s;

		char flag = 0;
		while(std::getline(in, s)) {
			if(s == "---") {
				if(flag)
					break;

				flag = 1;
				continue;
			}

			parseLine(s, pairs);
		}

		const char* result = dumpJSON(pairs);
		return result;
	}
}
