#include "stdafx.h"
#include "corpus.h"
#include <algorithm>

void GoodTuringSmoothing(Dictionary* model)
{
	Dictionary::iterator iter = model->end();
	iter--;
	float minCount = iter->second;
	int n0 = 0;
	
	while(iter->second == minCount && iter!=model->begin())
	{
		n0++;
		iter--;
	}

	Dictionary::iterator groupIter = model->end();
	groupIter--;
	float nr = (float)n0;
	int r=1;
	float sumwords = 0;
	while(iter!=model->begin())
	{
		float nrp1 = 0;
		float rp1value = iter->second;
		while(iter->second == rp1value && iter!=model->begin())
		{
			nrp1++;
			iter--;
		}
		float rvalue = groupIter->second;
		float n_r = (rvalue+1)*nrp1/nr;
		while(groupIter->second==rvalue && groupIter!=model->begin())
		{
			groupIter->second = n_r;
			groupIter--;
		}

		sumwords += nr*rvalue;
		nr = nrp1;
	}
	//TODO: What to do with "the" ?
	std::pair<std::string, float> unseen (UNSEENKEY,n0);
	std::pair<std::string, float> summ (SUMMKEY, sumwords);
	model->insert(unseen);
	model->insert(summ);
}

Dictionary* GetLettersCounts(string lCountsFile)
{
	cout << "Creating letters dictionary...\n";
	Dictionary* lcounts = new Dictionary();
	ifstream fin(lCountsFile.c_str(), ios_base::binary | ios_base::in);

	if (fin.is_open() == false) {
		return lcounts;
	}
	string line;
	while( getline(fin, line) ) { 
		string l1 = line.substr(0,1);
		string l2 = line.substr(1,1);		
		string l = l1+l2;
		string n = line.substr(3,line.size()-3);
		float count = (float)atof(n.c_str());
		(*lcounts)[l1]+=count;
		(*lcounts)[l2]+=count;
		(*lcounts)[l]+=count;
	}
	cout << "Letters dictionary created.\n";
	cout << "Good-Turing for letters counts..." << endl;
	GoodTuringSmoothing(lcounts);
	cout << "Good-Turing for letters finished" << endl;
	return lcounts; 
}

Dictionary* GetUnigramCount(string unigramsFile)
{
	cout << "Creating unigrams dictionary...\n";
	Dictionary* lcounts = new Dictionary();
	int n = 10000000;
	lcounts->rehash(std::ceil(n*1.0 / lcounts->max_load_factor()));

	ifstream fin(unigramsFile.c_str(), ios_base::binary | ios_base::in);

	if (fin.is_open() == false) {
		return lcounts;
	}
	string line;
	while( getline(fin, line) )
	{
		string::size_type i = 0;
		string word="";
		while(line[i]!=9)
		{
			word+=line[i++];
		}
		string n = line.substr(i,line.size()-i);
		double count = atof(n.c_str());
		std::pair<string, float> uniPair (word, count);
		lcounts->insert(uniPair);
	}
	cout << "Unigrams dictionary created.\n";
	return lcounts;
}

Dictionary* GetBigramModel(Dictionary* unicounts, string bigramsFile, string unigramsFile)
{
	cout << "Creating bigrams dictionary...\n";
	Dictionary* bicounts = new Dictionary();

	int n_ = 10000000;
	bicounts->rehash(std::ceil(n_ / bicounts->max_load_factor()));

	ifstream fin(bigramsFile.c_str(), ios_base::binary | ios_base::in);

	if (fin.is_open() == false) {
		return bicounts;
	}
	
	string line;
	
	while( getline(fin,line) )
	{
		string parts[2] = {""};
		string::size_type i = 0;
		int j=0;
		while(line[i]!=9)
		{
			if(line[i]==' ')
			{	
				j++; i++;
			}
			else			
				parts[j] += line[i++];

		}
		string n = line.substr(i,line.size()-i);
		float count = (float) atof(n.c_str());
		
		string bigram = parts[0]+" "+parts[1];

		(*unicounts)[parts[0]] += count;
		std::pair<string, float> bigrPair (bigram,count);
		bicounts->insert(bigrPair);
	}
	cout << "Bigrams dictionary created.\n";
	return bicounts;
}

// -------------------------- Normalization ---------------------------------------------------------------------

void ExtendUnigrams(Dictionary* unigrams, Dictionary* bigramsUnigrams)
{
	Dictionary* newWords = new Dictionary();

	Dictionary::iterator biUniIter = bigramsUnigrams->begin();
	float unigramNormalizeCoef = (float) 1.2;
	while(biUniIter!=bigramsUnigrams->end())
	{
		Dictionary::iterator unifind = unigrams->find(biUniIter->first);
		if(unifind == unigrams->end())
		{
			string lowcasewrd = biUniIter->first; //trying with lower case
			std::transform(lowcasewrd.begin(), lowcasewrd.end(), lowcasewrd.begin(), ::tolower);
			unifind = unigrams->find(lowcasewrd);
			if(unifind == unigrams->end())
			{
				Dictionary::iterator find = newWords->find(lowcasewrd);
				if(find == newWords->end())
				{			
					std::pair<string, float> pair (lowcasewrd, 0);
					newWords->insert(pair);
					find = newWords->find(pair.first);
				}
				find->second += unigramNormalizeCoef * biUniIter->second;
			}
		}
		biUniIter++;
	}

	for(Dictionary::iterator iter = newWords->begin();iter!=newWords->end();iter++)
	{
		std::pair<string, float> pair (iter->first,iter->second);
		unigrams->insert(pair);
	}
}

void NormalizeModel(Dictionary* unigrams, Dictionary* bigrams, Dictionary* bigramsUnigrams)
{
	cout << "Normalizing model..." <<endl;
	ExtendUnigrams(unigrams, bigramsUnigrams);

	cout << "Good-Turing for unigrams..." <<endl;
	GoodTuringSmoothing(unigrams);
	cout << "Good-Turing for unigrams finished." <<endl;

	Dictionary::iterator biIter = bigrams->begin();
	while(biIter != bigrams->end())
	{
		string firstWord = "";
		for(int i=0; i < biIter->first.length() && biIter->first[i]!=' ';i++)
			firstWord+=biIter->first[i];
		
		Dictionary::iterator finded = unigrams->find(firstWord);
		float unicount = 0;
		if(finded == unigrams->end())
		{
			string lowcase = firstWord; //trying lower case unigram
			std::transform(lowcase.begin(), lowcase.end(), lowcase.begin(), ::tolower);
			finded = unigrams->find(lowcase);
			if(finded==unigrams->end())
				finded = unigrams->find(UNSEENKEY);			
		}
		unicount = finded->second;
		
		biIter->second = biIter->second/unicount;
		if(biIter->second>1)
		{

		}
		biIter++;
	}

	cout << "Normalizing model finished." << endl;
}

Corpus* Corpus::TrainCorpus(string ruleFile, string letersCountFile, string bigramsFile, string unigramsFile)
{
	Corpus* corp = new Corpus();
	corp->ErrorModel = new Dictionary();
	Dictionary* lcounts = GetLettersCounts(letersCountFile);
	cout << "Creating errors model dictionary...\n";

	ifstream fin(ruleFile.c_str(), ios_base::binary | ios_base::in);
	if (fin.is_open() == false) {
		return corp;
	}
	string line;
	while( getline(fin, line) ) {  
		string rule = "";
		string parts[2]; 
		string::size_type i=0;		int j=0;
		while(line[i]!=9)
		{
			if(line[i] == '|')
			{
				j++;i++;
			}
			else
				parts[j]+=line[i++];
		}
		i++;		
		string letersKey; 
		if(parts[0].size() * parts[1].size() % 2 == 0) 
			letersKey = parts[0]; // del, ins or trans
		else
			letersKey = parts[1]; //sub

		rule += parts[0]+'|'+parts[1];
		int num = atoi(line.substr(i,line.size()-i).c_str());

		float nn;
		Dictionary::iterator findres = lcounts->find(letersKey);
		if(findres == lcounts->end())
			nn = 0;// (*lcounts)[UNSEENKEY];
		else
			nn  = findres->second;
		(*corp->ErrorModel)[rule]+=num/nn;
	}
	cout << "Errors model created.\n";

	cout << "Creating language model...\n";
	Dictionary* unigrams = GetUnigramCount(unigramsFile);

	Dictionary* bigramsUnigrams = new Dictionary();
	int n = 10000000;
	bigramsUnigrams->rehash(std::ceil(n / bigramsUnigrams->max_load_factor()));

	Dictionary* bigrams = GetBigramModel(bigramsUnigrams, bigramsFile, unigramsFile);
	NormalizeModel(unigrams, bigrams, bigramsUnigrams);

	corp->UnigramModel = unigrams;
	corp->BigramModel = bigrams;

	cout << "Language model created.\n";

	cout << "Corpus model created.\n";
	return corp;
}


