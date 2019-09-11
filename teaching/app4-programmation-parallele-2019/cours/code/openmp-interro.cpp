
double sommeTableau(double *a, int N)
{
  double a[N];
  double somme = 0.0, sommeLocale = 0.0;

#pragma omp parallel firstprivate(sommeLocale)
  {
#pragma omp for
    for (int i = 0; i < N; i++) { sommeLocale += a[i]; }
#pragma omp critical
    somme += sommeLocale;
  }

  return somme;
}

void setZero(double *a, double *b, int N)
{
#pragma omp parallel num_threads(2)
  {
    int thid = omp_get_thread_num();
    std::cout << thid << endl;

//#pragma omp for
    for (int i = 0; i < N; i++) { a[i] = 0; }
    std::cout << thid << endl;

    if (thid == 0) { 
      for (int i = 0; i < N / 2; i++) { b[i] = 0; }
    }
    else { // thid == 1
      for (i = N / 2; i < N; i++) { b[i] = 0; }
    }
    std::cout << thid << endl;
  }
}
