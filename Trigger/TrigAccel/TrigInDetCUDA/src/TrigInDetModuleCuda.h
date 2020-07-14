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

    inline void checkError() const {
      cudaError_t error = cudaGetLastError();
      if(error != cudaSuccess) {
	printf("CUDA error: %s\n", cudaGetErrorString(error));
	exit(-1);
      }
    };
    
    //data structures

    //1. "const" data: managed by the Factory
    
    std::map<unsigned int, unsigned char*> m_d_detmodels;
    
    int m_maxNumberOfContexts;//Factory setting
    int m_maxDevice;

    bool m_usePinnedMemory; 
    bool m_useWriteCombinedMemory; 
    bool m_linkOutputToShm;

    // host / device pairs for each device

    tbb::concurrent_queue<SeedMakingDeviceContext*> m_seedMakingDcQueue;

    // data context allocation / de-allocation
    
    SeedMakingDeviceContext* createSeedMakingContext(int);
    void deleteSeedMakingContext(SeedMakingDeviceContext*);

    bool m_dumpTimeLine;
    
    std::atomic<unsigned int> m_workItemCounters[100];//atomic counters for unique Work identification
    tbb::concurrent_vector<WorkTimeStamp> m_timeLine;
  };

#endif
