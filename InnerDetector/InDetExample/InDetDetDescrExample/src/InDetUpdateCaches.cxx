/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetDetDescrExample/InDetUpdateCaches.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "StoreGate/StoreGateSvc.h"


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
  msg(MSG::INFO) << "initialize()" << endreq;  
  StatusCode sc;
  // Retrieve GeoModel managers
  const InDetDD::SiDetectorManager * sctManager = 0;
  sc=detStore()->retrieve(sctManager, "SCT");
  if (sc.isFailure() || !sctManager) {
    msg(MSG::WARNING) << "Could not find the SCT_DetectorManager" << endreq;
  } else {
    msg(MSG::DEBUG) << "SCT_DetectorManager found" << endreq;
    m_detManagers.push_back(sctManager);
  }
  const InDetDD::SiDetectorManager * pixelManager = 0;
  sc=detStore()->retrieve(pixelManager, "Pixel");
  if (sc.isFailure() || !pixelManager) {
    msg(MSG::WARNING) << "Could not find the PixelDetectorManager" << endreq;
  } else {
    msg(MSG::DEBUG) << "PixelDetectorManager found" << endreq;
    m_detManagers.push_back(pixelManager);
 }
  const InDetDD::TRT_DetectorManager * trtManager = 0;
  sc=detStore()->retrieve(trtManager, "TRT");
  if (sc.isFailure() || !trtManager) {
    msg(MSG::WARNING) << "Could not find the TRT_DetectorManager" << endreq;
  } else {
    msg(MSG::DEBUG) << "TRT_DetectorManager found" << endreq;
    m_detManagers.push_back(trtManager);
  }
  if (m_inInit) {
    msg(MSG::DEBUG) << "Updating caches during initialize" << endreq;
    updateAll();
  }
  return StatusCode::SUCCESS;
}


StatusCode 
InDetUpdateCaches::execute() {
  if (m_first) {
    if (m_inExec) {
      msg(MSG::DEBUG) << "Updating caches during first execute" << endreq;
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
  msg(MSG::DEBUG) << "Updating ..." << endreq;
  IChronoStatSvc* chrono = chronoSvc();
  for (unsigned int i = 0; i <  m_detManagers.size(); i++) {
    const InDetDD::InDetDetectorManager * manager =  m_detManagers[i];
    msg(MSG::DEBUG) << "manager ..." << manager << endreq;
    if (manager) {
      std::string chronoTag =  manager->getName() + name();
      float memStart = getMem();
      chrono->chronoStart( chronoTag );
      msg(MSG::DEBUG) << "Updating " << manager->getName() << " caches ..." << endreq;
      manager->updateAll();
      msg(MSG::DEBUG) << "... done " << endreq;
      chrono->chronoStop(chronoTag);
      float memUsed = getMem() - memStart;      
      float uTime = chrono->chronoDelta(chronoTag, IChronoStatSvc::USER)/1000.;
      msg(MSG::DEBUG) << "Memory used by UpdatedCaches for  " << manager->getName() << " = " << memUsed/1024.0 << " MB" << endreq;
      msg(MSG::DEBUG) << "Time   used by UpdatedCaches for  " << manager->getName() << " = " << uTime << " ms" << endreq;
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
