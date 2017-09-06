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

#include "TrigFTKSim/FTKSetup.h"

#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
FTKDumpCondAlgo::FTKDumpCondAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_detectorTool("FTKDetectorTool/FTKDetectorTool"),
  m_DumpBadModules(false),
  m_DumpModuleIDMap(false),
  m_DumpGlobalToLocalMap(false),
  m_DumpIDMap(false),
  m_DumpModulePositions(false),
  m_IBLMode(0),
  m_ITkMode(false),
  m_fixEndcapL0(false)
{
  declareProperty("DumpBadModules", m_DumpBadModules, "If true enable dump of bad modules for FTK");
  declareProperty("DumpModuleIDMap", m_DumpModuleIDMap, "If true dumps the map of the modules in each tower");
  declareProperty("DumpGlobalToLocalMap",m_DumpGlobalToLocalMap, "True if you want to produce the Global-to-Local map");
  declareProperty("DumpIDMap",m_DumpIDMap);
  declareProperty("DumpModulePositions",m_DumpModulePositions,"To dump the corner positions of the modules");
  declareProperty("IBLMode",m_IBLMode);
  declareProperty("ITkMode",m_ITkMode,"True if you want to assume ITk geometry for FTK plane/tower maps");
  declareProperty("FixEndCapL0",m_fixEndcapL0, "Fix endcap L0 clustering");
}

FTKDumpCondAlgo::~FTKDumpCondAlgo()
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKDumpCondAlgo::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKDumpCondAlgo::initialize()" << endmsg;
  
  // FTK library global setup variables
  FTKSetup::getFTKSetup().setIBLMode(m_IBLMode);
  FTKSetup::getFTKSetup().setITkMode(m_ITkMode);
  FTKSetup::getFTKSetup().setfixEndcapL0(m_fixEndcapL0);

  // select how the input is obtained
  if (m_DumpBadModules || m_DumpGlobalToLocalMap) {
    // Use the SG to retrieve the hits, this also means other Athena tools can be used
    StatusCode scdet = m_detectorTool.retrieve();
    if (scdet.isFailure()) {
      log << MSG::FATAL << "Could not retrieve FTKDetectorTool tool" << endmsg;
      return StatusCode::FAILURE;
    }
    else {
      log << MSG::INFO << "Setting FTKDetectorTool tool" << endmsg;
      //      m_detectorTool->initialize();
      // connect the detector tool with the FTK maps
      //      m_detectorTool->setPlaneMap(m_pmap);
      //      m_detectorTool->setRegionMap(m_rmap);
      //      m_detectorTool->setSSMap(m_ssmap);
    }
  }

  //if (sc.isFailure()) {
  //  log << MSG::FATAL << "Unexpected problem during the initialization stage" << endmsg;
  //  return StatusCode::FAILURE;
  //}

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKDumpCondAlgo::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKDumpCondAlgo::execute() start" << endmsg;
  
  if (m_DumpBadModules) {
    m_detectorTool->makeBadModuleMap(); //Dump bad SS map
    m_detectorTool->dumpDeadModuleSummary(); //Dump bad module map
    m_DumpBadModules = false; // in case you run on more than 1 event, only dump once
  }

  if (m_DumpGlobalToLocalMap) {
    m_detectorTool->dumpGlobalToLocalModuleMap(); // Dump the map of the module of each tower
    m_DumpGlobalToLocalMap=false;
  }

  if (m_DumpIDMap) {
    m_detectorTool->dumpIDMap();
    m_DumpIDMap = false; // in case you run on more than 1 event, only dump once
  }

  if(m_DumpModulePositions) {
     m_detectorTool->dumpModulePositions();
     m_DumpModulePositions=false;
  }

  return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKDumpCondAlgo::finalize() {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finalize()" << endmsg;
    
    return StatusCode::SUCCESS;
}
