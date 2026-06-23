#include "SearchServer.h"
#include <cstddef>
#include <sstream>
#include <algorithm>
#include <iostream>

SearchServer::SearchServer(InvertedIndex* invertedIndex) : invertedIndex_(invertedIndex)
{}

/**
* search for full query matches through the InvertedIndex; for unlimited results per query keep maxResultsPerQuery as 0
* @return answers sorted from most to least relevant
*/
std::vector<std::vector<RatedWordOccurrence>> SearchServer::search(const std::vector<std::string>& queries, size_t maxResultsPerQuery) const
{
	std::cout << "Processing " << queries.size() << " search request(s)...\n";

	try {if (invertedIndex_ == nullptr) throw std::runtime_error("SearchServer is attempting to search with no known InvertedIndex");}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
	}

	std::vector<std::vector<RatedWordOccurrence>> results(queries.size());

	// process each query
	for (size_t i{0}; i<queries.size(); i++)
	{
		if (queries[i].empty()) 
			continue;
		// divide the query into tokens/subqueries
		std::vector<std::string> tokens;
		std::istringstream stream{queries[i]};
		std::string buffer{};
		while (stream >> buffer)
		{
			tokens.push_back(buffer);
		}

		// sort tokens by rarity (asc.) and remove duplicates
		std::sort(tokens.begin(), tokens.end(),
			[&](const std::string& a, const std::string& b)
			{ return invertedIndex_->getDocumentCount(a) < invertedIndex_->getDocumentCount(b); });
		tokens.erase(std::unique(tokens.begin(), tokens.end()), tokens.end());

		// go through each token, starting with the rarest one, and find common docId's 
		const std::vector<WordOccurrence> occs{ invertedIndex_->getSortedOccurrences(tokens[0])};
		if (!occs.empty())
		{
			std::vector<RatedWordOccurrence>& result{ results[i] };
			result.reserve(occs.size());
			for (auto& occ : occs)
			{
				result.push_back(occ);
			}

			// find common docId's, exclude those that don't match the full query
			for (size_t j{ 1 }; j < tokens.size(); j++)
			{
				std::vector<RatedWordOccurrence> currentResult{ result };
				auto occurrences = invertedIndex_->getSortedOccurrences(tokens[j]);
				result.clear();
				if (occurrences.empty())
					break;

				auto it1 = currentResult.begin();
				auto it2 = occurrences.begin();
				while (it1 != currentResult.end() && it2 != occurrences.end())
				{
					if (it1->docId < it2->docId)
						it1++;
					else if (it1->docId > it2->docId)
						it2++;
					else
					{
						it1->rating += (int)it2->wordCount;
						result.push_back(*it1);
						it1++;
						it2++;
					}
				}
				if (result.empty())
					break;
			}

			if (!result.empty())
			{
				// sort each found docId by absolute rating (desc.)
				std::sort(result.begin(), result.end(),
					[&](const RatedWordOccurrence& a, const RatedWordOccurrence& b)
					{ return a.rating > b.rating; });

				// enforce the response limit and calculate relative rating
				if (maxResultsPerQuery != 0 && result.size() > maxResultsPerQuery) result.resize(maxResultsPerQuery);
				unsigned int maxWordCount = result[0].rating;
				for (auto& occurrence : result)
				{
					occurrence.rating /= maxWordCount;
				}
			}
		}
	}
	return results;
}
