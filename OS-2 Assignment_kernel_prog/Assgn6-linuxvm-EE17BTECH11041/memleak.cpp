#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

int main(int argc, char* argv[]) {
  const int KB = 1024; /* bytes */
  const int MB = 1024 * KB; /* bytes */

  long size = 0;
  void *p = NULL;

  int ctr;
  int j;
  int times = 100000;
  long sum = 0;
  for(ctr=0;ctr<times;ctr++) {
    sleep(1);
    size = 100 * MB;
    p = calloc(1, size);
    if(p != NULL) {
      printf("SUCCESS.\n");
      sum += size;
      for(j=0; j<size; j++) {
	((char*)p)[j] = 1;
      }
	printf("Iteration-%d Total allocated: %ld MB\n",ctr+1,sum/MB);
    }
	else {
      printf("FAIL.\n");
      break;
    }
  }
}
