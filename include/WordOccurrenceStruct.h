#ifndef WORDOCCURRENCESTRUCT_H
#define WORDOCCURRENCESTRUCT_H

struct WordOccurrence
{
	size_t docId;
	size_t wordCount;

	WordOccurrence(size_t id = 0, size_t wc = 0) : docId(id), wordCount(wc) {}

	bool operator==(const WordOccurrence& other) const
	{
		return (docId == other.docId && wordCount == other.wordCount);
	}
};

struct RatedWordOccurrence
{
	size_t docId;
	double rating;

	RatedWordOccurrence(size_t id = 0, double r = 0) : docId(id), rating(r) {}
	RatedWordOccurrence (const WordOccurrence occ) : docId(occ.docId), rating(static_cast<double>(occ.wordCount)) {}

	bool operator==(const RatedWordOccurrence& other) const
	{
		return (docId == other.docId);
	}
};

#endif // !WORDOCCURRENCESTRUCT_H