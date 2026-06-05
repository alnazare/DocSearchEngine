#ifndef SEARCHSERVER_H
#define SEARCHSERVER_H

#include "InvertedIndex.h"
#include "WordOccurrenceStruct.h"
#include <vector>

class SearchServer
{
private:
	InvertedIndex* invertedIndex_;
public:
	SearchServer(InvertedIndex* invertedIndex);

	void setInvertedIndex(InvertedIndex* invertedIndex) { invertedIndex_ = invertedIndex; }

	std::vector<std::vector<RatedWordOccurrence>> search(const std::vector<std::string>& queries, size_t maxResultsPerQuery = 0) const;
};

#endif // !SEARCHSERVER_H