#include "ISF_FastCaloGpu/CaloGpuGeneral_cu.h"
#include "ISF_FastCaloGpu/GeoRegion.h"
#include "ISF_FastCaloGpu/GeoGpu_structs.h"
#include "ISF_FastCaloGpu/Hit.h"
#include "ISF_FastCaloGpu/Rand4Hits.h"

#include "ISF_FastCaloGpu/gpuQ.h"
#include "ISF_FastCaloGpu/Args.h"
#include <chrono>

#define BLOCK_SIZE 256

using namespace CaloGpuGeneral_fnc;

namespace CaloGpuGeneral_cu {

  __global__ void simulate_A( float E, int nhits, Chain0_Args args, bool reweight ) {

    long t = threadIdx.x + blockIdx.x * blockDim.x;
    if ( t < nhits ) {
      Hit hit;
      hit.E() = E;
      CenterPositionCalculation_d( hit, args );
      HistoLateralShapeParametrization_d( hit, t, args, reweight );
      HitCellMappingWiggle_d( hit, args, t );
    }
  }

  __global__ void simulate_ct( Chain0_Args args ) {

    unsigned long tid = threadIdx.x + blockIdx.x * blockDim.x;
    if ( tid < args.ncells ) {
      if ( args.cells_energy[tid] > 0 ) {
        unsigned int ct = atomicAdd( args.hitcells_ct, 1 );
        Cell_E       ce;
        ce.cellid           = tid;
        ce.energy           = args.cells_energy[tid];
        args.hitcells_E[ct] = ce;
      }
    }
  }

  __global__ void simulate_clean( Chain0_Args args ) {
    unsigned long tid = threadIdx.x + blockIdx.x * blockDim.x;
    if ( tid < args.ncells ) { args.cells_energy[tid] = 0.0; }
    if ( tid == 0 ) args.hitcells_ct[0] = 0;
  }

  __host__ void simulate_A_cu( float E, int nhits, Chain0_Args& args, bool reweight ) {
    int blocksize   = BLOCK_SIZE;
    int threads_tot = nhits;
    int nblocks     = ( threads_tot + blocksize - 1 ) / blocksize;
    simulate_A<<<nblocks, blocksize>>>( E, nhits, args, reweight );
  }

  __host__ void simulate_hits( float E, int nhits, Chain0_Args& args , bool reweight ) {

    cudaError_t err = cudaGetLastError();

    unsigned long ncells      = args.ncells;
    int           blocksize   = BLOCK_SIZE;
    int           threads_tot = args.ncells;
    int           nblocks     = ( threads_tot + blocksize - 1 ) / blocksize;

    // cleal the threads
    simulate_clean<<<nblocks, blocksize>>>( args );

    blocksize   = BLOCK_SIZE;
    threads_tot = nhits;
    nblocks     = ( threads_tot + blocksize - 1 ) / blocksize;

    // main cuda kernels
    // simulate the desposit energy for each hit
    simulate_A<<<nblocks, blocksize>>>( E, nhits, args, reweight );

    nblocks = ( ncells + blocksize - 1 ) / blocksize;
    // get the deposit energy
    simulate_ct<<<nblocks, blocksize>>>( args );

    int ct;
    gpuQ( cudaMemcpy( &ct, args.hitcells_ct, sizeof( int ), cudaMemcpyDeviceToHost ) );
    gpuQ( cudaMemcpy( args.hitcells_E_h, args.hitcells_E, ct * sizeof( Cell_E ), cudaMemcpyDeviceToHost ) );

    // pass result back
    args.ct = ct;
  }

} // namespace CaloGpuGeneral_cu
