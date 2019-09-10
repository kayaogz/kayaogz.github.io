#include <cstdio>
#include "omp.h"

int main() {
  int fin = 0;
  omp_lock_t lock;
  omp_init_lock(&lock);

#pragma omp parallel sections
  {
#pragma omp section
    {
      while (!fin) {
        printf("Pas fini \n");
      }
    }
#pragma omp section
    {
      fin = 1;
      printf("Fini \n");
    }
  }
  return 0;
}
