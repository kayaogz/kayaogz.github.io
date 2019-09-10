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
      while (1) {
        omp_set_lock(&lock);
        if (!fin) { printf("Pas fini \n"); }
        else { break; }
        omp_unset_lock(&lock);
      }
    }
#pragma omp section
    {
      omp_set_lock(&lock);
      fin = 1;
      printf("Fini \n");
      omp_unset_lock(&lock);
    }
  }
  return 0;
}
