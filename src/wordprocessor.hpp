#ifndef WORDPROCESSOR_HPP
#define WORDPROCESSOR_HPP
#include <ncurses.h>
#include <string>

class WordProcessor {
protected:
    std::string wordblock;
    WINDOW * writingwindow; 
public:
    WordProcessor (std::string wordblock, WINDOW* writingwindow) {
        this->writingwindow = writingwindow;
        this->wordblock = wordblock;
    }
    virtual void process() = 0;
};

#endif


