#include <stdio.h>
#include <unistd.h>



int main(){

    char c;
    //read one character from stdin
    //exit is press ctrl+d or ctrl+c
    while(read(STDIN_FILENO,&c,1)==1);



    return 0;
}