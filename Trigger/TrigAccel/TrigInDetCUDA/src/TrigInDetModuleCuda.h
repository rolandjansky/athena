/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETCUDA_TRIGINDETMODULECUDA_H
#define TRIGINDETCUDA_TRIGINDETMODULECUDA_H

#include <map>
#include <atomic>
#include "TrigAccelEvent/WorkFactory.h"
#include "TrigAccelEvent/TrigInDetAccelEDM.h"
#include "TrigAccelEvent/TrigInDetAccelCodes.h"

#include "TrigInDetDataContexts.h"

#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_vector.h"

class TrigInDetModuleCuda : public TrigAccel::WorkFactory {

 public:

  TrigInDetModuleCuda();

  ~TrigInDetModuleCuda();

  bool configure();

  TrigAccel::Work* createWork(int, std::shared_ptr<TrigAccel::OffloadBuffer>);

  const std::vector<int> getProvidedAlgs();

  virtual int getFactoryId() {
    return TrigAccel::TrigInDetModuleID_CUDA;
  }
    
  private:

    inline void checkError(int code = 0) const {
      cudaError_t error = cudaGetLastError();
      if(error != cudaSuccess) {
        printf("%d CUDA error %d: %s\n", getpid(), code, cudaGetErrorString(error));
        exit(-1);
      }
    };
    
    void getNumberOfGPUs();
    
    //data structures

    //1. "const" data: managed by the Factory

    unsigned char* m_h_detmodel;
    
    int m_maxDevice;

    // data context allocation / de-allocation
    
    SeedMakingDeviceContext* createSeedMakingContext(int) const;
    void deleteSeedMakingContext(SeedMakingDeviceContext*) const;

    bool m_dumpTimeLine;
    
    std::atomic<unsigned int> m_workItemCounters[100];//atomic counters for unique Work identification
    tbb::concurrent_vector<WorkTimeStamp> m_timeLine;
  };

#endif
