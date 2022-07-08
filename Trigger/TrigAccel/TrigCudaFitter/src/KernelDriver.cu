#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <cuda_runtime.h>
#include <chrono>

#include "DataTypes.h"

//#include "KalmanFilter_kernel_ver2.cuh"
#include "KalmanFilter_kernel_ver3.cuh"


extern "C" float runKernel(INPUT_TRACK_INFO_TYPE* d_In,
         DETECTOR_SURFACE_TYPE* d_Geo,
         OUTPUT_TRACK_INFO_TYPE* d_Out,
         HIT_INFO_TYPE* d_Hit,int Ntracks) {

  int threadsPerBlock = BLOCK_SIZE;

  int blocksPerGrid;

  if(Ntracks<BLOCK_SIZE)
    blocksPerGrid=1;
  else
    blocksPerGrid = (Ntracks + threadsPerBlock - 1) / threadsPerBlock;

  cudaDeviceSynchronize();

  auto kernelStart = std::chrono::steady_clock::now();

  KalmanFilterGPU<<<blocksPerGrid, threadsPerBlock>>>(d_In, d_Out, d_Geo, d_Hit, Ntracks);

  cudaError_t err = cudaGetLastError();

  if (err != cudaSuccess) {
    printf("Kernel launch: error code : %s\n", cudaGetErrorString(err));
  }
  cudaDeviceSynchronize();

  auto kernelStop = std::chrono::steady_clock::now();

  return (float)std::chrono::duration_cast<std::chrono::nanoseconds>( kernelStop - kernelStart ).count() * 1E-6;
}
