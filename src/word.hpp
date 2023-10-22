#ifndef WORD_HPP
#define WORD_HPP
#include <string>

using namespace std;

class Word
{
	public:
		Word ()
		{
			setWord ("Undefined", "Undefined", -1, 0);
		}

		Word (string w, string d, int g)
		{
			setWord (w, d, g);
		}

		Word (string w, string d, int g, int f)
		{
			setWord (w, d, g, f);
		}


		void setWord (string w, string d, int g)
		{			
			word = w;
			definition = d;
			grammar = g;
		}

		void setWord (string w, string d, int g, int f)
		{			
			word = w;
			definition = d;
			grammar = g;
			familiarity = f;
		}


		string getword ()
		{
			return word;
		}

		string getgrammar ()
		{
			vector<string> g = {"noun", "verb", "adverb", "article", "adjective", "other"};
			if (grammar < 0) return "Undefined";
			if (grammar > g.size() -1 ) grammar = g.size() -1;
			return g[grammar];
		}
	
		string getfamiliarity ()
		{
			vector<string> f = {"New", "Hard", "Medium", "Easy", "Mastered"};
			if (familiarity > f.size() -1 ) familiarity = f.size() -1;
			return f[familiarity];
		}


		string getdefinition ()
		{
			return definition;
		}

		void setword (string w)
		{
			word = w;
		}

		void setdef (string def)
		{
			definition = def;
		}

		void setgrammar (int g)
		{
			grammar = g;
		}

		void setfam (int f)
		{
			familiarity = f;
		}

	private:
		string word;
		string definition;
		int grammar;
		int familiarity;
};

#endif
