#pragma once
#include "stdafx.h"

class Corpus
{
public:
	Dictionary* ErrorModel;
	Dictionary* BigramModel;
	Dictionary* UnigramModel;
	static Corpus* TrainCorpus(string ruleFile, string letersCountFile, string bigramsFile, string unigramsFile);
};