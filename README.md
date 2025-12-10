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

- should be able to get the size of the terminal by simply calling ioctl() with the TIOCGWINSZ request

- TIOCGWINSZ = T:Terminal,I:Input,O:Output,C:control，Get WINdow SiZe

- ioctl(), TIOCGWINSZ, and struct winsize come from <sys/ioctl.h>

- if success,ioctl() place the nubmer of col wide and rows high the terminal is into the given winsize struct，othereise returns -1.

- if ioctl() failed ，have getWindowSize() report failure by returning -1，if it succeeded,pass the values back by settig the int references that passed to the function

- initEditor()’s job will be to initialize all the fields in the E struct

- ioctl() isn’t guaranteed to be able to request the window size on all systems,going to provide a fallback method of getting the window size

- the way is the position of cursor at the bottom-right of the screen,then ,use escape sequences that let us query the position of the cursor,it can tells us how many rows and cols there must be on the screen

- sending two escape sequences one after the other. The C command (Cursor Forward) moves the cursor to the right, and the B command (Cursor Down) moves the cursor down。argument says how much to move it right or down by

-  use a very large value, 999, which should ensure that the cursor reaches the right and bottom edges of the screen

- sscanf() comes from <stdio.h>

- we pass a pointer to the third character of buf to sscanf(), skipping the '\x1b' and '[' characters.

- passing a string of the form 24;80 to sscanf()  also passing it the string %d;%d which tells it to parse two integers separated by a ;, and put the values into the rows and cols variables.

- Append buffer:when every time we refresh the screen， make a whole bunch of small write()，will cause  annoying flicker effect

- to make sure the whole screen updates at once，replace `write()` with appends the string to a buffer，but C doesn't have dynamic strings,so create own dynamic string type call `abuf` to support append

- another possible cause flicker effect will take care of now，it's possible that cursor might be displayed in the middle of the screen split second when the terminal is drawing the screen，make sure dones't happen，hide cursor before refreshing the screen，and then show it again immediately after the refresh finish

-  use escape sequences to tell the terminal to hide and show the cursor。 The h and l commands

- instead of clearing the whole screen before each refresh，more optimal to clear each line as we redraw line，remove `<esc>[2j` (clear entire screen) escape sequence,and instead put a `<esc>[K`  sequence at the end of each line we draw

- K command erases part of the current line，it's like J command

- name editor and a version number，use the welcome buffer and snprintf() to interpolate our KILO_VERSION

- To center a string, you divide the screen width by 2 and substract half of the string length ， tells you how far from the left edge of the screen you should start printing the string，fill that space with space chracters,expect for first character,it should tilde


- want the user to be able to move the cursor around，first step is keep track  of the cursor's `X` and `Y` position in the global editor state

- E.cx is horizontal coordinate，E.cy is the vertical coordinate， initialize both of them to 0，start at the top-left of the screen

- allow user to move the cursor uinsg wasd keys

- replace the wasd keys with the arrow keys，saw before arrow keys sends multiple bytes are in the form of an escape sequence that starts with '\x1b', '[', followed by an 'A', 'B', 'C', or 'D' depending on which of the four arrow keys was pressed

- choose a representation for arrow keys that doesn’t conflict with wasd, in the editorKey enum

- give them a large integer value that is out of the range of a char

-  they don’t conflict with any ordinary keypresses

- A line viewer