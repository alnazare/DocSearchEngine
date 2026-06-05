#include "gtest/gtest.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include <algorithm>
#include <vector>

void PrintTo(const WordOccurrence& v, std::ostream* os)
{
	*os << "(" << v.docId << ", " << v.wordCount << ")";
}
void PrintTo(const RatedWordOccurrence& v, std::ostream* os) 
{
	*os << "(" << v.docId << ", " << v.rating << ")";
}

// InvertedIndex tests
void testInvertedIndexFunctionality(const std::vector<std::string>& docs, const std::vector<std::string>& requests, const std::vector<std::vector<WordOccurrence>>& expected)
{
	InvertedIndex idx;
	idx.updateDocumentBase(docs);

	std::vector<std::vector<WordOccurrence>> result{};
	for (auto& request : requests)
	{
		result.push_back(idx.getSortedOccurrences(request));
	}

	ASSERT_EQ(result, expected);
}

TEST(InvertedIndexTestCase, TestBasic1)
{
	const std::vector<std::string> docs{
		"london is the capital of great britain",
		"big ben is the nickname for the great bell of the striking clock"
	};
	const std::vector<std::string> requests{ "london", "the"};
	const std::vector<std::vector<WordOccurrence>> expected{ {{0,1}}, {{0,1},{1,3}} };
	testInvertedIndexFunctionality(docs, requests, expected);
}

TEST(InvertedIndexTestCase, TestBasic2)
{
	const std::vector<std::string> docs{
		"milk milk milk milk water water water",
		"milk water water",
		"milk milk milk milk milk water water water water water waterwater",
		"americano cappuccino"
	};
	const std::vector<std::string> requests{ "milk", "water", "cappuccino"};
	const std::vector<std::vector<WordOccurrence>> expected{ {{0, 4},{1, 1},{2,5}}, {{0,3},{1,2},{2,5}}, {{3,1}} };
	testInvertedIndexFunctionality(docs, requests, expected);
}

TEST(InvertedIndexTestCase, TestBasic3)
{
	const std::vector<std::string> docs{
		"a b c d e f g h i j k l", "negative statement"
	};
	const std::vector<std::string> requests{ "m", "statement"};
	const std::vector<std::vector<WordOccurrence>> expected{ {}, {{1,1}} };
	testInvertedIndexFunctionality(docs, requests, expected);
}


// SS tests
TEST(SearchServerTestCase, TestBasic1)
{
	const std::vector<std::string> docs{
		"milk milk milk milk water water water",
		"milk water water",
		"milk milk milk milk milk water water water water water waterwater",
		"americano cappuccino"
	};
	const std::vector<std::string> requests{ "milk water", "sugar"};
	const std::vector<std::vector<RatedWordOccurrence>> expected{ {{2, 1.0},{0, 0.7},{1, 0.3}}, {} };

	InvertedIndex idx; idx.updateDocumentBase(docs);
	SearchServer server(&idx);
	ASSERT_EQ(expected, server.search(requests));
}

TEST(SearchServerTestCase, TestBasic2)
{
	const std::vector<std::string> docs{
		"london is the capital of great britain",
		"paris is the capital of france",
		"berlin is the capital of germany",
		"rome is the capital of italy",
		"madrid is the capital of spain",
		"lisbon is the capital of portugal",
	};
	const std::vector<std::string> requests{ "the capital of france" };
	const std::vector<std::vector<RatedWordOccurrence>> expected{ {{1,1.0}} };

	InvertedIndex idx; idx.updateDocumentBase(docs);
	SearchServer server(&idx);
	ASSERT_EQ(expected, server.search(requests));
}