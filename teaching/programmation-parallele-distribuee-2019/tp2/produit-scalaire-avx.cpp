/**
  * Produit scalaire de deux tableaux avec les intrinseques AVX et FMA.
  * A compiler avec les drapeaux -O2, -mavx, et -mfma
  */

#include <immintrin.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdlib>

#define NREPET 1024

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cout << "Utilisation: \n  " << argv[0] << " [taille-de-tableau]\n";
    return 1;
  }
  int dim = std::atoi(argv[1]);
  if (dim % 8) {
    std::cout << "La taille de tableau doit etre un multiple de 8.\n";
    return 1;
  }

  float* tab0;
  float* tab1;
  
  // Allouer et initialiser deux tableaux de flottants de taille dim alignes par 32 octets
  // ...
  
  // Produit scalaire non-vectorise. On repete NREPET fois pour mieux mesurer le temps d'execution
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    // ...
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> tempsSeq = end-start;
  std::cout << std::scientific << "Produit scalaire sans AVX: " << tempsSeq.count() / NREPET << "s" << std::endl;

  // Produit scalaire vectorisee AVX
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    // ...
  }
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> tempsAVX = end-start;
  std::cout << std::scientific << "Produit scalaire avec AVX: " << tempsAVX.count() / NREPET << "s" << std::endl;

  // Produit scalaire vectorisee AVX FMA.
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    // ...
  }
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> tempsParAVXFMA = end-start;
  std::cout << std::scientific << "Produit scalaire avec AVX FMA: " << tempsParAVXFMA.count() / NREPET << "s" << std::endl;

  // Produit scalaire vectorisee AVX FMA deroulement.
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    // ...
  }
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> tempsParAVXFMADeroule = end-start;
  std::cout << std::scientific << "Produit scalaire avec AVX FMA deroulement: " << tempsParAVXFMADeroule.count() / NREPET << "s" << std::endl;

  // Afficher l'acceleration et l'efficacite
  // ...

  return 0;
}
