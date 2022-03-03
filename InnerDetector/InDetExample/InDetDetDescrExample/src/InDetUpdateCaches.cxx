/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetDetDescrExample/InDetUpdateCaches.h"

#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"



// for getmem
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>



InDetUpdateCaches::InDetUpdateCaches(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_first(true){  
  // Get parameter values from jobOptions file
  declareProperty("UpdateInInit", m_inInit = false);
  declareProperty("UpdateInExec", m_inExec = true);
}


StatusCode 
InDetUpdateCaches::initialize(){
  msg(MSG::INFO) << "initialize()" << endmsg;  
  StatusCode sc;
  // Retrieve GeoModel managers
  const InDetDD::SCT_DetectorManager * sctManager = nullptr;
  sc=detStore()->retrieve(sctManager, "SCT");
  if (sc.isFailure() || !sctManager) {
    msg(MSG::WARNING) << "Could not find the SCT_DetectorManager" << endmsg;
  } else {
    msg(MSG::DEBUG) << "SCT_DetectorManager found" << endmsg;
    m_detManagers.push_back(sctManager);
  }
  const InDetDD::PixelDetectorManager * pixelManager = nullptr;
  sc=detStore()->retrieve(pixelManager, "Pixel");
  if (sc.isFailure() || !pixelManager) {
    msg(MSG::WARNING) << "Could not find the PixelDetectorManager" << endmsg;
  } else {
    msg(MSG::DEBUG) << "PixelDetectorManager found" << endmsg;
    m_detManagers.push_back(pixelManager);
 }
  const InDetDD::TRT_DetectorManager * trtManager = nullptr;
  sc=detStore()->retrieve(trtManager, "TRT");
  if (sc.isFailure() || !trtManager) {
    msg(MSG::WARNING) << "Could not find the TRT_DetectorManager" << endmsg;
  } else {
    msg(MSG::DEBUG) << "TRT_DetectorManager found" << endmsg;
    m_detManagers.push_back(trtManager);
  }
  if (m_inInit) {
    msg(MSG::DEBUG) << "Updating caches during initialize" << endmsg;
    updateAll();
  }
  return StatusCode::SUCCESS;
}


StatusCode 
InDetUpdateCaches::execute() {
  if (m_first) {
    if (m_inExec) {
      msg(MSG::DEBUG) << "Updating caches during first execute" << endmsg;
      updateAll();
    }
    m_first = false;
  }
  return StatusCode::SUCCESS;
}


StatusCode 
InDetUpdateCaches::finalize() {
  return StatusCode::SUCCESS;
}


void 
InDetUpdateCaches::updateAll() {
  msg(MSG::DEBUG) << "Updating ..." << endmsg;
  IChronoStatSvc* chrono = chronoSvc();
  for (unsigned int i = 0; i <  m_detManagers.size(); i++) {
    const InDetDD::InDetDetectorManager * manager =  m_detManagers[i];
    msg(MSG::DEBUG) << "manager ..." << manager << endmsg;
    if (manager) {
      std::string chronoTag =  manager->getName() + name();
      float memStart = getMem();
      chrono->chronoStart( chronoTag );
      msg(MSG::DEBUG) << "Updating " << manager->getName() << " caches ..." << endmsg;
      manager->updateAll();
      msg(MSG::DEBUG) << "... done " << endmsg;
      chrono->chronoStop(chronoTag);
      float memUsed = getMem() - memStart;      
      float uTime = chrono->chronoDelta(chronoTag, IChronoStatSvc::USER)/1000.;
      msg(MSG::DEBUG) << "Memory used by UpdatedCaches for  " << manager->getName() << " = " << memUsed/1024.0 << " MB" << endmsg;
      msg(MSG::DEBUG) << "Time   used by UpdatedCaches for  " << manager->getName() << " = " << uTime << " ms" << endmsg;
    }
  }
}


float
InDetUpdateCaches::getMem() {
  int pid=getpid();
  std::ostringstream is;
  is<<pid;
  std::string spid=is.str();
  std::string temp="cat /proc/"+spid+"/status | grep VmSize > mem.txt";
  system(temp.c_str());
  std::ifstream in("mem.txt");
  std::string text,mem,text1;
  in>>text>>mem>>text1;
  system("rm -f mem.txt");
  float memsize=atof(mem.c_str());
  return memsize;
}
