/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2021, Nicholas Lang, Graem Sheppard
 * All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include "utility.h"
#include "internals.h"
#include "myshell.h"
#include "wait.h"
// Put macros or constants here using #define

// Define some colour values
#define COL_SEP "\x1B[37;1m" // Separator colour (in prompt)
#define COL_YAY "\x1B[32;1m" // Success colour
#define COL_ERR "\x1B[31;1m" // Error colour
#define COL_NON "\x1B[0m"    // Reset all attributes

// Put global environment variables here

// Define functions declared in myshell.h here
void split_string(char *buffer, char ***tokens_ptr, int *tokens_size)
{
    // Initialize empty token and counter
    char *token = NULL;
    char **tokens = NULL;
    int count = 0;

    // First pass: count spaces
    for (int i = 0; buffer[i] != '\0'; i++) {
        char c = buffer[i];
        if (c == ' ') count++;
    }

    // Allocate memory for tokens
    *tokens_ptr = calloc(count + 2, sizeof(char*));
    tokens = *tokens_ptr;

    // Second pass: create tokens
    token = strtok(buffer, " ");
    for (int i=0; token != NULL; i++)
    {
        tokens[i] = token;
        token = strtok(NULL, " \n");
    }

    // Verify that tokenization works
    for (int i=0; i < count; i++)
    {
        // printf("%s, ", tokens[i]);
    }

    *tokens_size = ++count;
}

int main(int argc, char *argv[])
{
    // Input buffer and and commands
    char buffer[BUFFER_LEN] = { 0 };
    char command[BUFFER_LEN] = { 0 };
    char arg[BUFFER_LEN] = { 0 };

    int last_exit = 0;
    char* path = getcwd(0, 0);
    char* shell_path = malloc(sizeof(char) * ((int)strlen(path) + 8));
    strcpy(shell_path, path);
    strcat(shell_path, "/myshell");
    setenv("shell", shell_path, 1);
    free(shell_path);
    // Add internal commands
    internals_add_all();
    FILE * input;
    char prompt;
    FILE *fp = NULL;
    int save_stdout;
    int save_stdin;
    int file_in;
    int file_out;
    // Parse the commands provided using argc and argv
    if (argc == 1)
    {
        input = stdin;
        prompt = 1;
    }
    else
    {
        prompt = 0;
        input = fopen(argv[1], "r");
        if (input == 0)
        {
            printf("Could not open file specified! :(\n");
            exit(1);
        }
    }

    // Perform an infinite loop getting command input from users
    while (1+2==3) // universe check
    {
        if (prompt)
        {
            // Print indicator for end of command output
            /*
                Rationale:
                * Most shells start the prompt here, but this can cause
                  a very confusing / asthetically unpleasing result when
                  a command or program does not output a new line like
                  it should.
                * Something still needs to output at the immediate end
                  of the program output so a developer can make sure
                  their program's output is correct.
            */
            printf("-\x1B[0m-\n");

            // Print exit status of last command
            if (last_exit == 0)
            {
                // Print OK in green;bright
                printf(COL_YAY "OK" COL_NON);
            }
            else
            {
                // Print error code in red;bright
                printf(COL_ERR "%02x" COL_NON, last_exit);
            }

            char *mode = COL_SEP "$" COL_NON;

            // Print the rest of the prompt
            printf(COL_SEP ":" COL_NON "%s%s ", util_print_cwd(), mode);
        }

        // BASE CASE
        if (fgets(buffer, BUFFER_LEN, input) == NULL)
            break;

        char **tokens = NULL;
        int count = 0;

        // Get user input again if user didn't type anything
        if (buffer[0]=='\n') continue;

        // Remove newline from user input
        char *newLineLoc = strchr( buffer, '\n' );
        *newLineLoc = 0;

        // Split user input into tokens
        split_string(buffer, &tokens, &count);

        // Update the command buffer
        strncpy(command, tokens[0], BUFFER_LEN-1);

        char command_handled = 0;

        // Loop through internal commands
        command_t **command_table_ptr = internal_commands;
        while (*command_table_ptr != NULL) {
            command_t cmp_command = **command_table_ptr;
            if (strcmp(cmp_command.name, command) == 0)
            {
                // Run the internal command
                /**********************************************/
		            /*WRITE YOUR CODE HERE*/
		for (int i = 0; i < count; i++) {
			if (strcmp(tokens[i], "<") == 0) {
				// Next token is "<", so stdin needs to be redirected to that file.
				// Check first if there's another element in the array, and if there is, handle it.
				if (i+1 < count) 
				{
					save_stdin = dup(fileno(stdin));
					fp = freopen(tokens[i+1], "r", stdin);
					file_in = 1;
				}
			} else if (strcmp(tokens[i], ">") == 0) {
				// Next token is ">", so stdin needs to be redirected to that file.
				// Check first if there's another element in the array, and if there is, handl it.
				if (i+1 < count) {
					save_stdout = dup(fileno(stdout));
					fp = freopen(tokens[i+1], "w", stdout);
					file_out = 1;
				}
			} else if (strcmp(tokens[i], ">>") == 0) {
				// Token is ">>". Handle as above.
				if (i+1 < count) {
					save_stdout = dup(fileno(stdout));
					fp = freopen(tokens[i+1], "a", stdout);
					file_out = 1;
				}
			}
		}

                int exit_code = cmp_command.cmd_ptr(count, tokens);
			
		if (file_in == 1) {
			fflush(stdin);
			fclose(stdin);
			stdin = fdopen(save_stdin, "r");
			file_in = 0;
		} else if (file_out == 1) {
			fflush(stdout);
			fclose(stdout);
			stdout = fdopen(save_stdout, "w");
			file_out = 0;
		}


		            /**********************************************/
		
                last_exit = exit_code;

                command_handled = 1;

                // The command is found, break from the loop
                if (exit_code == EXIT_SUCCESS) break;
            }
            command_table_ptr++;
        };


        // Treat as program invocation
        if (command_handled == 0) {
          pid_t pid = fork();
          if (pid == 0) {
            // sets the parent to the shell variable
            setenv("parent", getenv("shell"), 1);
            // Execute the program and return 0 if successful
            exit(execve(command, tokens, environ)); // pass in the current environ
          } else {
            int status;
            // Wait for the child (pid) to terminate
	    if (strcmp(tokens[count - 1], "&")) {
		// Does not end with &
            	waitpid(pid, &status, 0);
	    } else {
		// Ends with &, so use WNOHANG to not stop parent execution
		waitpid(pid, &status, WNOHANG);
	    }

            // Process exited normally
            if (WIFEXITED(status)) {
              // Get its exit code
              int exit_status = WEXITSTATUS(status);
              if (exit_status == 0) {
                command_handled = 1;
              }
            }
          }
        }

        // Unsupported command
        if (command_handled == 0)
        {
            fputs("Unsupported command, use help to display the manual\n", stderr);
            last_exit = 127;
        }
    }
    exit(1);
}
