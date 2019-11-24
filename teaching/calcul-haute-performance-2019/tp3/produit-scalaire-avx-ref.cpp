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

float produitScalaire(float *A, float *B, int taille)
{
  float res = 0.0f;
  for (size_t i = 0; i < taille; i++) { 
    res += A[i] * B[i];
  }
  return res;
}

float produitScalaireAVX(float *A, float *B, int taille)
{
  __m256 res = _mm256_set1_ps(0.0f);
  for (size_t i = 0; i < taille; i += 8) {
    __m256 la = _mm256_load_ps(A + i);
    __m256 lb = _mm256_load_ps(B + i);
    __m256 lab = _mm256_mul_ps(la, lb);
    res = _mm256_add_ps(res, lab);
  }
  float resTab[8] __attribute__((aligned(32)));
  _mm256_store_ps(resTab, res);
  return resTab[0] + resTab[1] + resTab[2] + resTab[3] + resTab[4] + resTab[5] + resTab[6] + resTab[7];
}

float produitScalaireAVXFMA(float *A, float *B, int taille)
{
  __m256 res = _mm256_set1_ps(0.0f);
  for (size_t i = 0; i < taille; i += 8) {
    __m256 la = _mm256_load_ps(A + i);
    __m256 lb = _mm256_load_ps(B + i);
    res = _mm256_fmadd_ps(la, lb, res);
  }
  float resTab[8] __attribute__((aligned(32)));
  _mm256_store_ps(resTab, res);
  return resTab[0] + resTab[1] + resTab[2] + resTab[3] + resTab[4] + resTab[5] + resTab[6] + resTab[7];
}

float produitScalaireAVXFMADeroulement(float *__restrict__ A, float *__restrict__ B, int taille)
{
  __m256 res1 = _mm256_set1_ps(0.0f);
  __m256 res2 = _mm256_set1_ps(0.0f);
  __m256 res3 = _mm256_set1_ps(0.0f);
  __m256 res4 = _mm256_set1_ps(0.0f);
  __m256 res5 = _mm256_set1_ps(0.0f);
  __m256 res6 = _mm256_set1_ps(0.0f);
  __m256 res7 = _mm256_set1_ps(0.0f);
  __m256 res8 = _mm256_set1_ps(0.0f);
  for (size_t i = 0; i < taille; i += 64) {
    __m256 la1 = _mm256_load_ps(A + i);
    __m256 la2 = _mm256_load_ps(A + i + 8);
    __m256 la3 = _mm256_load_ps(A + i + 16);
    __m256 la4 = _mm256_load_ps(A + i + 24);
    __m256 la5 = _mm256_load_ps(A + i + 32);
    __m256 la6 = _mm256_load_ps(A + i + 40);
    __m256 la7 = _mm256_load_ps(A + i + 48);
    __m256 la8 = _mm256_load_ps(A + i + 56);
    __m256 lb1 = _mm256_load_ps(B + i);
    __m256 lb2 = _mm256_load_ps(B + i + 8);
    __m256 lb3 = _mm256_load_ps(B + i + 16);
    __m256 lb4 = _mm256_load_ps(B + i + 24);
    __m256 lb5 = _mm256_load_ps(B + i + 32);
    __m256 lb6 = _mm256_load_ps(B + i + 40);
    __m256 lb7 = _mm256_load_ps(B + i + 48);
    __m256 lb8 = _mm256_load_ps(B + i + 56);
    res1 = _mm256_fmadd_ps(la1, lb1, res1);
    res2 = _mm256_fmadd_ps(la2, lb2, res2);
    res3 = _mm256_fmadd_ps(la3, lb3, res3);
    res4 = _mm256_fmadd_ps(la4, lb4, res4);
    res5 = _mm256_fmadd_ps(la5, lb5, res5);
    res6 = _mm256_fmadd_ps(la6, lb6, res6);
    res7 = _mm256_fmadd_ps(la7, lb7, res7);
    res8 = _mm256_fmadd_ps(la8, lb8, res8);
  }
  res1 = _mm256_add_ps(res1, res2);
  res3 = _mm256_add_ps(res3, res4);
  res5 = _mm256_add_ps(res5, res6);
  res7 = _mm256_add_ps(res7, res8);
  res1 = _mm256_add_ps(res1, res3);
  res5 = _mm256_add_ps(res5, res7);
  res1 = _mm256_add_ps(res1, res5);
  float resTab[8] __attribute__((aligned(32)));
  _mm256_store_ps(resTab, res1);
  return resTab[0] + resTab[1] + resTab[2] + resTab[3] + resTab[4] + resTab[5] + resTab[6] + resTab[7];
}

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
  tab0 =(float*) _mm_malloc (dim*sizeof(float), 32);
  tab1 =(float*) _mm_malloc (dim*sizeof(float), 32);
  for (int i = 0; i < dim; i++) { 
    tab0[i] = i;
    tab1[i] = i;
  }
  
  // Produit scalaire non-vectorise. On repete NREPET fois pour mieux mesurer le temps d'execution
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    produitScalaire(tab0, tab1, dim);
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> tempsSeq = end-start;
  std::cout << std::scientific << "Produit scalaire sans AVX: " << tempsSeq.count() / NREPET << "s" << std::endl;

  // Produit scalaire vectorisee AVX.
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    produitScalaireAVX(tab0, tab1, dim);
  }
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> tempsAVX = end-start;
  std::cout << std::scientific << "Produit scalaire avec AVX: " << tempsAVX.count() / NREPET << "s" << std::endl;

  // Produit scalaire vectorisee AVX FMA.
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    produitScalaireAVXFMA(tab0, tab1, dim);
  }
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> tempsParAVXFMA = end-start;
  std::cout << std::scientific << "Produit scalaire avec AVX FMA: " << tempsParAVXFMA.count() / NREPET << "s" << std::endl;

  // Produit scalaire vectorisee AVX FMA deroulement.
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NREPET; i++) {
    produitScalaireAVXFMADeroulement(tab0, tab1, dim);
  }
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> tempsParAVXFMADeroule = end-start;
  std::cout << std::scientific << "Produit scalaire avec AVX FMA deroulement: " << tempsParAVXFMADeroule.count() / NREPET << "s" << std::endl;

  // Afficher l'acceleration et l'efficacite
  double acceleration = tempsSeq.count() / tempsParAVXFMADeroule.count();
  double efficacite = acceleration / 8;
  std::cout << "Acceleration: " << std::setprecision(2) << std::fixed << acceleration << std::endl;
  std::cout << "Efficacite: " << std::setprecision(2) << std::fixed << efficacite << std::endl;

  return 0;
}
