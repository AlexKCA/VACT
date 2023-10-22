# Vocab Aquisition & Comprehension Tool - CIS 278

## Dependencies
NOTE: This project depends on ncurses.h which is a Unix library. It has only been tested on Linux, but MacOS and other Unix-based OSs should work. 

If you are using Windows, [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) should allow you to run this project. However, I haven't specifically tested it. Alternatively, you could use a full-on virtual machine. However, WSL is more convienient.

To build this project, you will need g++ and ncurses.h
On Debian-based systems (Debian, Ubuntu, Linux Mint, etc. [WSL2 is usually Debian-based by default]) you can run the following
```bash
sudo apt install g++ libncurses-dev
```

## Building
To build the project, cd into VACT/src and run the following
```
g++ -o ../bin/main main.cpp -lncurses
```
## Execution
To execute the project cd into VACT/bin and run the following
```
./main
```

## Hotkeys
While viewing document:
```
q - quit
h - left
j - down
k - up
l - right
```

Menus:
```
y - yes
n - no
```
## Workflow
1. Go to ```VACT/res/files/``` and add a ```.txt``` file with the text you want to process.
2. ```sh create.sh``` to build and run the program. Now select the file you made in the program.
3. Go through your file. Add entries to your dictionary and set your familiarity with particular words.
