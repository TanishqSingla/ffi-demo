#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

std::vector<std::string> files;
std::vector<std::string> metadata;
std::string data;

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

void parse_line(std::string& line, std::vector<std::pair<std::string, std::string>>& pairs) {
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

	auto pair = std::pair<std::string, std::string>(trim(key), trim(value));

	pairs.push_back(pair);
}

void read_metadata(std::string filename) {
	if (!fs::exists(filename)) {
		return;
	}

	std::ifstream in(filename);
	std::string s;

	char flag = 0;
	std::vector<std::pair<std::string, std::string>> pairs;

	while(getline(in, s)) {
		if(s == "---") {
			if(flag)
				break;

			flag = 1;
			continue;
		}

		parse_line(s, pairs);
	}

	std::string json("{");

	std::string entry;
	for(int i = 0; i < pairs.size(); i++) {
		entry = pairs[i].first + ":" + pairs[i].second;
		if(i < pairs.size() - 1) {
			entry += ",";
		}

		json += entry;
	}

	json += "}";
	metadata.push_back(json);
};

std::string get_metadat_from_files() {
	std::vector<std::thread> ths;

	for (auto &file : files) {
		ths.push_back(std::thread(&read_metadata, file));
	}

	for (auto &th : ths) {
		th.join();
	}

	data += "[";

	for(int i = 0; i < metadata.size(); i++) {
		data += metadata[i];
		if(i == metadata.size() - 1)
			continue;
		else
		 data += ",";
	}

	data += "]";
	return data;
}

extern "C" {
	void add_filename (const char* filename) {
		files.push_back(filename);
	}

	const char* get_metadata() {
		get_metadat_from_files();
		const char* result = data.c_str();
		return result;
	}

	void print_files() {
		for(auto &file : files) {
			std::cout << file << std::endl;
		}
	}
}
