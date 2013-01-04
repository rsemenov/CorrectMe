#include "stdafx.h"
#include "word.h"

class PhraseModel
{
public:
	string Phrase;
	float Probability;
	std::vector<WordModel>* Words;
	PhraseModel();
	static PhraseModel* Parse(string phrase);
	static PhraseModel* Create(std::vector<WordModel> words);
	
	bool operator<(const PhraseModel& phrase) const
	{
		if(std::abs(Probability - phrase.Probability)<eps )
		{
			int res = Phrase.compare(phrase.Phrase);
			return res>0;
		}
		else
			return (Probability - phrase.Probability)>0;
	}
private:
	double eps;
};

