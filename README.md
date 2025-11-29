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

- read() will wait indefinitely for input from the keyboard before it returns，if  want to do something like animate something on the screen while waiting for user input，can set a timeout

- VMIN and VTIME come from <termios.h>，c_cc field，which stands for “control characters”

- VMIN value sets the minimum number of bytes of input needed before read()

- The VTIME value sets the maximum amount of time to wait before read() returns

- Most C library functions that fail will set the global errno variable to indicate what the error was.

- errno and EAGAIN come from <errno.h>

- tcsetattr(), tcgetattr(), and read() all return -1 on failure

- run echo test | ./main result in the error from tcgetattr(), something like Inappropriate ioctl for device.

- Ctrl key combined with the alphabetic keys seemed to map to bytes 1–26

- Ctrl+q mapping to quit operation

- write() and STDOUT_FILENO come from <unistd.h>

- \x1b, which is the escape character or 27 in decimal.

-  '[' character. Escape sequences instruct the terminal to do various text formatting tasks, such as coloring text, moving the cursor around, and clearing parts of the screen

- using the J command to clear the screen. so just <esc>[J by itself would also clear the screen from the cursor to the end.

- <esc>[2J command left the cursor at the bottom of the screen.  reposition it at the top-left corner so that we’re ready to draw the editor interface from top to bottom

- The H command actually takes two arguments: the row number and the column number at which to position the cursor

- 