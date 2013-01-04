// CorrectMe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include "corrector.h"

using namespace std;

std::tr1::unordered_map<string, string>* testset;

void BuildTestSet()
{
	testset = new std::tr1::unordered_map<string, string>();
	std::pair<string, string> t1 ("writtings","writings");
	std::pair<string, string> t2 ("exemple","example");
	std::pair<string, string> t3 ("heroe","hero");
	std::pair<string, string> t4 ("sines","sinse");
	//std::pair<string, string> t5 ("schold","scold");
	std::pair<string, string> t6 ("wont","want");
	std::pair<string, string> t7 ("there little birds","three little birds");
	std::pair<string, string> t8 ("the man","the main");
	std::pair<string, string> t9 ("too take","to take");
	std::pair<string, string> t10 ("being abble","being able");
	//std::pair<string, string> t11 ("what to go","what to do");
	std::pair<string, string> t12 ("is returnd","is returned");
	std::pair<string, string> t13 ("the action ratrieves","the action retrieves");
	//std::pair<string, string> t14 ("indevidual blog post","individual blog post");
	std::pair<string, string> t15 ("been workin with","been working with");
	//std::pair<string, string> t16 ("","");
	std::pair<string, string> t17 ("user expirience","user experience");
	std::pair<string, string> t18 ("take a look a this page","take a look at this page");
	
	testset->insert(t1);
	testset->insert(t2);
	testset->insert(t3);
	testset->insert(t4);
	//testset->insert(t5);
	testset->insert(t6);
	testset->insert(t7);
	testset->insert(t8);
	testset->insert(t9);
	testset->insert(t10);
	//testset->insert(t11);
	testset->insert(t12);
	testset->insert(t13);
	//testset->insert(t14);
	testset->insert(t15);
	//testset->insert(t16);
	testset->insert(t17);
	testset->insert(t18);
}

std::set<string>* GetResult(Corrector* corrector, string line)
{
	set<PhraseModel>* results = new set<PhraseModel>();
			int res = corrector->CorrectMe(line, results);
			int max = results->size()<10?results->size():10;
			int k=0;
			set<string>* resset = new set<string>();
			for(set<PhraseModel>::iterator i=results->begin();i!=results->end() && k<10;i++)
			{
				k++;
				resset->insert((*i).Phrase.c_str());
			}
			return resset;
}

int TestCorrector(Corrector* corrector)
{
	int good = 0;
	BuildTestSet();
	for(std::tr1::unordered_map<string, string>::iterator iter = testset->begin(); iter!= testset->end(); iter++)
	{
		std::set<string>* res = GetResult(corrector, iter->first);
		if(res->find(iter->second) != res->end())
			good++;
	}
	return good;
}

string filepref = "d:\\TK\\FuzzyTextSearch\\CorrectMe_1\\CorrectMe\\Debug\\words_";
string ext = ".txt";
int Test(Corrector* corrector)
{
	for(int wc = 3; wc<=3; wc++)
	{
		for(int ec = 1; ec<=1; ec++)
		{			
			int right = 0;
			int all = 0;
			char wcc = '0'+wc;
			char ecc = '0'+ec;
			string file = filepref + wcc + "_" + ecc + ext;
			ifstream fin(file.c_str(), ios_base::binary | ios_base::in);

			if (fin.is_open() == false) {
				return 1; 
			}
			string line;
			while( getline(fin, line) ) { 
				all++;
				int i =0;
				while(line[i]!='|' && i<line.size())
				{
					i++;
				}
				if(i==line.size())
					break;
				string wrong = line.substr(0,i);
				string cor = line.substr(i+1,line.size()-i-2);
				std::set<string>* set = GetResult(corrector,wrong);

				if(set->find(cor) != set->end())
				{
					right++;
					cout << cor;
					cout << " find " << endl;
				}
				else
				{
					cout << cor;
					cout << " not find" << endl;
					for(std::set<string>::iterator j=set->begin();j!=set->end();j++)
					{
						cout << (*j) << endl;
					}
				}
			cout << wcc <<";"<<ecc<<";"<<all<<";"<<right;

			}
			cout << wcc <<";"<<ecc<<";"<<all<<";"<<right;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	/*Corpus* enCorpus = Corpus::TrainCorpus("d:\\TK\\FuzzyTextSearch\\CorrectMe\\count_1edit.txt", 
		"d:\\TK\\FuzzyTextSearch\\CorrectMe\\count_2l.txt", 
		"d:\\TK\\FuzzyTextSearch\\CorrectMe\\count_2w.txt", 
		"d:\\TK\\FuzzyTextSearch\\CorrectMe\\count_1w.txt");
		*/
	Corpus* enCorpus = Corpus::TrainCorpus("count_1edit.txt", 
		"count_2l.txt", 
		"count_2w.txt", 
		"count_1w.txt");

		/*Corpus* enCorpus = Corpus::TrainCorpus("d:\\tasks\\my\\CorrectMe\\count_1edit.txt", 
		"d:\\tasks\\my\\CorrectMe\\count_2l.txt", 
		"d:\\tasks\\my\\CorrectMe\\count_2w.txt", 
		"d:\\tasks\\my\\CorrectMe\\count_1w.txt");*/

		Corrector* corrector = new Corrector(enCorpus);
		cout<< "Testing..." << endl;
		//cout<<	"TestResult is ";
		//cout<< TestCorrector(corrector) <<endl;
		//Test(corrector);

		BuildTestSet();
		for(std::tr1::unordered_map<string, string>::iterator iter = testset->begin(); iter!= testset->end(); iter++)
		{
			set<PhraseModel>* results = new set<PhraseModel>();
			cout << "---------------------------------------------" <<endl;			
			cout << "Input word: " << iter->first << endl;
			int res = corrector->CorrectMe(iter->first, results);
			int max = results->size()<10?results->size():10;
			int k=0;
			
			for(set<PhraseModel>::iterator i=results->begin();i!=results->end() && k<10;i++)
			{
				if(k==0)
					cout << "Correct word: ";
				if(k==1)
					cout << "Other results: " << endl;

				k++;
				
				cout << (*i).Phrase.c_str();
				cout << "\t\t";
				cout << (*i).Probability;
				cout << "\n";
			}
		}
		while (true)
		{
			cout << "---------------------------------------------" <<endl;
			string line;
			cout << "Enter your phrase: " << endl;
			getline(std::cin, line);		
			set<PhraseModel>* results = new set<PhraseModel>();
			int res = corrector->CorrectMe(line, results);
			int max = results->size()<10?results->size():10;
			int k=0;
			for(set<PhraseModel>::iterator i=results->begin();i!=results->end() && k<10;i++)
			{
				if(k==0)
					cout << "Correct word: ";
				if(k==1)
					cout << "Other results: " << endl;

				k++;
				cout << (*i).Phrase.c_str();
				cout << " ";
				cout << (*i).Probability;
				cout << "\n";
			}
		}

	//d:\tasks\my\CorrectMe\CorrectMe\
	return 0;
}



