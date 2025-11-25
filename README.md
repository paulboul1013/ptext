# ptext

# note

- read() and STDIN_FILENO from <unistd.h> 
- read() returns the number of bytes that it read
- default your terminal starts in canonical mode, also called cooked mode
- If want to process each keypress as it comes in，is raw mode. 
- Raw mode is achieved by turning off a great many flags in the terminal
- tcgetattr() to read the current attributes into a struct
-  modifying the struct by hand, and passing the modified struct to tcsetattr() to write the new terminal attributes back out

- 'ECHO' is you type will show termianl,it's useful in canonical mode，but hvae trouble in th raw mode，need to turn it off，it's like when you type password in the sudo instruction

- when enableRawMode my terminal really can see what I typing，just need press 'Ctrl+c' to start a fresh line and type 'reset' and press 'Enter'




# processing 

