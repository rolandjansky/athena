/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_DetectorTool.h"
#include "LUCID_DetectorFactory.h" 
#include "LUCID_GeoModel/LUCID_DetectorManager.h" 

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"

#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

LUCID_DetectorTool::LUCID_DetectorTool(const std::string& type, 
				       const std::string& name, 
				       const IInterface* parent): 
  GeoModelTool(type, name, parent),
  m_manager(0)
{
}

LUCID_DetectorTool::~LUCID_DetectorTool() {}

StatusCode LUCID_DetectorTool::create() { 
  
  MsgStream log(msgSvc(), name()); 
  
  log << MSG::INFO << "Building LUCID geometry" << endmsg;
  
  IGeoDbTagSvc* geoDbTag{nullptr};
  StatusCode sc = service("GeoDbTagSvc",geoDbTag);
  if (sc.isFailure()) { log << MSG::FATAL << "Could not locate GeoDbTagSvc" << endmsg; return StatusCode::FAILURE; }

  IRDBAccessSvc* raccess = 0;
  
  sc = service("RDBAccessSvc",raccess);
  
  if(sc.isFailure()) { log << MSG::FATAL << "Could not locate RDBAccessSvc" << endmsg; return StatusCode::FAILURE; }

  std::string AtlasVersion = geoDbTag->atlasVersion();
  std::string LucidVersion = raccess->getChildTag("LUCID",AtlasVersion,"ATLAS");

  if(LucidVersion.empty()) {
    log << MSG::DEBUG << "LUCID is not part of the selected ATLAS geometry. Skipping" << endmsg;
    return StatusCode::SUCCESS;
  }

  GeoModelExperiment* theExpt; 

  if (StatusCode::SUCCESS != detStore()->retrieve(theExpt, "ATLAS")) { 
    
    log << MSG::ERROR << "Could not find GeoModelExperiment ATLAS" << endmsg; 
    return StatusCode::FAILURE; 
  } 
  
  if(0 == m_detector) {

    GeoPhysVol* world = &*theExpt->getPhysVol();
        
    LUCID_DetectorFactory theLUCID_Factory(detStore().operator->(),raccess);

    theLUCID_Factory.create(world);
    m_manager = theLUCID_Factory.getDetectorManager();
    theExpt->addManager(m_manager);

    sc = detStore()->record(m_manager,
			  m_manager->getName());
    
    if (sc.isFailure()) { log << MSG::ERROR << "Could not register LUCID detector manager" << endmsg; return StatusCode::FAILURE; }
    
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::FAILURE;
}

StatusCode LUCID_DetectorTool::clear() {

  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<LUCID_DetectorManager>::ID(),m_manager->getName());

  if(proxy) {
    proxy->reset();
    m_manager = 0;
  }
  
  proxy = detStore()->proxy(ClassID_traits<GeoBorderSurfaceContainer>::ID(), "LUCID", false);
  
  if(proxy) {
    proxy->reset();
  }

  return StatusCode::SUCCESS;
}
