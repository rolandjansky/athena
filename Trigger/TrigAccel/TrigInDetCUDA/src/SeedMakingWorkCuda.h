/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETCUDA_SEEDMAKINGWORKCUDA_H
#define TRIGINDETCUDA_SEEDMAKINGWORKCUDA_H

#include <vector>

#include "tbb/concurrent_vector.h"
#include "TrigInDetDataContexts.h"
#include "TrigAccelEvent/Work.h"
#include "TrigAccelEvent/WorkFactory.h"


class SeedMakingWorkCuda : public TrigAccel::Work{

public:
  SeedMakingWorkCuda(unsigned int, SeedMakingDeviceContext*, std::shared_ptr<TrigAccel::OffloadBuffer>, 
  tbb::concurrent_vector<WorkTimeStamp>*);
  ~SeedMakingWorkCuda();
  std::shared_ptr<TrigAccel::OffloadBuffer> getOutput();
  bool run();
  unsigned int getId() const {
    return m_workId;
  }
  
private:

  inline void checkError() const {
    cudaError_t error = cudaGetLastError();
    if(error != cudaSuccess) {
      printf("CUDA error: %s\n", cudaGetErrorString(error));
      exit(-1);
    }
  };
  
  unsigned int m_workId;
  SeedMakingDeviceContext* m_context;  
  std::shared_ptr<TrigAccel::OffloadBuffer> m_input, m_output;
  tbb::concurrent_vector<WorkTimeStamp>* m_timeLine;

  float m_CovMS, m_ptCoeff, m_minPt2, m_ptCoeff2, m_maxD0; 
};

#endif
