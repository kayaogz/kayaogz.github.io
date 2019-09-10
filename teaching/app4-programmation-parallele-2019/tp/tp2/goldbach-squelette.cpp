#include <iostream>
#include <chrono>
#include <cmath>
#include "omp.h"

#define N 8000

/**
  * Calculer si x est un nombre premier ou pas
  */
bool estPremier(int x)
{
  if (x < 2) { return false; }
  for (int i = 2; i <= std::sqrt(x); i++) {
    if (x % i == 0) { return false; }
  }
  return true;
}

/**
  * Calculer le nombre de paires (i, j) tel que i + j = x et i et j sont des nombres premiers.
  */
int goldbach(int x)
{
  int compte = 0;
  if (x <= 2) { return 0; }
  for (int i = 2; i <= x / 2; i++) {
    if (estPremier(i) && estPremier(x - i)) { compte++; }
  }
  return compte;
}

int main()
{
  int numPairs[N];
  for (int i = 0; i < N; i++) { numPairs[i] = 0; }

  // Parallelisation avec l'ordonnancement static.
  auto start = std::chrono::high_resolution_clock::now();
  // A FAIRE ...
  std::chrono::duration<double> temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(static): " << temps.count() << "s\n";

  // Parallelisation avec l'ordonnancement static et taille de bloc 32.
  start = std::chrono::high_resolution_clock::now();
  // A FAIRE ...
  temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(static, 32): " << temps.count() << "s\n";

  // Parallelisatio avec l'ordonnancement dynamic
  start = std::chrono::high_resolution_clock::now();
  // A FAIRE ...
  temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(dynamic): " << temps.count() << "s\n";

  // Parallelisatio avec l'ordonnancement dynamic et taille de bloc 32
  start = std::chrono::high_resolution_clock::now();
  // A FAIRE ...
  temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(dynamic, 32): " << temps.count() << "s\n";

  // Parallelisatio avec l'ordonnancement guided
  start = std::chrono::high_resolution_clock::now();
  // A FAIRE ...
  temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(guided): " << temps.count() << "s\n";

  // Parallelisatio avec l'ordonnancement guided et taille de bloc 32
  start = std::chrono::high_resolution_clock::now();
  // A FAIRE ...
  temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(guided, 32): " << temps.count() << "s\n";

  return 0;
}
