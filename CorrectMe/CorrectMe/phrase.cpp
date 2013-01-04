#include "stdafx.h"
#include "phrase.h"

PhraseModel::PhraseModel()
{
	this->Words = new std::vector<WordModel>();
	this->eps = 0.0000000000000000000000000000001;
}

PhraseModel* PhraseModel::Parse(string phrase)
{
	PhraseModel* m = new PhraseModel();
	m->Phrase = phrase;	
	string w = "";
	for(int i=0;i<phrase.size();i++)
	{
		 if(phrase[i]==' ')		 
		 {
			 WordModel* wm = new WordModel();
			 wm->OriginWord = w;
			 m->Words->push_back(*wm);
			 w="";		 
		 }
		 else		 
			 w+=phrase[i];		 
	}

	WordModel* wm = new WordModel();
	wm->OriginWord = w;
	m->Words->push_back(*wm);

	return m;
}

PhraseModel* PhraseModel::Create(std::vector<WordModel> words)
{
	PhraseModel* m = new PhraseModel();
	for(int k=0;k< words.size();k++)
		m->Words->push_back(words[k]);
	for(std::vector<WordModel>::iterator i=words.begin();i!=words.end()-1;i++)
		m->Phrase += i->OriginWord + " ";
	m->Phrase += (words.end()-1)->OriginWord;
	return m;
}
