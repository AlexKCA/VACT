#include <ncurses.h>
#include <vector>
#include <cstring>
#include <chrono>
#include <ctime>  
#include "filereader.hpp"
#include "word.hpp"
#include "sllist.hpp"
#include "constants.hpp"
#include "wordwrapper.hpp"
#include "wordhelper.hpp"
#include "wordcolorizer.hpp"

using namespace std;


WINDOW * text_box;
WINDOW * text_window;
WINDOW * tt_box;
WINDOW * tt_window;
string bookname = "";
char inpch;
int WORD_BUFFER_SIZE = 500; 
vector<string> textstrblocks;
vector<Word> dictionaryWords;
int first_word = 0; 
int activeword = 0;

int writeDict ();
SLList<Word> readDictWords ();
int refresh_tt (void);
int fixori (void);
int isdefinedword (string word);
int print_words (void);
void dodef (string& definition);
void dogram (int& grammar);
void dofam (int& familiarity);
void writeentry (Word w);
Word getactiveword (string word);
int ncurses_init();
int choose_text();
int main_init();
int main_loop();
int end_deinit();

template <class T>
bool vector_contains (vector<T> vec, T thing);

struct Pos
{
	int x;
	int y;
};

Pos cursor;

int main (void)
{
    ncurses_init();
    choose_text();
    main_init();
    main_loop();
    end_deinit();
	return 0;
}


int main_loop () {
    // main loop
	while ((inpch = wgetch(text_window)) != 'q')
	{ 
		switch(inpch)
		{ 
			case 'x':
                if (WORD_BUFFER_SIZE < dictionaryWords.size() - 5) WORD_BUFFER_SIZE+=5;
                break;
			case 'z':
                if (WORD_BUFFER_SIZE >= 5) WORD_BUFFER_SIZE-=5;
                break;
			case 'h':
				if (activeword >= 1)
				{
					activeword--;
					fixori();
				}
				break;
			case 'l':
				if (activeword < textstrblocks.size() - 1)
				{
					activeword++;
					fixori();
				}
				break;
			case 'k':
				activeword -= jumplen;
				if (activeword < 0) activeword = 0;
				fixori();
				break;
			case 'j':
				activeword += jumplen;
				if (activeword >= textstrblocks.size()) activeword = textstrblocks.size() - 1;
				fixori();
				break;
			case 'e':
                bool alreadyDefined = false;
				// check if word is already defined
				for (int word = 0; word < dictionaryWords.size(); word++)
				{
					if (dictionaryWords.at(word).getword() == WordHelper::getword(textstrblocks.at(activeword)))
					{
                        alreadyDefined = true;
						break;
					}
				}
			

				string block = textstrblocks.at(activeword);
				string word = WordHelper::getword(block);
				string definition; 
				int grammar;
				
				if (alreadyDefined)
				{
					int familiarity;

					getyx (tt_window, cursor.y, cursor.x);
					mvwprintw (tt_window, cursor.y++ + 1, 1, "Modify which?");
					mvwprintw (tt_window, cursor.y++ + 1, 1, "1. Definition");
					mvwprintw (tt_window, cursor.y++ + 1, 1, "2. Grammar Use");
					mvwprintw (tt_window, cursor.y++ + 1, 1, "3. Familiarity");

					while ( (inpch = wgetch(tt_window)) < '1' || inpch > '3' );

					// get the member vars from the word
					Word temp = getactiveword (word);
					wclear (tt_window);

					// def
					if (inpch == '1')
					{
						dodef (definition);
                        temp.setdef (definition);
						writeentry (temp);
					}
					// grammar
					else if (inpch == '2')
					{
						dogram (grammar);
                        temp.setgrammar (grammar);
						writeentry (temp);
					}

					// fam
					else if (inpch == '3')
					{
						dofam (familiarity);
                        temp.setfam (familiarity);
						writeentry (temp);
					}
				}
				// not defined
				else
				{
					getyx (tt_window, cursor.y, cursor.x);
					mvwprintw (tt_window, cursor.y++ + 1, 1, "Add to dictionary?");
					mvwprintw (tt_window, cursor.y++ + 1, 1, "y\tn");
					wrefresh (tt_window);
					while ( (inpch = wgetch(tt_window)) != 'y' && inpch != 'n') { print_words(); }

					if (inpch == 'y')
					{
						werase (tt_window);
						dodef (definition);
						dogram (grammar);
						writeentry (Word(word, definition, grammar, 0));
					}

				}
				break;
		}
		print_words();
		refresh_tt ();


	}
    return 0;
}

int main_init () {
    textstrblocks = getblocks(bookname).tovector(); // initialize the string blocks from the text
	dictionaryWords = readDictWords().tovector(); // initialize the dictionary from the dictionary file

    // init screen
	// Create text window
	text_box = newwin((int)(LINES), (int)(COLS * TWPER), 0, 0); 	
	text_window = newwin((int)(LINES) - 2, (int)(COLS * TWPER) - 2, 1, 1);	

	print_words ();
	
	// Create tooltip window
	tt_box = newwin (LINES, (int)(COLS * (1 - TWPER)), 0, (int)(COLS * TWPER));
	tt_window = newwin (LINES - 2, (int)(COLS * (1 - TWPER)) - 2, 1, (int)(COLS * TWPER) + 1);

	refresh_tt ();
    return 0;
}

int choose_text () {
	vector<string> fileoptions = gettexts().tovector(); // get the texts from the appropriate directory
	if (fileoptions.size() > 0)	{
		echo ();
		while ( !vector_contains(fileoptions, bookname) ) {
			bookname = "";
			printw ("Enter the name of the file you want.\n");
			for (string file : fileoptions)	{
				printw ((" > " + file + "\n").c_str());
			}
			inpch = getch(); 
			while ( inpch != '\n' ) {
                // backspace; my machine detects as ascii 7 but it should be 8... 
				if ( (inpch == KEY_BACKSPACE || inpch == 7 || inpch == 8) && bookname.length() > 0) { 
					getyx (stdscr, cursor.y, cursor.x); // write over the original character that was there
					printw (" ");
					move (cursor.y, cursor.x);
					bookname.pop_back(); // actually affect the string
				}
				else // everything else
				{
					bookname.push_back (inpch);
				}
				inpch = getch();
			}
			refresh (); // can still see the text even when I don't call this for some reason; here for safety
			clear();

			if ( !vector_contains(fileoptions, bookname) ) {
                if (WordHelper::tolower(bookname) == "quit") {
                    noecho(); 
                    refresh();
                    endwin();
                    exit (1);
                }
                attron(COLOR_PAIR(REDFGPAIR));
				printw ( ("You entered \"" + bookname + "\". File does not exist. Please try again.\n").c_str() );
                attroff(COLOR_PAIR(REDFGPAIR));
			}
		}
		noecho ();
	}
	else
	{
		printw ("Error. No available texts. Ending execution now.\n");
		refresh();
		getch();
		endwin(); 
		return 1;
	}
    return 0;
}

int ncurses_init () {
	initscr();					// Start curses mode
	cbreak();					// Line Buffering Off
	keypad(stdscr, true);		// Functions keys, etc
	if(has_colors() == false)
	{ 
		endwin();
		cout << "Your terminal does not support color! D:" << endl;
        exit(1);
		return 1;
	}
	start_color();				// color
    init_pair(REDFGPAIR, COLOR_RED, COLOR_BLACK);
    init_pair(YELLOWFGPAIR, COLOR_YELLOW, COLOR_BLACK);
	noecho (); // hide input
    return 0;
}


// Does the vector have the thing?
template <typename T>
bool vector_contains (vector<T> vec, T thing)
{
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec.at(i) == thing) return true;
	}
	return false;
}

// Update the dictionary file with the new words, after exec is done
int writeDict()
{
	ofstream file;
	// clear the file
	file.open("../res/dictionary/dict.vact", ofstream::out | ofstream::trunc);
	file.close();

	file.open("../res/dictionary/dict.vact", ios::app); // append mode
	ofstream log;
	log.open("../log.txt", ios::app); // append mode
	if (file.is_open() && log.is_open())
	{
		for (int id = 0; id < dictionaryWords.size(); id++)
		{
			Word temp = dictionaryWords.at(id);

			time_t current_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
			log << "Wrote \"" << temp.getword() << "\" to the dictionary. " << ctime(&current_time) << endl;
			file << temp.getword() << ";" << temp.getdefinition() << ";" << temp.getigrammar() << ";" << temp.getifamiliarity() << endl;
		}
		file.close();
	}
	else return 1; 
	return 0;
}

// Get all the Words from the dictionary file on init
SLList<Word> readDictWords()
{
        int id = 0;
        string istring;
        SLList<Word> list;
        ifstream file("../res/dictionary/dict.vact");
        if (file.is_open())
        {
            while (getline(file, istring))
            {
                Word temp;
                unsigned long start = 0;
                unsigned long len = istring.find_first_of(';');
                temp.setword (istring.substr(start, len));

                start += len + 1;
                len = istring.substr(start).find_first_of(';');
                temp.setdef (istring.substr(start, len));

                start += len + 1;
                len = istring.substr(start).find_first_of(';');
                temp.setgrammar (atoi( istring.substr(start, len).c_str() ));

                start += len + 1;
                temp.setfam (atoi( istring.substr(start).c_str() ));

                list.orderedinsert (temp); // alphabetize our dictionary
                id++;
            }
            file.close();
        }
        return list;
}

// the basic tooltip window
int refresh_tt (void)
{

	// tt_window
	werase (tt_window);
    wattron(tt_box, COLOR_PAIR(YELLOWFGPAIR));
	box (tt_box, 0, 0); 
	char tt_title[] = "Tooltip";	
	mvwprintw (tt_box, 0, (int)(COLS * (1 - TWPER) / 2) - strlen(tt_title) / 2, tt_title); 
    wattroff(tt_box, COLOR_PAIR(YELLOWFGPAIR));
	wrefresh (tt_box); 

	string s, tt_word, e;
    WordHelper::splitblock(textstrblocks.at(activeword), s, tt_word, e); 
	
	tt_word = "Selected: \"" + tt_word + "\"";
	wmove (tt_window, 0, 1);
	wprintw (tt_window, tt_word.c_str());
	wmove (tt_window, 1, 1);
	
	int defined = 0;
	int tindex;
	for (int word = 0; word < dictionaryWords.size(); word++)
	{
		if (dictionaryWords.at(word).getword() == WordHelper::getword(textstrblocks.at(activeword)))
		{
			defined = 1;
			tindex = word;
			break;
		}
	}
	// getyx (tt_window, cursor.y, cursor.x);
	if (!defined)
	{
		wprintw (tt_window, "This word is undefined.");
	}
	else 
	{
		Word temp = dictionaryWords.at(tindex);
		wprintw (tt_window, ("Word: " + temp.getword() + "\n").c_str());
		wprintw (tt_window, ("Definition: " + temp.getdefinition() + "\n").c_str());
		wprintw (tt_window, ("Grammatical Use: " + temp.getgrammar() + "\n").c_str());
		wprintw (tt_window, ("Familiarity: " + temp.getfamiliarity() + "\n").c_str());
	}
	wprintw (tt_window, "\n e to modify.");
	
	wrefresh (tt_window);
	
	return 0;

}

// fix the orientation of the text window; we want WORD_BUFFER_SIZE words to be shown 
int fixori (void)
{
	if (activeword >= WORD_BUFFER_SIZE + first_word) first_word = activeword;
	else if (activeword < first_word)
	{
		first_word = activeword - WORD_BUFFER_SIZE;
		if (first_word < 0) first_word = 0;
	}
	return 0;
}

// is the word string defined in the dictionaryWords vector?
int isdefinedword (string word)
{
	for (int i = 0; i < dictionaryWords.size(); i++)
	{
		if (dictionaryWords.at(i).getword() == word) return 1;
	}
	return 0;
}

// output all of the words the we want to view into the text window
int print_words (void)
{

// PRINTING
	wmove (text_window, 0, 0); wrefresh (text_window); // go to top

	for (int i = first_word; i < first_word + WORD_BUFFER_SIZE; i++)
	{
		if (i > textstrblocks.size() - 1) break; // prevent OOB Error

		string block, word, start, end;
		block = textstrblocks.at(i);
        WordHelper::splitblock(block, start, word, end);

    // COLORIZER
        WordColorizer (block, text_window, (i == activeword), (isdefinedword(WordHelper::tolower(word))) ).process();

    // WRAPPING
        if ( i < textstrblocks.size() - 1 ) { // prevents OOB Error
            WordWrapper (block, textstrblocks.at(i + 1), text_window).process();
        }
	}

// DRAW WINDOW
    wattron(text_box, COLOR_PAIR(YELLOWFGPAIR));
	box (text_box, 0, 0);		// Draw a box border for the window
	char text_title[] = "Text";
	mvwprintw (text_box, 0, (int)(COLS * TWPER / 2) - strlen(text_title) / 2, text_title); // Title
    wattroff(text_box, COLOR_PAIR(YELLOWFGPAIR));
	wrefresh (text_box);

	wrefresh (text_window);

	return 0;
}

// handle assigning the definition of a word
void dodef (string & definition)
{
	mvwprintw (tt_window, 1, 1, "Definition: ");
	nocbreak ();	
	echo();
	int ch = wgetch(tt_window);
	
	while ( ch != '\n' )
	{
		definition.push_back( ch );
		ch = wgetch(tt_window);
	}
	cbreak();
	noecho();
}

// handle assigning the grammar of a word
void dogram (int & grammar)
{
	inpch = 0;
	wprintw (tt_window, " 1. Noun\n ");
	wprintw (tt_window, "2. Verb\n ");
	wprintw (tt_window, "3. Adverb\n ");
	wprintw (tt_window, "4. Article\n ");
	wprintw (tt_window, "5. Adjective\n ");
	wprintw (tt_window, "6. Other\n ");
	
	inpch = wgetch(tt_window) - '0'; // -'0' important; '0'-'9' != 0-9
	while ( !(inpch <= 6 && inpch >= 1) )
	{
		inpch = wgetch(tt_window) - '0';
	}
	grammar = inpch - 1; 
}


// handle assigning the familiarity of a word
void dofam (int & familiarity)
{
	inpch = 0;
	wprintw (tt_window, " 1. New\n ");
	wprintw (tt_window, " 2. Hard\n ");
	wprintw (tt_window, " 3. Medium\n ");
	wprintw (tt_window, " 4. Easy\n ");
	wprintw (tt_window, " 5. Mastered\n ");
	
	inpch = wgetch(tt_window) - '0'; // -'0' important; '0'-'9' != 0-9
	while ( !(inpch <= 5 && inpch >= 1) )
	{
		inpch = wgetch(tt_window) - '0';
	}
	familiarity = inpch - 1; 
}

// write Word to runtime entries and remove dupes if applicable
void writeentry (Word w)
{
	if (!vector_contains(dictionaryWords, w)) dictionaryWords.push_back(w);
	werase (tt_window);
	wprintw (tt_window, " Added the following entry:");
	wprintw (tt_window, ("\n Word: " + w.getword() + "\n").c_str());
	wprintw (tt_window, ("Definition: " + w.getdefinition() + "\n").c_str());
	wprintw (tt_window, ("Grammatical Use: " + w.getgrammar() + "\n").c_str());
	wprintw (tt_window, ("Familiarity: " + w.getfamiliarity() + "\n").c_str());


	wgetch(tt_window); // Wait
}

// get "activeword" as a Word from dictionaryWords
Word getactiveword (string word)
{
	for (Word w : dictionaryWords)
	{
		if (w == word)
		{
			return w;
		}
	}

	cerr << "Error! Internal disagreement about existence of dictionary entry." << endl;
	return Word();
}

int end_deinit() {
    // End exec
	clear ();
	printw ("Saving dictionary...\n");
	writeDict ();
	printw ("Dictionary saved.\n");
	printw ("Execution terminated. Press any key to continue...");
	refresh ();
	getch();
	endwin(); // exit curse mode
    return 0;
}
