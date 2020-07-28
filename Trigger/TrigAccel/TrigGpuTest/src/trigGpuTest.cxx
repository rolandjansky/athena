/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <dlfcn.h>

#include <experimental/filesystem>
#include "tbb/tick_count.h"

#include "TrigAccelEvent/WorkFactory.h"
#include "TrigAccelEvent/DataExportBuffer.h"
#include "TrigAccelEvent/TrigInDetAccelEDM.h"
#include "TrigAccelEvent/TrigInDetAccelCodes.h"

#include <vector>
#include <memory>

int main(int argc, char* argv[]) {
  if(argc < 4) {
    std::cout<<"trigGpuTest usage: ./trigGpuTest <geo_file.bin> <data_dir> nevents"<<std::endl;
    exit(0);
  }

  
  //open the factory library

  void* handle = dlopen("libTrigInDetCUDA.so", RTLD_LAZY);
  
  if(!handle) {
    fprintf(stderr, "cannot load the factory library : %s\n", dlerror());
    exit(EXIT_FAILURE);
  }

  dlerror();

  //declare library interface methods 

  TrigAccel::WorkFactory* (*getFactory)();
  int (*getFactoryId)();
  void (*deleteFactory)(TrigAccel::WorkFactory*);

  getFactory   = (TrigAccel::WorkFactory* (*)()) dlsym(handle, "getFactory");
  getFactoryId = (int (*)()) dlsym(handle, "getFactoryId");
  deleteFactory = (void (*)(TrigAccel::WorkFactory*)) dlsym(handle, "deleteFactory");

  std::cout<<"factory library id = "<<std::hex<<getFactoryId()<<std::dec<<std::endl;

  TrigAccel::WorkFactory* pW = getFactory();

  bool cfgResult = pW->configure();

  if(!cfgResult) {
    std::cout<<"Factory config failed"<<std::endl;
    exit(-2);
  }

  
  const size_t bufferOffset = 256;

  TrigAccel::DATA_EXPORT_BUFFER* pBG = new TrigAccel::DATA_EXPORT_BUFFER();

  std::string geoName(argv[1]);

  std::cout<<"reading geometry from file "<<geoName<<std::endl;

  size_t bSize = pBG->load(geoName);
  
  std::cout<<"loaded "<<bSize<<" bytes"<<std::endl;
  
  std::shared_ptr<TrigAccel::OffloadBuffer> pDMBuff = std::make_shared<TrigAccel::OffloadBuffer>(pBG);

  delete pBG;

  std::cout<<"Creating Work item for task "<<TrigAccel::InDetJobControlCode::SIL_LAYERS_EXPORT<<std::endl;

  pW->createWork(TrigAccel::InDetJobControlCode::SIL_LAYERS_EXPORT, pDMBuff);


  std::string data_path(argv[2]);
  std::vector<std::string> event_files;

  for(const auto& entry : std::experimental::filesystem::directory_iterator(data_path)) {
    event_files.push_back(entry.path());
  }

  int nEvents = atoi(argv[3]);

  std::cout<<"running the GPU test with "<<nEvents<<" events"<<std::endl;

  TrigAccel::DATA_EXPORT_BUFFER* pB = new TrigAccel::DATA_EXPORT_BUFFER();

  int fileIdx = 0;
  
  std::ofstream timeFile("results.csv");
  
  timeFile<<"nsp,nseeds,time"<<std::endl;
  
  for(int iEvent=0;iEvent<nEvents;iEvent++) {
    
    const std::string& fileName = event_files[fileIdx];

    fileIdx++;
    if(fileIdx >= (int)event_files.size()) {
      fileIdx = 0;
    }
  
    std::cout<<"reading event from file "<<fileName<<std::endl;

    bSize = pB->load(fileName);
  
    TrigAccel::SEED_MAKING_JOB* pJ = reinterpret_cast<TrigAccel::SEED_MAKING_JOB*>(pB->m_buffer + bufferOffset);

    TrigAccel::SPACEPOINT_STORAGE& sps = pJ->m_data;

    tbb::tick_count tzero = tbb::tick_count::now();

    std::shared_ptr<TrigAccel::OffloadBuffer> pBuff = std::make_shared<TrigAccel::OffloadBuffer>(pB);

    TrigAccel::Work* pJob = pW->createWork(TrigAccel::InDetJobControlCode::MAKE_SEEDS, pBuff);
    
    if(!pJob) {
      std::cout<<"ERROR: cannot create work item"<<std::endl;
      exit(-3);
    }

    pJob->run();

    tbb::tick_count tnow=tbb::tick_count::now();
    tbb::tick_count::interval_t duration = tnow - tzero;
    std::cout<<"triplet making took "<<duration.seconds()*1000.0<<" ms"<<std::endl;

    std::shared_ptr<TrigAccel::OffloadBuffer> pOB = pJob->getOutput();

    TrigAccel::OUTPUT_SEED_STORAGE* pOutput = reinterpret_cast<TrigAccel::OUTPUT_SEED_STORAGE *>(pOB->m_rawBuffer);

    std::cout<<"Found "<<pOutput->m_nSeeds<<" triplets"<<std::endl;
    timeFile<<sps.m_nSpacepoints<<","<<pOutput->m_nSeeds<<","<<duration.seconds()*1000.0<<std::endl;    

    delete pJob;
  }
  
  timeFile.close();

  delete pB;
 
  deleteFactory(pW);

  dlclose(handle);

  

}
