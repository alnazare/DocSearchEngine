#include "InvertedIndex.h"
#include "ThreadPool.h"
#include <cstddef>
#include <iostream>
#include <thread>
#include <sstream>
#include <algorithm>

InvertedIndex::InvertedIndex(const std::vector<std::string>& docs)
{
	updateDocumentBase(docs);
}

/**
* splits the data into tokens, counts the occurrences and saves it for later use; any previous data is cleared
* @param text documents
*/
void InvertedIndex::updateDocumentBase(const std::vector<std::string>& docs)
{
	std::cout << "\nIndexing " << docs.size() << " document(s)...\n";
	docContents_ = docs;
	frequencyDictionary_.clear();

	// go through all the document contents and recount the occurrences
	if (!docContents_.empty())
	{
		ThreadPool pool{};
		for (size_t i{ 0 }; i < docContents_.size(); i++)
		{
			// split a document into tokens
			pool.addTask([this, i]() 
				{	
					std::map<std::string, size_t> localDictionary;
					std::istringstream stream{ docContents_[i] };
					std::string buffer{};
					while (stream >> buffer)
					{
						localDictionary[buffer]++;
					}
					//TODO: token normalization
					for (auto& word : localDictionary)
					{
						std::lock_guard<std::mutex> lock(dictionaryMutex);
						frequencyDictionary_[word.first].push_back({ i, word.second });
					}
				});
		}
		pool.wait();
	}
}

/**
* @return occurrences in docId order (asc.)
*/
std::vector<WordOccurrence> InvertedIndex::getSortedOccurrences(const std::string& word) const
{
	auto it = frequencyDictionary_.find(word);
	if (it != frequencyDictionary_.end())
	{
		auto occurrences{ it->second };
		std::sort(occurrences.begin(), occurrences.end(), [&]
		(const WordOccurrence& a, const WordOccurrence& b)
			{ return a.docId < b.docId; });
		return occurrences;
	}
	return std::vector<WordOccurrence>();
}

/**
* @return amount of documents in which the word occurs
*/
size_t InvertedIndex::getDocumentCount(const std::string& word) const
{
	auto it = frequencyDictionary_.find(word);
	if (it != frequencyDictionary_.end())
	{
		return it->second.size();
	}
	return 0;
}
