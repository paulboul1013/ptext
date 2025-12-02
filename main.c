#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <ctype.h>
#include <sys/ioctl.h>


//define
#define CTRL_KEY(k) ((k) & 0x1f)

//data
struct termios orig_termios;

struct editorConfig {
    int screenrows;
    int screencols;
    struct termios orig_termios;
};

struct editorConfig E;

//terminal
void die(const char *s){
    write(STDOUT_FILENO,"\x1b[2J",4); //clear screen
    write(STDOUT_FILENO,"\x1b[H",3); //move cursor to top-left corner

    perror(s);
    exit(1);
}


void disableRawMode(){
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&E.orig_termios)==-1) die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO,&E.orig_termios)==-1) die("tcgetattr");

    atexit(disableRawMode);

    struct termios raw=E.orig_termios;
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

    editorDrawRows();

    write(STDOUT_FILENO,"\x1b[H",3);
}

void editorDrawRows() {
    int y;
    for (y=0;y<E.screenrows;y++) {
        write(STDOUT_FILENO,"~\r\n",3);
    }
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

int getWindowSize(int *rows, int *cols){
    struct winsize ws;

    if (1 || ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws)==-1 || ws.ws_col==0){
        if (write(STDOUT_FILENO,"\x1b[999C\x1b[999B",12)!=12) return -1;
        editorReadKey();
        return -1;
    }
    else{
    
        *rows=ws.ws_row;
        *cols=ws.ws_col;
        return 0;
    }
   
}

void initEditor(){
    if (getWindowSize(&E.screenrows,&E.screencols)==-1) die("getWindowSize");
}


//init
int main(){
    enableRawMode();    
    initEditor();

    while(1){
        editorRefreshScreen();
        editorProcessKeypress();

  
    }



    return 0;
}