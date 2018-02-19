/* Author: Shanmukh Challa
 * Class: CS344
 * Program: smallsh.c
 * Description: Running a small shell by running processes and handling signals
 * References used:
 *      //https://www.tutorialspoint.com/c_standard_library/c_function_signal.htm
 *      //http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/exec.html
 *      //https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/strtok.htm
 */

#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>
#include<sys/stat.h>
#include<stdbool.h>

int catchZ = 0; //Flag to check if CTRL Z was clicked
int hasAmp = 0; //Flag to check if theres is an ampersand in the input
int status = 0; //Check the status
bool backgroundProcess; //Check if its a background process or not
int inputLocation, outputLocation, ampersandLocation; //Temporarily store the locations of the input, output, and ampersand

//Function prototypes for CTRL Z and CTRL C
void catchSIGINT(int signo);
void catchSIGTSTP(int signo);

//CTRL C handler
void catchSIGINT(int signo)
{
  	char * message = " Terminated by signal 2\n";
  	write(STDOUT_FILENO, message, 28);
}

//CTRL Z handler
void catchSIGTSTP(int signo) {
  if (!catchZ) {
    char *message = "\nEntering foreground-only mode (& is now ignored)\n:";
    write(STDOUT_FILENO, message, strlen(message));
    catchZ = 1;
    //backgroundProcess = false;
  }
  else {
    char *message = "\nExiting foreground-only mode\n:";
    write(STDOUT_FILENO, message, strlen(message));
    catchZ = 0;
  }
}

int main(int argc, char * argv[]) {
  //Functions used throughout the program
	char* temp = NULL;
	char* userInput[513];
	int argumentCount = 0;
	char* argumentList;
	char* inputFile = NULL;
	char* outputFile = NULL;
	bool inputFlag;
	bool outputFlag;
	int exitProgram = 0;
	int i, j;
  int pid;
	int cpid;
	int targetFD;
	int result;
	pid_t background[100];
	int counter = 0;

  //Structs and signal handlers
  struct sigaction SIGINT_action = {0};
	SIGINT_action.sa_handler = SIG_IGN;
	sigaction(SIGINT, &SIGINT_action, NULL);

	struct sigaction SIGINT_act = {0};
	SIGINT_act.sa_handler = catchSIGINT;
	sigfillset(&SIGINT_act.sa_mask);
	SIGINT_act.sa_flags = SA_RESTART;
	sigaction(SIGINT, &SIGINT_act, NULL);

  struct sigaction SIGTSTP_action = {0};
  SIGTSTP_action.sa_handler = catchSIGTSTP;
  sigfillset(&SIGTSTP_action.sa_mask);
  SIGTSTP_action.sa_flags = SA_RESTART;
  sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	do
  {
       //
			     for (i = 0; i < counter; i++)
			     {
				      pid_t checkPID = waitpid(background[i], &status, WNOHANG);
				      if (checkPID != 0)
				      {
		          printf("Background PID %d is done.", background[i]);
              printf(" Exit value: %d\n", WEXITSTATUS(status));
              fflush(stdout);
              for (j = i; j < counter; j++)
				      {
					             background[i] = background[j+i];

              }
              counter--;
				      }
			     }
           backgroundProcess = false;

           //Print the starting prompt
           printf(": ");
           fflush(stdout);

           ssize_t size = 0;

           //Get user input through the getline function
					 //Adapted from CS344 Canvas Section 3.3
					 while(1)
					 {
					 	int numCharsEntered = getline(&temp, &size, stdin); // Get a line from the user
					 	if (numCharsEntered == -1)
					 	{
					 		clearerr(stdin);

					 	}
					 	else {
					 		break; // Exit the loop - we've got input

					 	}
				 	 }
           argumentCount = 0; //Set number of arguments to 0
           argumentList = strtok(temp, "  \n"); //Break out the argument using strtok

           while (argumentList != NULL) {
                userInput[argumentCount] = strdup(argumentList); //Store the list of arguments in userInput
                argumentList = strtok(NULL, " \n");
                argumentCount++; //Increment argumentCount to get an accurate number of arguments
           }

					 for (i = 0; i < argumentCount; i++)
					 {
						 if (strcmp(userInput[i],"<") == 0) //Check to see if there is an "<" in the argument
						 {
							 inputFile = userInput[i+1]; //Store the argument next to < as the inputFile
							 inputLocation = i; //Set the location of the input file in in inputLocation so we can nullify it later
							 inputFlag = true; //Set the inputFlag to true
						 }
						 if (strcmp(userInput[i],">") == 0) //Check to see if there is an ">" in the argument
						 {
							 outputFile = userInput[i+1]; //Store the argument next to > as the outputFile
							 outputLocation = i; //Set the location of the input file in in outputLocation so we can nullify it later
							 outputFlag = true; //Set the outputFlag to true
						 }
						 if (strcmp(userInput[i],"&") == 0) //Check to see if there is an "&" in the argument
						 {
               hasAmp = 1; //Set hasAmp to true, used to nullify it later

               if (catchZ == 1) //If CTRL Z is used, this flag will set catchZ to true, meaning there is no background process
               {
							    backgroundProcess = false;
               }
               else
               {
                 backgroundProcess = true;
               }
               ampersandLocation = i; //Set the location of the ampersand in in outputLocation so we can nullify it later
							 //userInput[i] = NULL;
						 }
					 }

					 for (i = 0; i < argumentCount; i++)
					 {
						 if (strstr(userInput[i],"$$") != NULL) //Find the "$$" in the input so we can expand it with the appropriate PID
						 {
							 int dollarPID = getpid();
							 char findPID[30] = {0};
 					     char combined[100] = {0};
							 snprintf(findPID, 30, "%d", dollarPID); //Append dollarPID into findPID
               for (j = 0; j < strlen(userInput[i]) - 2; j++)
							 {
                    combined[j] = userInput[i][j];
							 }
               strncat(combined, findPID, 100);
							 userInput[i] = strdup(combined); //strdup is an alternative for strcpy; strcpy was causing a lot of problems with this

						 }
					 }

					 userInput[argumentCount] = NULL;

           if (userInput[0] == NULL || !(strncmp(userInput[0], "#", 1))) //If userInput has nothing or has a comment, then do nothing.
           {
              continue;

           }
           else if(strcmp(userInput[0], "exit") == 0) //if userInput has "exit", then exit the program
           {
							for (i = 0; j < counter; j++)
							{
								kill(background[i], SIGTERM); //Kill the processes

							}
            	exit(0); //exit in built function
		          exitProgram = 1; //exitProgram flag is set to true so you break out of the do-while loop

           }
           else if(strcmp(userInput[0], "status") == 0) //If status is in the input, then print out the status
           {
              printf("Exit value: %d\n", WEXITSTATUS(status));
              fflush(stdout);

      		 }
           else if(strcmp(userInput[0], "cd") == 0) //If cd is in the input, go to the appropriate directory
           {
              if (userInput[1] == NULL) { //If there is nothing after the cd, then go to the home directory
                  chdir(getenv("HOME"));
              }
              else //Else, go into the directory specified
              {
                chdir(userInput[1]);
              }
           }

      		 //All other commands
           else
           {
						 pid = fork();

             if(pid < 0) //Checking to see if there was an error while forking
             {
               perror("Error while forking!\n");
               fflush(stdout);
               exit(1);
             }
             else if (pid == 0) //If it is a child process
             {
               if (hasAmp) //Nullify the ampersand argument so execvp doesn't think its an argument
               {
                 userInput[ampersandLocation] = NULL;

               }
               hasAmp = 0;

               if (inputFlag)
               {
                 //Adapted from CS344 Canvas Lecture 3.4
                 targetFD = open(inputFile, O_RDONLY);
								 if (targetFD == -1) {
									 perror("input open()");
									 exit(1);
								 }
								 result = dup2(targetFD, 0);
								 if (result == -1) {
									 perror("input dup2()");
									 exit(2);
								 }

								 userInput[inputLocation] = NULL;
								 userInput[inputLocation+1] = NULL;
	 							//close the file stream
	 							fcntl(targetFD, F_SETFD, FD_CLOEXEC);
               }

							 if (outputFlag)
               {
                 //Adapted from CS344 Canvas Lecture 3.4
                 targetFD = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

								 if (targetFD == -1) {
									 perror("output open()");
									 exit(1);
								 }
								 int result = dup2(targetFD, 1);
								 if (result == -1) {
									 perror("output dup2()");
									 exit(2);
								 }

                 userInput[outputLocation] = NULL;
                 userInput[outputLocation+1] = NULL;

                 fcntl(targetFD, F_SETFD, FD_CLOEXEC);
               }

               if (backgroundProcess == true && inputFlag == true) //Its is a backgroundProcess and its an input file, send the file into "/dev/null"
               {
                 //Adapted from CS344 Canvas Lecture 3.4

                targetFD = open("/dev/null", O_RDONLY);
                if (targetFD == -1) {
                  perror("input open()");
                  exit(1);
                }
                result = dup2(targetFD, 0);
                if (result == -1) {
                  perror("input dup2()");
                  exit(2);
                }

                userInput[inputLocation] = NULL; //Nullify the "<" symbol
                userInput[inputLocation+1] = NULL; //Nullify the name of the file so we don't think its an argument
               //close the file stream
               fcntl(targetFD, F_SETFD, FD_CLOEXEC);
               }

              if (backgroundProcess == true && outputFlag == true) //Its is a backgroundProcess and its an output file, send the file into "/dev/null"
               {
                 //Adapted from CS344 Canvas Lecture 3.4

                 targetFD = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0644);

                if (targetFD == -1) {
                  perror("output open()");
                  exit(1);
                }
                int result = dup2(targetFD, 1);
                if (result == -1) {
                  perror("output dup2()");
                  exit(2);
                }
               userInput[outputLocation] = NULL; //Nullify the ">" symbol
               userInput[outputLocation+1] = NULL; //Nullify the name of the file so we don't think its an argument
              //close the file stream
              fcntl(targetFD, F_SETFD, FD_CLOEXEC);
             }

			          if (execvp(userInput[0], userInput)) { //execute
									//Command not recognized error, exit
			            fprintf(stderr, ": %s command is not recognized!\n", userInput[0]);
			            fflush(stdout);
			            exit(1);

							 }

               exit(-1); //Exit the child process
             }
						 else //Its a parent process
             {
							 inputFlag = false; //Reset the inputFlag
							 outputFlag = false; //Reset the outputFlag

							 if(backgroundProcess == false) //If its not a backgroundProcess, then wait
							 {

								 waitpid(pid, &status, 0);

							 }
							 else if (backgroundProcess == true) //If its a backgroundProces
							 {
								 printf("Background PID is: %d\n", pid); //Print out the background PID of the process
								 background[counter] = pid; //Store the PID in an array of type pid_t
								 counter++;
								 fflush(stdout);
								 pid_t isDone = waitpid(pid, &status, WNOHANG); //Checkif process is done
							 }

             }
             backgroundProcess = false; //Reset background process to false
           }

   } while (exitProgram != 1); //While exitProgram is not equal to 1, keep looping through the above program

    return 0;
}
