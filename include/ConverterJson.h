#ifndef CONVERTERJSON_H
#define CONVERTERJSON_H

#include "nlohmann/json.hpp"
#include "WordOccurrenceStruct.h"
#include <cstddef>
#include <string>
#include <vector>

struct Configuration
{
	std::string displayName, version;
	size_t maxResults{0};
};

class ConverterJson
{
public:
	static std::string jsonFormatVersion() { return "0.1"; };

	static bool checkForJsonFiles();
	static std::vector<std::string> getDocContents();
	static Configuration getConfig();
	static std::vector<std::string> getRequests();
	static void putAnswers(const std::vector<std::vector<RatedWordOccurrence>>& answers);
	static void regenerateJsonFile(unsigned short which);
};

#endif // !CONVERTERJSON_H