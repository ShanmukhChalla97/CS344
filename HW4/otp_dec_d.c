#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {  // Error function used for reporting issues
  perror(msg);
  exit(1);

}

int main(int argc, char *argv[]){
  int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
  int status;
  int i;
  socklen_t sizeOfClientInfo;
	char plainText[70000];
  char keyText[70000];
  char cipher[70000];
  struct sockaddr_in serverAddress, clientAddress;

  if (argc < 2) { // Check usage & args
    fprintf(stderr,"USAGE: %s port\n", argv[0]);
    exit(1);
  }

  // Set up the address struct for this process (the server)
  memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
  portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
  serverAddress.sin_family = AF_INET; // Create a network-capable socket
  serverAddress.sin_port = htons(portNumber); // Store the port number
  serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

  // Set up the socket
  listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
  if (listenSocketFD < 0) {
    fprintf(stderr, "ERROR opening socket\n");
  } // Enable the socket to begin listening

  if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) { // Connect socket to port
    error("ERROR on binding\n");

  }

  listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

  //infinite loop to continue accepting connections
  while (1) {
    // Accept a connection, blocking if one is not available until one connects
    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
    if (establishedConnectionFD < 0) {
      error("ERROR on accept");

    }// Get the message from the client and display it

    //fork a child to do the decryption
    pid_t pid = fork();

    if (pid < 0) {
      fprintf(stderr, "Hull breach!\n");
      exit(2);
    }
    //in the child
    else if (pid == 0) {
      //clear the arrays before stroing information
      memset(plainText, '\0', sizeof(plainText));
      memset(keyText, '\0', sizeof(keyText));

      //receive the file from the client
      charsRead = recv(establishedConnectionFD, plainText, sizeof(plainText),MSG_WAITALL);
      if (charsRead < 0) {
        fprintf(stderr, "ERROR reading text from socket!\n");

      }
      //wait until the entire message is read
      charsRead = recv(establishedConnectionFD, keyText, sizeof(keyText),MSG_WAITALL);
      if (charsRead < 0) {
        fprintf(stderr, "ERROR reading text from socket!\n");

      }

      memset(cipher, '\0', sizeof(cipher));
      //decrypt the text
      for (i = 0; i < strlen(plainText); i++)
      {
        int test1;
        int test2;
        char test4;

        //for a space
        if (plainText[i] == ' ') {
          test1 = 26;
        //for a letter
        } else {
          test1 = plainText[i] - 65;

        }

        //for a space
        if (keyText[i] == ' ') {
          test2 = 26;
        //for a letter
        } else {
          test2 = keyText[i] - 65;

        }
        //do the math to get the decrypted value
        char test3 = test1 - test2 + 27;
        if (test3 < 0)
        {
            test4 = test3 % 27;
        }
        else {
           test4 = (test3) % 27;
        }
        //convert back to ASCII letter
        cipher[i] = test4==26?32:test4+65;
      }
      strcat(cipher, "\n");

      //send the text back to the client
      charsRead = send(establishedConnectionFD, cipher, sizeof(cipher), 0);
      close(establishedConnectionFD); // Close the existing socket which is connected to the client

    }
    //in the parent
    else {
      pid_t waitPid = waitpid(pid, &status, 0);
      break;

    }
  }

    close(listenSocketFD); // Close the listening socket

  return 0;
}
