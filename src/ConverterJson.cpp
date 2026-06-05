#include "ConverterJson.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <format>

namespace fs = std::filesystem;

/**
* looks for config.json and requests.json, lets the user to regenerate them if any are missing
*/
bool ConverterJson::checkForJsonFiles()
{
	bool ok{ true };
	std::ifstream file("config.json");
	if (!file.is_open())
	{
		ok = false;
		while (true)
		{
			std::cout << "Couldn't find config.json. Create one? (y/n)\n";
			char input{};
			std::cin >> input;
			if (input == 'y')
			{
				regenerateJsonFile(1);
				break;
			}
			if (input == 'n')
				break;
		}
	}
	file.close();
	file.open("requests.json");
	if (!file.is_open())
	{
		ok = false;
		while (true)
		{
			std::cout << "Couldn't find requests.json. Create one? (y/n)\n";
			char input{};
			std::cin >> input;
			if (input == 'y')
			{
				regenerateJsonFile(2);
				break;
			}
			if (input == 'n')
				break;
		}
	}
	file.close();
	return ok;
}

/**
* @return the string contents of all documents listed in config.json
*/
std::vector<std::string> ConverterJson::getDocContents()
{
	try
	{
		std::ifstream input("config.json");
		if (!input.is_open()) throw std::exception("No config.json found");
		nlohmann::json j{ nlohmann::json::parse(input) };
		input.close();

		if (!j.count("config")) throw std::exception("Invalid config.json (missing config containter)");

		unsigned int invalidPathCounter{ 0 };
		if (j.count("files"))
		{
			std::vector<std::string> contents;
			contents.reserve(j["files"].size());
			std::cout << "\nAccessing " << j["files"].size() << " path(s)...";

			auto getDocContent = [&contents, &invalidPathCounter](const fs::path& path)
				{
					// read the document
					std::ifstream file{ path };
					if (file.is_open())
					{
						if (path.extension() != ".txt" && path.extension() != ".md") return;
						std::string doc, buffer;
						while (getline(file, buffer))
						{
							doc += buffer + '\n';
						}
						contents.push_back(doc);
					}
					else
					{
						invalidPathCounter++;
					}
				};

			// go through each file and folder specified
			for (auto& path : j["files"])
			{
				if (fs::exists(path))
				{
					if (fs::is_directory(path))
					{
						for (auto& it : fs::recursive_directory_iterator(path))
						{
							if (fs::is_regular_file(it))
							{
								getDocContent(it);
							}
						}
					}
					else
					{
						getDocContent(path);
					}
				}
				else
				{
					invalidPathCounter++;
				}
			}
			if (invalidPathCounter)
			{
				std::cout << "\nWARNING: " << invalidPathCounter << " path(s) in config.json couldn't be accessed, including nested files";
			}
			return contents;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "\nERROR: " << e.what() << '\n';
	}
	return std::vector<std::string>{};
}

/**
* @return all settings from config.json excluding document paths
*/
Configuration ConverterJson::getConfig()
{
	try
	{
		std::ifstream input("config.json");
		if (!input.is_open()) throw std::exception("No config.json found");
		nlohmann::json j{ nlohmann::json::parse(input) };
		input.close();
		if (!j.count("config")) throw std::exception("Invalid config.json (missing config containter)");
		if (!j.at("config").count("max_responses")) throw std::exception("Invalid config.json (missing max_responses)");

		Configuration config;
		config.version = j["config"]["version"];
		if (config.version != jsonFormatVersion) throw std::exception("Invalid config.json (wrong version)");
		config.displayName = j["config"]["name"];
		config.maxResults = j["config"]["max_responses"];
		return config;
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
		while (true)
		{
			std::cout << "Create a clean config.json? (y/n)\n";
			char input{};
			std::cin >> input;
			if (input == 'y')
			{
				regenerateJsonFile(1);
				break;
			}
			if (input == 'n')
				break;
		}
	}
	return Configuration();
}

/**
* @return all queries from requests.json
*/
std::vector<std::string> ConverterJson::getRequests()
{
	try
	{
		std::ifstream input("requests.json");
		if (!input.is_open()) throw std::exception("No requests.json found");
		nlohmann::json j{ nlohmann::json::parse(input) };
		input.close();

		if (!j.count("requests")) 
			throw std::exception("Invalid requests.json (missing requests container)");
		if (j.at("requests").empty()) std::cout << "WARNING: requests.json has no requests included\n";
		std::vector<std::string> vec = j.at("requests");
		return vec;
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
		while (true)
		{
			std::cout << "Create a clean requests.json? (y/n)\n";
			char input{};
			std::cin >> input;
			if (input == 'y')
			{
				regenerateJsonFile(2);
				break;
			}
			if (input == 'n')
				break;
		}
	}
	return std::vector<std::string>();
}

/**
* writes the results to answers.json according to the template
*/
void ConverterJson::putAnswers(const std::vector<std::vector<RatedWordOccurrence>>& answers)
{
	nlohmann::ordered_json j;
	unsigned int idCount{ 0 };
	if (!answers.empty())
	{
		for (auto& requestResult : answers)
		{
			idCount++;
			const std::string requestId{ std::format("request{:03}", idCount) };
			if (!requestResult.empty())
			{
				j["answers"][requestId] = { {"result", "true"}, {"relevance", nlohmann::json::array()} };
				for (auto& relevance : requestResult)
				{
					j["answers"][requestId]["relevance"].push_back(nlohmann::ordered_json{
						{"docid", relevance.docId},
						{"rank", relevance.rating}
						});
				}
			}
			else
			{
				j["answers"][requestId]["result"] = "false";
			}
		}
	}
	else
	{
		j = { {"answers", nlohmann::json::object()}};
	}
	std::ofstream output;
	output.open("answers.json");
	output << j.dump(4);
	output.close();
	std::cout << "SUCCESS: answers.json has been updated\n";
}

void ConverterJson::regenerateJsonFile(unsigned short which)
{
	std::ofstream output;
	nlohmann::ordered_json j;
	switch (which)
	{
	case 1:
		output.open("config.json");
		j = {
			{"config",
				{{"name", ""},
				{"version", jsonFormatVersion},
				{"max_responses", 0}}
			},
			{"files", nlohmann::json::array()}
			};
		break;

	case 2:
		output.open("requests.json");
		j = {{"requests", nlohmann::json::array()}};
		break;

	default:
		return;
	}
	output << j.dump(4);
	output.close();
}
