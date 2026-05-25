#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include "WordOccurrenceStruct.h"
#include <string>
#include <vector>
#include <map>
#include <mutex>
 

class InvertedIndex
{
private:
	std::vector<std::string> docContents_;
	std::map <std::string, std::vector<WordOccurrence>> frequencyDictionary_;
	std::mutex dictionaryMutex;

public:
	InvertedIndex() = default;
	InvertedIndex(const std::vector<std::string>& docs);

	void updateDocumentBase(const std::vector<std::string>& docs);

	std::vector<WordOccurrence> getSortedOccurrences(const std::string& word) const;
	size_t getDocumentCount(const std::string& word) const;
	bool isEmpty() const { return frequencyDictionary_.empty(); }
};

#endif // !INVERTEDINDEX_H