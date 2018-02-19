#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char * argv[]) {

  //seed the time
  srand(time(NULL));

  int i;
  int keyLength;
  int randomKey;

  //check for proper arguments
  if (argc < 2) {
    fprintf(stderr, "%s", "Key length required!\n");
    exit(1);

  }
  //store keylength in a variable
  else {
    keyLength = atoi(argv[1]);
  }

  //generate random characters
  for (i = 0; i < keyLength; i++) {
    int random = rand() % 27;
    if (random < 26) {
      //convert to ASCII letter
      randomKey = 65 + random;
      printf("%c", randomKey);
    }
    else {
      printf(" ");
    }
  }
  //include a newline at the end
  printf("\n");

  return 0;
}
