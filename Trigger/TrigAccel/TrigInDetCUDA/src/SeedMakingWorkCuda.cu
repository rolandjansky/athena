/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cuda.h>
#include <cuda_runtime.h>

#include "TrigAccelEvent/TrigInDetAccelEDM.h"
#include "SeedMakingWorkCuda.h"

#include "tbb/tick_count.h"
#include <cstring>
#include <cmath>
#include <iostream>
#include <algorithm>

#include "DoubletCountingKernelCuda.cuh"
#include "DoubletMakingKernelCuda.cuh"
#include "DoubletMatchingKernelCuda.cuh"

SeedMakingWorkCuda::SeedMakingWorkCuda(unsigned int id, SeedMakingDeviceContext* ctx, std::shared_ptr<TrigAccel::OffloadBuffer> data, 
  tbb::concurrent_vector<WorkTimeStamp>* TL) : 
  m_workId(id),
  m_context(ctx), 
  m_input(data),
  m_timeLine(TL)
 {
  
  m_output = std::make_shared<TrigAccel::OffloadBuffer>(sizeof(TrigAccel::OUTPUT_SEED_STORAGE));//output data
}

SeedMakingWorkCuda::~SeedMakingWorkCuda() {
  
  SeedMakingDeviceContext* p = m_context;

  int id = p->m_deviceId;

  cudaSetDevice(id);

  cudaStreamDestroy(p->m_stream);

  cudaFree(p->d_settings);
  cudaFree(p->d_spacepoints);
  cudaFree(p->d_detmodel);
  
  cudaFree(p->d_outputseeds);
  cudaFree(p->d_doubletstorage);
  cudaFree(p->d_doubletinfo);

  cudaFreeHost(p->h_settings);
  cudaFreeHost(p->h_spacepoints);
  cudaFreeHost(p->h_outputseeds);

  delete p;
  m_context = 0;
}

std::shared_ptr<TrigAccel::OffloadBuffer> SeedMakingWorkCuda::getOutput() {
  return m_output;
}

bool SeedMakingWorkCuda::run() {

  m_timeLine->push_back(WorkTimeStamp(m_workId, 0, tbb::tick_count::now()));

  const SeedMakingDeviceContext& p = *m_context;
  
  int id = p.m_deviceId;  
  
  TrigAccel::OUTPUT_SEED_STORAGE* ps = reinterpret_cast<TrigAccel::OUTPUT_SEED_STORAGE*>(p.h_outputseeds);
  
  cudaSetDevice(id);

  checkError();
  
  cudaMemcpyAsync(p.d_settings, p.h_settings, sizeof(TrigAccel::SEED_FINDER_SETTINGS), cudaMemcpyHostToDevice, p.m_stream);

  checkError();

  cudaMemcpyAsync(p.d_spacepoints, p.h_spacepoints, sizeof(TrigAccel::SPACEPOINT_STORAGE), cudaMemcpyHostToDevice, p.m_stream);

  checkError();

  cudaStreamSynchronize(p.m_stream);
    
  TrigAccel::SEED_FINDER_SETTINGS* dSettings  = reinterpret_cast<TrigAccel::SEED_FINDER_SETTINGS *>(p.d_settings);
  TrigAccel::SPACEPOINT_STORAGE* dSpacepoints = reinterpret_cast<TrigAccel::SPACEPOINT_STORAGE *>(p.d_spacepoints);
  TrigAccel::DETECTOR_MODEL* dDetModel        = reinterpret_cast<TrigAccel::DETECTOR_MODEL*>(p.d_detmodel);
  TrigAccel::OUTPUT_SEED_STORAGE* dOutput     = reinterpret_cast<TrigAccel::OUTPUT_SEED_STORAGE*>(p.d_outputseeds);

  DOUBLET_INFO* dInfo                         = reinterpret_cast<DOUBLET_INFO*>(p.d_doubletinfo);
  DOUBLET_STORAGE* dStorage                   = reinterpret_cast<DOUBLET_STORAGE*>(p.d_doubletstorage);

  cudaMemset(p.d_outputseeds,0,10*sizeof(int));

  checkError();

  cudaMemset(p.d_doubletstorage,0,3*sizeof(int));

  checkError();
  
  const TrigAccel::SPACEPOINT_STORAGE* pSPS = reinterpret_cast<const TrigAccel::SPACEPOINT_STORAGE *>(p.h_spacepoints);
  int nSlices = pSPS->m_nPhiSlices;
  int nLayers = pSPS->m_nLayers;
  
  int nMiddleSp = NUM_MIDDLE_THREADS;//determines size of the doublet/triplet buffers
  int nOtherSp = OUTER_THREADS_MULTIPLIER*p.m_gpuParams.m_nNUM_SMX_CORES/NUM_MIDDLE_THREADS;//the size of the spacepoint buffer

  dim3 gridDimensions(nSlices, nLayers);
  dim3 blockDimensions(nMiddleSp, nOtherSp);

  cudaMemset(p.d_doubletinfo,0,sizeof(DOUBLET_INFO));

  checkError();

  cudaStreamSynchronize(p.m_stream);

  checkError();

  doubletCountingKernel<<<gridDimensions, blockDimensions, 0, p.m_stream>>>(dSettings, dSpacepoints, dDetModel, dInfo, nLayers, nSlices);

  cudaStreamSynchronize(p.m_stream);

  checkError();

  doubletMakingKernel<<<gridDimensions, blockDimensions, 0, p.m_stream>>>(dSettings, dSpacepoints, dDetModel, dOutput, 
    dInfo, dStorage, nLayers, nSlices);

  cudaStreamSynchronize(p.m_stream);

  checkError();

  int nStats[3];

  cudaMemcpy(&nStats[0], p.d_doubletstorage, 3*sizeof(int), cudaMemcpyDeviceToHost);

  
  doubletMatchingKernel<<<p.m_gpuParams.m_nNUM_TRIPLET_BLOCKS, NUM_TRIPLET_THREADS, 0, p.m_stream>>>(dSettings, dSpacepoints, dDetModel, dInfo, 
    dStorage,  dOutput, nStats[0]);

  cudaStreamSynchronize(p.m_stream);

  checkError();

  TrigAccel::OUTPUT_SEED_STORAGE* pOutput = reinterpret_cast<TrigAccel::OUTPUT_SEED_STORAGE *>(m_output->m_rawBuffer);

  //Read back GPU results

  pOutput->m_nMiddleSps = 0;
  pOutput->m_nSeeds = 0;		
  pOutput->m_nI = 0;
  pOutput->m_nO = 0;
  pOutput->m_nCandidates = 0;

  cudaMemcpyAsync(p.h_outputseeds, p.d_outputseeds, sizeof(TrigAccel::OUTPUT_SEED_STORAGE), cudaMemcpyDeviceToHost, p.m_stream);

  checkError();
  
  cudaStreamSynchronize(p.m_stream);

  checkError();

  memcpy(pOutput, ps, sizeof(TrigAccel::OUTPUT_SEED_STORAGE));
  
  m_timeLine->push_back(WorkTimeStamp(m_workId, 1, tbb::tick_count::now()));

  return true;
}
