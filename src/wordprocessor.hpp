#ifndef WORDPROCESSOR_HPP
#define WORDPROCESSOR_HPP
#include <string>

class WordProcessor {
public:
    WordProcessor (std::string wordblock) { this->wordblock = wordblock; }
    
    // for each "block" (text in between spaces) get the word and the punctuation on the start and end, and isolate them
    void process (std::string & start, std::string & word, std::string & end) {
        // Handle punctuation	
        start = ""; end = " ";
        int wordch1, wordchlast;
        for (int i = 0; i < wordblock.length(); i++)
        {
            if (iswordchar(wordblock.at(i)))
            {
                word += wordblock.at(i);
            }
        }

        for (int i = 0; i < wordblock.length(); i++)
        {
            if (iswordchar(wordblock.at(i)))
            {
                wordch1 = i;
                break;
            }
        }
        start = wordblock.substr(0, wordch1);

        for (int i = wordblock.length() - 1; i >= 0; i--)
        {
            if (iswordchar(wordblock.at(i)))
            {
                wordchlast = i + 1;
                break;
            }
        }

        // end = wordblock.substr(wordchlast) + " ";
        if (wordchlast >= 0 && wordchlast < wordblock.length()) // prevents OOB Error
        {
            end = wordblock.substr(wordchlast) + " ";
        } 
    }

    // return the word std::string from ^
    std::string getword ()
    {
        std::string s, w, e;
        process (s, w, e);
        w = tolower(w);
        return w;
    }
    
    // tolower an entire std::string
    // Note: Works with stuff like german esset as the letter just stays the same
    // helper, not dependent on member variables
    static std::string tolower (std::string str) {
        for (int i = 0; i < str.length(); i++)
        {
            if (str.at(i) >= 'A' && str.at(i) <= 'Z')
            {
                str[i] += 'a' - 'A';
            }
        }
        return str;
    }

    // Is the character one we want in our Word's word?
    // helper, not dependent on member variables
    static int iswordchar (char ch)
    {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '-' || (ch >= '0' && ch <= '9');
    }



protected:
    std::string wordblock; 
};

#endif
