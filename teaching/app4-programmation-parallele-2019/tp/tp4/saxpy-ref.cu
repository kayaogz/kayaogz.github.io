#include <iostream>
#include <algorithm>
#include <chrono>
#include <cuda.h>

using namespace std;

__global__
void saxpy(size_t n, float a, float *x, float *y)
{
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) y[i] = a * x[i] + y[i];
}

int main(int argc, char **argv)
{
  size_t N = atoi(argv[1]);
  float *x, *y, *res, *dx, *dy;
  float a = 2.0f;

  // Allocate and initialize vectors x and y on the CPU
  x = (float *) malloc(N * sizeof(float));
  y = (float *) malloc(N * sizeof(float));
  res = (float *) malloc(N * sizeof(float));

  for (int i = 0; i < N; i++) {
    x[i] = i;
    y[i] = 1.0f;
  }

  // Allocate device vectors dx and dy, then copy x and y into them
  cudaMalloc(&dx,  N * sizeof(float));
  cudaMalloc(&dy, N * sizeof(float));

  cudaMemcpy(dx, x, N * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(dy, y, N * sizeof(float), cudaMemcpyHostToDevice);

  // Launch the CUDA kernel for saxpy
  int blockSize = 1024;
  saxpy<<<(N + blockSize - 1) / blockSize, blockSize>>>(N, a, dx, dy);

  auto begin = std::chrono::high_resolution_clock::now(); 
  cudaMemcpy(res, dy, N * sizeof(float), cudaMemcpyDeviceToHost);

  // Verify results
  {
    int i;
    for (i = 0; i < N; i++) {
      float temp = a * x[i] + y[i];
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
