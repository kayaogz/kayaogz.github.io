#include <immintrin.h>
#include <iostream>
#include <chrono>
#include <cstdlib>

#define NREPET 2048

int main(int argc, char **argv)
{
  int dim = std::atoi(argv[1]);
  float* tab0;
  float* tab1;
  
  // Allouer et initialiser deux tableaux de flottants de taille dim alignes par 32 octets
  tab0 =(float*) _mm_malloc (dim*sizeof(float), 32);
  tab1 =(float*) _mm_malloc (dim*sizeof(float), 32);
  for (int i = 0; i < dim; i++) { 
    tab0[i] = i;
    tab1[i] = 0;
  }
  
  // Copier tab0 dans tab1 de manière scalaire~(code non-vectorise).
  // On repete NREPET fois pour mieux mesurer le temps d'execution
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    for(int i=0;i<dim;i++){
      tab1[i]=tab0[i];
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diffSeq = end-start;
  std::cout << std::scientific << "Copier sans AVX: " << diffSeq.count() << "s" << std::endl;

  // Copier tab0 dans tab1 de maniere vectorisee avec AVX
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    for(int i=0;i<dim;i+=8){
    __m256 r0 = _mm256_load_ps( tab0+i );
    _mm256_store_ps( tab1+i, r0 );
    }
  }
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diffPar = end-start;
  std::cout << std::scientific << "Copier avec AVX: " << diffPar.count() << "s" << std::endl;

  // Afficher l'acceleration et l'efficacite
  double acceleration = diffSeq.count() / diffPar.count();
  double efficacite = acceleration / 8;
  std::cout << "Acceleration: " << acceleration << std::endl;
  std::cout << "Efficacite: " << efficacite << std::endl;

  return 0;
}
