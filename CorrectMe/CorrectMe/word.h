#include "stdafx.h"

class WordModel
{
public:
	WordModel()
	{
		this->SpellErrors = new std::vector<string>();
	}
	string NewWord;
	string OriginWord;
	std::vector<string>* SpellErrors;
};