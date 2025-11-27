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

- the atexit() from <stdlib.h> ，can point the address of function ，when program exit，the function will auto exeucte

- TCSAFLUSH :it discards any unread input before applying the changes to the terminal

- ICANON:allow us to turn off canonical mode

- iscntrl() from <ctype.h> :  tests whether a character is a control character

- Control characters are nonprintable characters that we don’t want to print to the screen

- ctrl-c send SIGINT signal to current process cause it to terminate

- ctrl-z send SIGTSTP signal to current process cause it to suspend

- default ctrl-s and ctrl-q are used for software flow control

- Ctrl-S stops data from being transmitted to the terminal until you press

- Ctrl-Q. This originates in the days when you might want to pause the transmission of data to let a device like a printer catch up.

- IXON comes from <termios.h>:I stands for “input flag”  and XON comes from the names of the two control characters that Ctrl-S and Ctrl-Q

- when you type Ctrl-V, the terminal waits for you to type another character and then sends that character literally

- Ctrl-M is weird: it’s being read as 10, when we expect it to be read as 13, since it is the 13th letter of the alphabet, and Ctrl-J already produces a 10. What else produces 10? The Enter key does.

- ICRNL comes from <termios.h>. The I stands for “input flag”, CR stands for “carriage return”, and NL stands for “new line”

- The terminal requires both of these characters in order to start a new line of text.

-  The carriage return moves the cursor back to the beginning of the current line, and the newline moves the cursor down a line, scrolling the screen if necessary

- turn off all output processing features by turning off the OPOST flag. In practice, the "\n" to "\r\n"

- turn off BRKINT, INPCK, ISTRIP, and CS8 won’t have any observable effect for you。But at one time or another, switching them off was considered (by someone) to be part of enabling “raw mode”