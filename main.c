#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <ctype.h>


//define
#define CTRL_KEY(k) ((k) & 0x1f)

//data
struct termios orig_termios;


//terminal
void die(const char *s){
    write(STDOUT_FILENO,"\x1b[2J",4); //clear screen
    write(STDOUT_FILENO,"\x1b[H",3); //move cursor to top-left corner

    perror(s);
    exit(1);
}


void disableRawMode(){
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&orig_termios)==-1) die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO,&orig_termios)==-1) die("tcgetattr");
    tcgetattr(STDIN_FILENO, &orig_termios);

    atexit(disableRawMode);

    struct termios raw=orig_termios;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN]=0;
    raw.c_cc[VTIME]=1;

    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw)==-1) die("tcsetattr");
}


char editorReadKey(){
    int nread;
    char c;
    while ((nread=read(STDIN_FILENO,&c,1))!=1){
        if (nread==-1 && errno!=EAGAIN) die("read");
    }
    return c;
}

//output
void editorRefreshScreen(){
    write(STDOUT_FILENO,"\x1b[2J",4); //clear screen
    write(STDOUT_FILENO,"\x1b[H",3); //move cursor to top-left corner
}


//input
void editorProcessKeypress(){
    char c=editorReadKey();
    
    switch (c) {
        case 'q':
            write(STDOUT_FILENO,"\x1b[2J",4); 
            write(STDOUT_FILENO,"\x1b[H",3);
            exit(0);
            break;
        case CTRL_KEY('q'):
            write(STDOUT_FILENO,"\x1b[2J",4); //clear screen
            write(STDOUT_FILENO,"\x1b[H",3); //move cursor to top-left corner
            exit(0);
            break;
    }
}


//init
int main(){
    enableRawMode();    

    while(1){
        editorRefreshScreen();
        editorProcessKeypress();

  
    }



    return 0;
}