#include <chrono>
#include <iostream>
#include "omp.h"

inline double f(double x)
{
  return (4 / (1 + x * x));
}

int main()
{
  int i;
  const int N = 100000000;
  double pi = 0.0;
  double s = 1 / (double)N;

  // En premiere partie, calculer le pi avec omp for et reduction
  auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for reduction(+:pi)
  for (i = 0; i < N; i++) {
    double si = i * 1 / N;
    pi = pi + s * (f(i * s) + f((i + 1) * s)) / 2;
  }  
  std::cout << "pi = " << pi << std::endl;
  std::chrono::duration<double> tempsOmpFor = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps parallel omp for: " << tempsOmpFor.count() << "s\n";

  // En deuxieme partie, calculer le pi avec boucle parallele faite a la main
  pi = 0.0;
  start = std::chrono::high_resolution_clock::now();
#pragma omp parallel 
  {
    int thId = omp_get_thread_num();
    int numTh = omp_get_num_threads();
    int elemParTh = N / numTh;
    int thDebut = thId * elemParTh;
    int thFin;
    if (thId == numTh - 1) { thFin = N; } // Si N n'est pas divisible par numTh, le dernier thread parcour jusqu'au bout
    else { thFin = thDebut + elemParTh; }
    double piLocal = 0.0;
    for (int i = thDebut; i < thFin; i++) {
      double si = i * 1 / N;
      piLocal = piLocal + s * (f(i * s) + f((i + 1) * s)) / 2;
    }
#pragma omp critical
    pi += piLocal;
  }
  std::cout << "pi = " << pi << std::endl;
  std::chrono::duration<double> tempsForMain = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps parallel for a la main: " << tempsForMain.count() << "s\n";

  return 0;
}
