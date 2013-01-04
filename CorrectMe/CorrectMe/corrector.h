#pragma once

#include "stdafx.h"
#include "corpus.h"
#include "phrase.h"

class Corrector
{
public:
	Corpus* Corp;
	Corrector(Corpus* corpus);
	int Corrector::CorrectMe(string phrase, set<PhraseModel>* result);
private:
	void edits(std::string word, std::vector<WordModel>* result);
	void known(std::vector<WordModel>* candidates, std::vector<WordModel>* knowns);
	float getLanguageProbability(PhraseModel* candidate);
	float getErrorProbability(PhraseModel* candidate);
	float getSummaryProbability(PhraseModel* candidate);

};