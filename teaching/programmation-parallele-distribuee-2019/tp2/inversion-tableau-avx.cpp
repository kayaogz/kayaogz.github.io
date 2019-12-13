/**
  * Copie d'un tableau dans un autre avec les intrinseques AVX.
  * A compiler avec les drapeaux -O2 -mavx2.
  */

#include <immintrin.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdlib>

#define NREPET 1024

void afficherUsage()
{ 
  printf("Usage: ./inversion-tableau-avx [taille-du-tableau]\n");
}

int main(int argc, char **argv)
{
  if (argc < 2) { 
    afficherUsage();
    return 1;
  }
  int dim = std::atoi(argv[1]);
  
  // Allouer et initialiser un tableau de taille dim aligne par 32 octets
  float *tab = (float *)_mm_malloc(dim * sizeof(float), 32);
  for (int i = 0; i < dim; i++) { tab[i] = i; }
  
  // Inverser le tableau en place~(c'est a dire sans utiliser un deuxieme tableau auxiliaire)
  // On repete NREPET fois pour mieux mesurer le temps d'execution
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    for (int j = 0; j < dim / 2; j++) { 
      float temp = tab[j];
      tab[j] = tab[dim - j];
      tab[dim - j] = temp;
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diffSeq = end-start;
  std::cout << std::scientific << "Inversion sans AVX: " << diffSeq.count() / NREPET << "s" << std::endl;

  // Inverser le tableau en place avec AVX~(c'est a dire sans utiliser un deuxieme tableau auxiliaire)
  // On repete NREPET fois pour mieux mesurer le temps d'execution
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    // ...
  }
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diffPar = end-start;
  std::cout << std::scientific << "Inversion avec AVX: " << diffPar.count() / NREPET << "s" << std::endl;

  return 0;
}
