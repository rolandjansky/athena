/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETCUDA_TRIGINDETDATACONTEXTS_H
#define TRIGINDETCUDA_TRIGINDETDATACONTEXTS_H

#include "TrigAccelEvent/TrigInDetAccelEDM.h"

#include "SeedMakingDataStructures.h"
#include <cuda_runtime.h>
#include <tbb/tick_count.h>

class WorkTimeStamp {
public:
  WorkTimeStamp(unsigned int id, int ev, const tbb::tick_count& t) :
    m_workId(id), m_eventType(ev), m_time(t) {};
  WorkTimeStamp(const WorkTimeStamp& w) : m_workId(w.m_workId), m_eventType(w.m_eventType), m_time(w.m_time) {};
  unsigned int m_workId;
  int m_eventType;
  tbb::tick_count m_time;
};


struct SeedMakingDeviceContext {
public:
  SeedMakingDeviceContext() : m_deviceId(-1), h_spacepoints(0), d_spacepoints(0), d_size(0), h_size(0) {};
  size_t hostSize() { return h_size;}
  size_t deviceSize() { return d_size;}
  
  int m_deviceId;
  cudaStream_t m_stream;
  unsigned char *h_settings;
  unsigned char *d_settings;
  unsigned char *h_spacepoints;
  unsigned char *d_spacepoints;
  
  unsigned char *d_detmodel;
  
  unsigned char *h_outputseeds;
  unsigned char *d_outputseeds;
  
  unsigned char *d_doubletstorage;
  unsigned char *d_doubletinfo;
  
  size_t d_size, h_size;
  GPU_PARAMETERS m_gpuParams;
  
private:
  SeedMakingDeviceContext(const SeedMakingDeviceContext& sc) : m_deviceId(sc.m_deviceId) {};
};




#endif
