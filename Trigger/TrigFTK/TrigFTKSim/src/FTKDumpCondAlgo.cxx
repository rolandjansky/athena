/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKDumpCondAlgo.h"
//#include "TrigFTKSim/FTKPMap.h"
//#include "TrigFTKSim/FTKSetup.h"
//#include "TrigFTKSim/FTKRoadFileOutput.h"
//#include "TrigFTKSim/FTK_SGRoadOutput.h"
//#include "TrigFTKSim/FTK_RegionalRawInput.h"

#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
FTKDumpCondAlgo::FTKDumpCondAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_detectorTool("FTKDetectorTool/FTKDetectorTool"),
  m_DumpBadModules(false)
{
  // number of banks
  declareProperty("DumpBadModules",m_DumpBadModules, "If true enable dump of bad modules for FTK");
}

FTKDumpCondAlgo::~FTKDumpCondAlgo()
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKDumpCondAlgo::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKDumpCondAlgo::initialize()" << endreq;
  
  // select how the input is obtained
  if (m_DumpBadModules) {
    // Use the SG to retrieve the hits, this also means other Athena tools can be used
    StatusCode scdet = m_detectorTool.retrieve();
    if (scdet.isFailure()) {
      log << MSG::FATAL << "Could not retrieve FTKDetectorTool tool" << endreq;
      return StatusCode::FAILURE;
    }
    else {
      log << MSG::INFO << "Setting FTKDetectorTool tool" << endreq;
      //      m_detectorTool->initialize();
      // connect the detector tool with the FTK maps
      //      m_detectorTool->setPlaneMap(m_pmap);
      //      m_detectorTool->setRegionMap(m_rmap);
      //      m_detectorTool->setSSMap(m_ssmap);
    }
  }

  //if (sc.isFailure()) {
  //  log << MSG::FATAL << "Unexpected problem during the initialization stage" << endreq;
  //  return StatusCode::FAILURE;
  //}

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKDumpCondAlgo::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKDumpCondAlgo::execute() start" << endreq;
  
  if (m_DumpBadModules) {
    m_detectorTool->makeBadModuleMap(); //Dump bad SS map
    m_detectorTool->dumpDeadModuleSummary(); //Dump bad module map 
  }

  return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKDumpCondAlgo::finalize() {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finalize()" << endreq;
    
    return StatusCode::SUCCESS;
}
