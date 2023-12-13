#ifndef WORDWRAPPER_HPP
#define WORDWRAPPER_HPP
#include "wordprocessor.hpp"
#include <ncurses.h>
#include <string>

class WordWrapper : public WordProcessor {
private:
    std::string nextblock;
public:
// CONSTRUCTOR
    WordWrapper (std::string wordblock, std::string nextblock, WINDOW* writingwindow) : WordProcessor (wordblock, writingwindow) {
        this->nextblock = nextblock;
    };
// MAIN PROCESS
    // do wrapping
    // if adding the next word will go past COL XXX, then also do a newline
    // i.e. if the length of space + the next block + the current x is >= COL XXX then newline
    void process () override {
        int temp, currentcol, ncols;
		getyx (writingwindow, temp, currentcol);
        getmaxyx(writingwindow, temp, ncols);
        if ( ( nextblock.length() + currentcol) >= ncols ) wprintw (writingwindow, "\n");
    }
};
#endif

