#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
void error(const char *msg) {
  perror(msg);
  exit(0);
} // Error function used for reporting issues

int main(int argc, char *argv[]){
  int socketFD, portNumber, charsWritten, charsRead;
  int i;
  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;

  char plainText[70000];
  char keyText[70000];
  char cipher[70000];
  FILE * plainFile = NULL;
  FILE * keyFile = NULL;

  if (argc < 3) {
    fprintf(stderr,"USAGE: %s hostname port\n", argv[0]);
    exit(0);
  } // Check usage & args

  //open the plaintext file
  if (plainFile = fopen(argv[1], "r")) {
    //store it in an array
    fgets(plainText, sizeof(plainText), plainFile);
    fclose(plainFile);

  } else {
    fprintf(stderr, "ERROR on opening plain text file!\n");

  }

  //open key file
  if (keyFile = fopen(argv[2], "r")) {
    //store it in an array
    fgets(keyText, sizeof(keyText), keyFile);
    fclose(keyFile);

  } else {
    fprintf(stderr, "ERROR on opening plain text file!\n");

  }

  //error check to make sure the key file is a proper length
  if(strlen(plainText) > strlen(keyText)) {
    fprintf(stderr, "ERROR key text is too short!\n");

  }

  //error check to make sure there are only capital letters and spaces
  for (i = 0; i < strlen(plainText)-1; i++){
    if (!(isupper(plainText[i]) || isspace(plainText[i])))
    {
      fprintf(stderr, "ERROR invalid text!\n");
      exit(1);
    }
  }

  //error check to make sure there are only capital letters and spaces
  for (i = 0; i < strlen(keyText)-1; i++){
    if (!(isupper(keyText[i]) || isspace(keyText[i])))
    {
      fprintf(stderr, "ERROR invalid keys!\n");
      exit(1);
    }
  }

  // Set up the server address struct
  memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
  portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
  serverAddress.sin_family = AF_INET; // Create a network-capable socket
  serverAddress.sin_port = htons(portNumber); // Store the port number
  serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address

  if (serverHostInfo == NULL) {
    fprintf(stderr, "CLIENT: ERROR, no such host\n");
    exit(0);
  }

  memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

  // Set up the socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
  if (socketFD < 0) {
    error("CLIENT: ERROR opening socket");// Connect to server

  }

  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {// Connect socket to address
    error("CLIENT: ERROR connecting");// Get input message from user

  }

  //clear the arrays before storing information
  plainText[strcspn(plainText, "\n")] = '\0';
  keyText[strcspn(keyText, "\n")] = '\0';
  charsWritten = send(socketFD, plainText, sizeof(plainText), 0); // Write to the server

  if (charsWritten < 0) {
    error("CLIENT: ERROR writing to socket!\n");

  }
  if (charsWritten < sizeof(plainText)) {
    printf("CLIENT: WARNING: Not all data written to socket!\n");// Make sure all data is sent

  }

  charsWritten = send(socketFD, keyText, sizeof(keyText), 0);
  //If there is a problem with sending the key text.
  	if(charsWritten < 0)
  	{
  	error("CLIENT: ERROR writing to socket!\n");
  	}
  //Make sure all of the data gets written to socket
  	if(charsWritten < sizeof(keyText))
  	{
  	printf("CLIENT: WARNING not all data written to socket!\n");
  	}

  	//Receive the response from the server
    memset(cipher, '\0', sizeof(cipher));
  	charsRead = recv(socketFD, cipher, sizeof(cipher) - 1, MSG_WAITALL);

    printf("%s", cipher);

    //Clear out my arrays for reuse
  	memset(plainText, '\0', sizeof(plainText));
  	memset(keyText, '\0', sizeof(keyText));

  	//If there was an error reading from the server.
  	if (charsRead < 0)
  	{
  	error("CLIENT: ERROR reading from socket");
  	}
  	//Send the encrypted text to stdout.

  	//Close the socket.
  	close(socketFD);

  return 0;
}
