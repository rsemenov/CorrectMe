#include "stdafx.h"
#include "corrector.h"

Corrector::Corrector(Corpus* corpus)
{
	this->Corp = corpus;
}

void Corrector::edits(string word, std::vector<WordModel>* edits)
{
	for (string::size_type i = 0;i < word.size();    i++) //deletions
	{
		WordModel* newWord = new WordModel();
		newWord->OriginWord = word.substr(0, i) + word.substr(i + 1);
		newWord->SpellErrors->push_back(word.substr(i,1)+"|");
		edits->push_back((*newWord)); 
	}
	for (string::size_type i = 0;i < word.size() - 1;i++) //transposition
	{
		WordModel* newWord = new WordModel();
		newWord->OriginWord = word.substr(0, i) + word.substr(i+1,1) + word.substr(i,1) + word.substr(i + 2);
		newWord->SpellErrors->push_back((word.substr(i,2)+"|"+word.substr(i+1,1)+word.substr(i,1)));
		edits->push_back((*newWord)); 
	}
		for (char j = 'a';j <= 'z';++j) {
			for (string::size_type i = 0;i < word.size();    i++) //alterations
			{
				WordModel* newWord = new WordModel();
				newWord->OriginWord = word.substr(0, i) + j + word.substr(i + 1);
				newWord->SpellErrors->push_back(word.substr(i,1)+"|"+j);
				edits->push_back((*newWord)); 
			}
			for (string::size_type i = 0;i < word.size()+1;i++) //insertion
			{
				WordModel* newWord = new WordModel();
				newWord->OriginWord = word.substr(0, i) + j + (word.size()>i?word.substr(i):"");
				newWord->SpellErrors->push_back(word.substr(i,1)+"|"+j+(word.size()>i?word.substr(i,1):""));
				edits->push_back((*newWord));
			}
		}
}

void Corrector::known(std::vector<WordModel>* candidates, std::vector<WordModel>* knowns)
{
	Dictionary::iterator end = this->Corp->UnigramModel->end();

	for(std::vector<WordModel>::iterator i= candidates->begin();i!=candidates->end();i++)
	{
		Dictionary::iterator value = this->Corp->UnigramModel->find(i->OriginWord);
		if(value!=end)
			knowns->push_back((*i));
	}
}

float Corrector::getLanguageProbability(PhraseModel* candidate)
{
	Dictionary::iterator end = this->Corp->BigramModel->end();
	float res = 1;
	if(candidate->Words->size()>1)
	{
		for(int i=0;i<candidate->Words->size()-1;i++)
		{
			string bigram = (*candidate->Words)[i].OriginWord + " " + (*candidate->Words)[i+1].OriginWord;
			Dictionary::iterator value = this->Corp->BigramModel->find(bigram);
			if(value!=end)
				res *= value->second;
		}
	}
	else
	{
		string word = candidate->Words->begin()->OriginWord;
		Dictionary::iterator findres = this->Corp->UnigramModel->find(word);
		if(findres == this->Corp->UnigramModel->end())
			res = (*this->Corp->UnigramModel)[UNSEENKEY];
		else
			res = findres->second;	
		float wordsCount = (*this->Corp->UnigramModel)[SUMMKEY];
		res = res/wordsCount;
	}
	return res<1?res:0;
}

float Corrector::getErrorProbability(PhraseModel* candidate)
{
	Dictionary::iterator end = this->Corp->ErrorModel->end();
	float res = 1;
	for(int i=0;i<candidate->Words->size();i++)
	{
		std::vector<string>* rules = (*candidate->Words)[i].SpellErrors;
		for(int k=0;k<rules->size();k++)
		{
			Dictionary::iterator value = this->Corp->ErrorModel->find((*rules)[k]);
			if(value==end)
				res*= 0.0000000000000001;
			else
				res*=value->second;
		}		
	}
	return res;
}

float Corrector::getSummaryProbability(PhraseModel* candidate)
{
	double langProb = this->getLanguageProbability(candidate);
	if(langProb>0)
	{
		//cout << "Summary probability for "<< candidate->Phrase<<":"<<endl;
		//double errorProb = this->getErrorProbability(candidate);
		//cout << "	Error probability: "<< errorProb <<endl;
		//cout << "	Language probability: "<< langProb <<endl;
	}

	double p =  1*langProb;
	//if(p>0)
		//cout << "	Summary probability: "<< p <<endl;

	return p<1?p:0;
}

int Corrector::CorrectMe(string phrase, set<PhraseModel>* result)
{
	PhraseModel* originPhrase = PhraseModel::Parse(phrase);
	std::vector<PhraseModel>* candidates = new std::vector<PhraseModel>();
	for(int i=0;i<originPhrase->Words->size();i++)
	{
		std::vector<WordModel>* edit1Res = new std::vector<WordModel>();
		this->edits((*originPhrase->Words)[i].OriginWord, edit1Res);
		
		std::vector<WordModel>* editRes = new std::vector<WordModel>();
		editRes->reserve(edit1Res->size()*(edit1Res->size()+10));

		editRes->insert(editRes->end(), edit1Res->begin(), edit1Res->end());

		/*for(std::vector<WordModel>::iterator eIter = edit1Res->begin(); eIter!=edit1Res->end();eIter++)
		{			
			this->edits((*originPhrase->Words)[i].OriginWord, editRes);			
		}*/

		std::vector<WordModel>* knownsRes = new std::vector<WordModel>();
		this->known(editRes, knownsRes);
		knownsRes->push_back((*originPhrase->Words)[i]);
		
		std::vector<WordModel>* phrasewrds= new std::vector<WordModel>();
		for(int j= 0;j!=originPhrase->Words->size();j++)		
			phrasewrds->push_back((*originPhrase->Words)[j]);

		std::vector<WordModel>::iterator iter= phrasewrds->begin() + i;
		
		for(std::vector<WordModel>::iterator k= knownsRes->begin();k!=knownsRes->end();k++)
		{
			iter = phrasewrds->erase(iter);
			iter = phrasewrds->insert(iter,(*k));
			PhraseModel* pm = PhraseModel::Create((*phrasewrds));	
			candidates->push_back((*pm));
		}
	}
	float prob = 0;
	PhraseModel resphrase;
	for(int i=0;i<candidates->size();i++)
	{
		float p =  this->getSummaryProbability(&(*candidates)[i]);
		if(p>0)
		{
			(*candidates)[i].Probability = p;
			result->insert((*candidates)[i]);
			if(prob<p)
			{
				prob = p;
				resphrase = (*candidates)[i];
			}
		}
	}
	return 0;
}