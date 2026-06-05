#include "ConverterJson.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include <iostream>

int main()
{
	if (!ConverterJson::checkForJsonFiles()) return 1;

	auto config = ConverterJson::getConfig();
	std::cout << "========================================================="
			<< "\nRunning " << config.displayName
			<< "\n=========================================================";

	InvertedIndex index{ ConverterJson::getDocContents() };
	SearchServer server{ &index };
	ConverterJson::putAnswers(server.search(ConverterJson::getRequests(), config.maxResults));
	
	return 0;
}