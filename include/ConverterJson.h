#ifndef CONVERTERJSON_H
#define CONVERTERJSON_H

#include "nlohmann/json.hpp"
#include "WordOccurrenceStruct.h"
#include <string>
#include <vector>

struct Configuration
{
	std::string displayName, version;
	size_t maxResults{0};
};

namespace ConverterJson
{
	const std::string jsonFormatVersion{ "0.1" };

	bool checkForJsonFiles();
	std::vector<std::string> getDocContents();
	Configuration getConfig();
	std::vector<std::string> getRequests();
	void putAnswers(const std::vector<std::vector<RatedWordOccurrence>>& answers);
	void regenerateJsonFile(unsigned short which);
};

#endif // !CONVERTERJSON_H