/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cuda.h>
#include <cuda_runtime.h>
#include <atomic>

#include "TrigInDetModuleCuda.h"
#include "SeedMakingDataStructures.h"
#include "SeedMakingWorkCuda.h"

#include "TrigAccelEvent/TrigInDetAccelCodes.h"

extern "C" TrigAccel::WorkFactory* getFactory() {
  return new TrigInDetModuleCuda();
}

extern "C" int getFactoryId() { 
  return TrigAccel::TrigInDetModuleID_CUDA; 
} 	  

extern "C" void deleteFactory(TrigAccel::WorkFactory* c){
  TrigInDetModuleCuda* mod=reinterpret_cast<TrigInDetModuleCuda*>(c);
  delete mod;
}

TrigInDetModuleCuda::TrigInDetModuleCuda() : m_maxNumberOfContexts(12), m_maxDevice(0), m_usePinnedMemory(true), 
						  m_useWriteCombinedMemory(false),  m_linkOutputToShm(false), m_dumpTimeLine(false) {

  m_d_detmodels.clear();

  cudaGetDeviceCount(&m_maxDevice);

  cudaError_t error = cudaGetLastError();

  if(error != cudaSuccess) {
    m_maxDevice = 0;
  }

  for(unsigned int i=0;i<getProvidedAlgs().size();i++) {
    m_workItemCounters[i] = 0;
  }

  m_timeLine.clear();

}

TrigInDetModuleCuda::~TrigInDetModuleCuda() {

  SeedMakingDeviceContext* ps = 0;
  
  std::cout<<"deleting "<<m_seedMakingDcQueue.unsafe_size()<<" device contexts"<<std::endl;
  
  while(m_seedMakingDcQueue.try_pop(ps)) deleteSeedMakingContext(ps);

  for(auto dm : m_d_detmodels) {
    cudaSetDevice(dm.first);
    cudaFree(dm.second);

  }
  m_d_detmodels.clear();

  if(m_dumpTimeLine) {

     std::cout<<"time_line has "<<m_timeLine.size()<<" events"<<std::endl;
     if(m_timeLine.size() > 0) {
        tbb::tick_count t0 = m_timeLine[0].m_time;
        std::ofstream tl("timeLine.csv");
        tl<<"workId,eventType,time"<<std::endl;
        tl<<m_timeLine[0].m_workId<<","<<m_timeLine[0].m_eventType<<",0"<<std::endl;
        for(unsigned int tIdx = 1;tIdx < m_timeLine.size();++tIdx) {
           tbb::tick_count t1 = m_timeLine[tIdx].m_time;
           auto duration = t1-t0;
           tl<<m_timeLine[tIdx].m_workId<<","<<m_timeLine[tIdx].m_eventType<<","<<1000*duration.seconds()<<std::endl;
        }
        tl.close();
     }
  }
}

bool TrigInDetModuleCuda::configure() {

  std::vector<int> allowedGPUs, nDeviceContexts;

  allowedGPUs.resize(1,0);//configured for just 1 device with deviceId = 0

  nDeviceContexts.resize(1,8);//configured for 8 DataContexts

  if(m_maxDevice == 0) {
     std::cout<<"No CUDA devices found"<<std::endl;
     return false;
  }
  
  if(allowedGPUs.empty() || nDeviceContexts.empty()) return false;

  if(allowedGPUs.size() != nDeviceContexts.size()) return false;

  unsigned int dcIndex=0;

  size_t memTotalSize = 0;

  std::vector< SeedMakingDeviceContext*> vSeedDCs[100];//we do not have that many GPUs

  int nDCTotal=0;

  for(std::vector<int>::iterator devIt = allowedGPUs.begin(); devIt!= allowedGPUs.end();++devIt, dcIndex++) {

    int deviceId = (*devIt);

    if(deviceId<0 || deviceId>=m_maxDevice) continue;

    size_t memTotalSizeOnDevice = 0;

    cudaSetDevice(deviceId);

    checkError();
        
    unsigned char* d_detmodel;

    cudaMalloc((void **)&d_detmodel, sizeof(TrigAccel::DETECTOR_MODEL));

    checkError();
    
    m_d_detmodels.insert(std::pair<unsigned int, unsigned char*>(deviceId, d_detmodel));
    
    int nDC = nDeviceContexts[dcIndex];
    nDCTotal += nDC;

    memTotalSizeOnDevice += sizeof(TrigAccel::DETECTOR_MODEL);

    for(int dc=0;dc<nDC;dc++) {
      SeedMakingDeviceContext* p = createSeedMakingContext(deviceId);
      memTotalSizeOnDevice += p->deviceSize();
      vSeedDCs[dcIndex].push_back(p);
    }

    memTotalSize += memTotalSizeOnDevice;
    
    std::cout<<"GPU"<<deviceId<<" allocated data context size = "<<1e-6*memTotalSizeOnDevice<<" MBytes"<<std::endl;
  }

  int nDCLeft = nDCTotal;
  while(nDCLeft>0) {
     for(unsigned int iGPU=0;iGPU<allowedGPUs.size();iGPU++) {
        if(vSeedDCs[iGPU].empty()) continue;
	m_seedMakingDcQueue.push(vSeedDCs[iGPU].back());
	vSeedDCs[iGPU].pop_back();
        --nDCLeft;
     }
  }
  
  std::cout<<"Data context queue : ";
  for(tbb::concurrent_queue< SeedMakingDeviceContext*>::const_iterator i(m_seedMakingDcQueue.unsafe_begin()); i!=m_seedMakingDcQueue.unsafe_end(); ++i ) {
    std::cout<<(*i)->m_deviceId<<" ";
  }
  std::cout<<std::endl;

  std::cout<<"Total size of memory allocated on all GPUs = "<<1e-6*memTotalSize<<" MBytes"<<std::endl;

  return true;
}


SeedMakingDeviceContext* TrigInDetModuleCuda::createSeedMakingContext(int id) {

  cudaSetDevice(id);

  SeedMakingDeviceContext* p = new SeedMakingDeviceContext;

  p->m_deviceId = id;

  //set stream

  cudaStreamCreate(&p->m_stream);

  //check device property

  cudaDeviceProp deviceProp;
  cudaGetDeviceProperties(&deviceProp, id);

  p->m_gpuParams.m_nSMX = deviceProp.multiProcessorCount;

  int ncores = 0;

  if ((deviceProp.minor == 1) || (deviceProp.minor == 2)) ncores = 128;
  else if (deviceProp.minor == 0) ncores = 64;	   
       else printf("Cannot determine the number of cores: unknown device type\n"); 

  p->m_gpuParams.m_nNUM_SMX_CORES = ncores;//_ConvertSMVer2Cores_local(deviceProp.major, deviceProp.minor);
  p->m_gpuParams.m_nNUM_TRIPLET_BLOCKS = NUM_TRIPLET_BLOCKS;
  if(deviceProp.maxThreadsPerBlock < p->m_gpuParams.m_nNUM_TRIPLET_BLOCKS) 
    p->m_gpuParams.m_nNUM_TRIPLET_BLOCKS = deviceProp.maxThreadsPerBlock;

  //Allocate memory
  
  cudaMalloc((void **)&p->d_settings, sizeof(TrigAccel::SEED_FINDER_SETTINGS));
  cudaMalloc((void **)&p->d_spacepoints, sizeof(TrigAccel::SPACEPOINT_STORAGE));

  auto dmIt = m_d_detmodels.find(p->m_deviceId);
  if(dmIt!=m_d_detmodels.end()) {
    p->d_detmodel = (*dmIt).second;
  }
  
  cudaMalloc((void **)&p->d_outputseeds, sizeof(TrigAccel::OUTPUT_SEED_STORAGE));
  cudaMalloc((void **)&p->d_doubletstorage, sizeof(DOUBLET_STORAGE));
  cudaMalloc((void **)&p->d_doubletinfo, sizeof(DOUBLET_INFO));


  p->d_size = sizeof(TrigAccel::SEED_FINDER_SETTINGS) +  
              sizeof(TrigAccel::SPACEPOINT_STORAGE) + sizeof(TrigAccel::OUTPUT_SEED_STORAGE) + sizeof(DOUBLET_STORAGE) + sizeof(DOUBLET_INFO);

  cudaMallocHost((void **)&p->h_settings, sizeof(TrigAccel::SEED_FINDER_SETTINGS));
  cudaMallocHost((void **)&p->h_spacepoints, sizeof(TrigAccel::SPACEPOINT_STORAGE));
  cudaMallocHost((void **)&p->h_outputseeds, sizeof(TrigAccel::OUTPUT_SEED_STORAGE));

  p->h_size = sizeof(TrigAccel::SEED_FINDER_SETTINGS) + sizeof(TrigAccel::SPACEPOINT_STORAGE) + sizeof(TrigAccel::OUTPUT_SEED_STORAGE);

  return p;
}

void TrigInDetModuleCuda::deleteSeedMakingContext(SeedMakingDeviceContext* p) {

  int id = p->m_deviceId;

  cudaSetDevice(id);

  cudaStreamDestroy(p->m_stream);

  cudaFree(p->d_settings);
  cudaFree(p->d_spacepoints);
  
  cudaFree(p->d_outputseeds);
  cudaFree(p->d_doubletstorage);
  cudaFree(p->d_doubletinfo);

  cudaFreeHost(p->h_settings);
  cudaFreeHost(p->h_spacepoints);
  cudaFreeHost(p->h_outputseeds);

  delete p;

}


TrigAccel::Work* TrigInDetModuleCuda::createWork(int workType, std::shared_ptr<TrigAccel::OffloadBuffer> data){
  
  if(workType == TrigAccel::InDetJobControlCode::SIL_LAYERS_EXPORT){
    
    for(auto dm : m_d_detmodels) {

       unsigned int deviceId = dm.first;

       cudaSetDevice(deviceId);
       
       cudaMemcpy(dm.second, (unsigned char*)data->get(), sizeof(TrigAccel::DETECTOR_MODEL), cudaMemcpyHostToDevice);
    }
    return 0;
  }

  if(workType == TrigAccel::InDetJobControlCode::MAKE_SEEDS){
 
    SeedMakingDeviceContext* ctx = 0;
    
    while(!m_seedMakingDcQueue.try_pop(ctx)) {
      //      std::cout<<"waiting for free device context..."<<std::endl;
    };

    TrigAccel::SEED_MAKING_JOB *pArray = reinterpret_cast<TrigAccel::SEED_MAKING_JOB*>(data->get());
    
    //1. copy settings to the context host array

    TrigAccel::SEED_FINDER_SETTINGS* p_settings = reinterpret_cast<TrigAccel::SEED_FINDER_SETTINGS*>(ctx->h_settings);
    memcpy(p_settings, &pArray->m_settings, sizeof(TrigAccel::SEED_FINDER_SETTINGS));

    //2. copy spacepoints to the context host array

    TrigAccel::SPACEPOINT_STORAGE* p_spacePoints = reinterpret_cast<TrigAccel::SPACEPOINT_STORAGE*>(ctx->h_spacepoints);
    memcpy(p_spacePoints, &pArray->m_data, sizeof(TrigAccel::SPACEPOINT_STORAGE));
    
    unsigned int workNum = m_workItemCounters[0]++;//seed making uses counter #0
    
    unsigned int workId = workNum*100;
    
    SeedMakingWorkCuda* w = new SeedMakingWorkCuda(workId, SeedMakingWorkContextCuda(ctx, m_usePinnedMemory, 
									     m_useWriteCombinedMemory, 
									     m_linkOutputToShm), data, m_seedMakingDcQueue, m_timeLine);
    
    return w;
  }

  return 0;
}

const std::vector<int> TrigInDetModuleCuda::getProvidedAlgs(){
  std::vector<int> v{
      TrigAccel::InDetJobControlCode::SIL_LAYERS_EXPORT,
      TrigAccel::InDetJobControlCode::MAKE_SEEDS
  };
  return v;
}

