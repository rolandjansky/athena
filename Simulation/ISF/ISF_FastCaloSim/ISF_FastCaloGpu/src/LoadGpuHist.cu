#include "ISF_FastCaloGpu/LoadGpuHist.h"
#include <iostream>
#include "ISF_FastCaloGpu/gpuQ.h"
#include <cuda.h>

LoadGpuHist::LoadGpuHist() {}

LoadGpuHist::~LoadGpuHist() {
  free( m_hf1d );
  cudaFree( ( *m_hf1d_h ).h_errors);
  cudaFree( ( *m_hf1d_h ).h_contents );
  cudaFree( ( *m_hf1d_h ).h_borders);
  free( m_hf1d_h );
  cudaFree( m_hf1d_d );
}

void LoadGpuHist::LD1D() {
  // this call  assume  already have Histo set in m_hf
  // this function allocate memory of GPU and deep copy m_hf to m_hf_d
  if ( !m_hf1d ) {
    std::cout << "Error Load 1DHisto " << std::endl;
    return;
  }

  FH1D* hf_ptr = new FH1D;
  FH1D  hf;

  hf.nbins = ( *m_hf1d ).nbins;

  gpuQ( cudaMalloc( (void**)&hf.h_borders, ( hf.nbins + 1 ) * sizeof( float ) ) );
  gpuQ( cudaMalloc( (void**)&hf.h_contents,  hf.nbins  * sizeof( float ) ) );
  gpuQ( cudaMalloc( (void**)&hf.h_errors,  hf.nbins  * sizeof( float ) ) );
  gpuQ( cudaMemcpy( hf.h_borders, ( *m_hf1d ).h_borders, ( hf.nbins + 1 ) * sizeof( float ),
                    cudaMemcpyHostToDevice ) );
  gpuQ( cudaMemcpy( hf.h_contents, ( *m_hf1d ).h_contents,  hf.nbins  * sizeof( float ),
                    cudaMemcpyHostToDevice ) );
  gpuQ( cudaMemcpy( hf.h_errors, ( *m_hf1d ).h_errors,  hf.nbins  * sizeof( float ),
                    cudaMemcpyHostToDevice ) );
  *( hf_ptr ) = hf;
  m_hf1d_h    = hf_ptr;

  gpuQ( cudaMalloc( (void**)&m_hf1d_d, sizeof( FH1D ) ) );
  gpuQ( cudaMemcpy( m_hf1d_d, m_hf1d_h, sizeof( FH1D ), cudaMemcpyHostToDevice ) );
}

