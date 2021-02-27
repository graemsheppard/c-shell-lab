Description
myshell is a simple C shell intended to learn about low-level process management and io. It supports a variety of system management functionality taken from either command-line or shell script files. It is written in C and intended for use on Unix distributions.

Startup and shutdown
To start the shell, navigate to the correct directory on your terminal and run the shell using "./myshell". This will transfer control of the terminal to myshell. To exit the program, either enter the command 'quit', or press control+C.

Supported commands
cd
Changes current working directory to the directory following the command. A relative or absolute path to a folder must be provided.
clr
Clears the screen visually, scrolling down to give the appearance of a freshly opened terminal.
dir
Displays all files and directories in the current working directory.
echo
Prints the text following the command to selected output (stdout by default).
environ
Prints a list of the current environment variables.
help
Displays help information.
ls
Lists all files in the current working directory.
quit
Exits the shell.
sleep
Pauses execution of the shell until the user inputs an enter again.

All other input is treated as a program to be run. The shell will look in the current working directory for an executable with a name matching the input, and will execute it if possible.

Lexical structure
The space character acts as a delimiter between commands and arguments. Commands that take arguments should be given of the form `echo test` where `echo` is the command and `test` is the argument the command will take. The shell also supports some special control characters. `>` will redirect output. Instead of outputting to the terminal, it will output to the selected file. For example, `echo test > test.txt` will output "test" to the file `text.txt`. The file will be created if it does not already exist, and truncated if it does. `<` will redirect input. Instead of taking input from the command-line, it will take input from the file following the `<` character. `>>` will redirect output to the selected file per the above example, however it will append to the file instead of truncating it. `&` at the end of a command will cause the command, or program called by the command, to execute in the background. This will leave the user in control of the shell while the program executes, allowing the user to input additional commands, instead of pausing the shell until the command or program finishes executing.
