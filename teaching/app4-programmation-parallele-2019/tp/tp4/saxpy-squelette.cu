#include <iostream>
#include <algorithm>
#include <chrono>
#include <cuda.h>

using namespace std;

__global__
void saxpy(size_t n, float alpha, float *a, float *b)
{
}

int main(int argc, char **argv)
{
  size_t N = 1024;
  float *a, *b, *res, *da, *db;
  float alpha = 2.0f;

  // Allocate and initialize vectors a and b on the CPU
  a = (float *) malloc(N * sizeof(float));
  b = (float *) malloc(N * sizeof(float));
  res = (float *) malloc(N * sizeof(float));

  for (int i = 0; i < N; i++) {
    a[i] = i;
    b[i] = 1.0f;
  }

  // Allocate device vectors da and db, then copy a and b into them
  // A FAIRE

  // Launch the CUDA kernel for saxpy
  // A FAIRE

  // Copy results back to the CPU
  // A FAIRE

  // Verify results
  {
    int i;
    for (i = 0; i < N; i++) {
      float temp = alpha * a[i] + b[i];
      if (std::abs(res[i] - temp) / std::max(1e-6f, temp) > 1e-6) { 
        cout << res[i] << " " << temp << endl;
        break;
      }
    }
    if (i == N) {
      cout << "saxpy on GPU is correct." << endl;
    } else {
      cout << "saxpy on GPU is incorrect on element " << i << "." << endl;
    }
  }

  return 0;
}
