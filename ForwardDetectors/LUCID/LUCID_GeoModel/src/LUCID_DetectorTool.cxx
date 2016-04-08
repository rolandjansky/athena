/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_GeoModel/LUCID_DetectorTool.h"
#include "LUCID_GeoModel/LUCID_DetectorFactory.h" 
#include "LUCID_GeoModel/LUCID_DetectorManager.h" 

#include "GeoModelInterfaces/IGeoModelSvc.h"
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

StatusCode LUCID_DetectorTool::create(StoreGateSvc* detStore) { 
  
  MsgStream log(msgSvc(), name()); 
  
  log << MSG::INFO << "Building LUCID geometry" << endreq;
  
  IGeoModelSvc* geoModel;

  StatusCode sc = service("GeoModelSvc",geoModel);
  
  if (sc.isFailure()) { log << MSG::FATAL << "Could not locate GeoModelSvc" << endreq; return StatusCode::FAILURE; }
  
  IRDBAccessSvc* raccess = 0;
  
  sc = service("RDBAccessSvc",raccess);
  
  if(sc.isFailure()) { log << MSG::FATAL << "Could not locate RDBAccessSvc" << endreq; return StatusCode::FAILURE; }

  std::string AtlasVersion = geoModel->atlasVersion();
  std::string LucidVersion = raccess->getChildTag("LUCID",AtlasVersion,"ATLAS");

  if(LucidVersion.empty()) {
    log << MSG::DEBUG << "LUCID is not part of the selected ATLAS geometry. Skipping" << endreq;
    return StatusCode::SUCCESS;
  }

  GeoModelExperiment* theExpt; 

  if (StatusCode::SUCCESS != detStore->retrieve(theExpt, "ATLAS")) { 
    
    log << MSG::ERROR << "Could not find GeoModelExperiment ATLAS" << endreq; 
    return StatusCode::FAILURE; 
  } 
  
  if(0 == m_detector) {

    GeoPhysVol* world = &*theExpt->getPhysVol();
        
    LUCID_DetectorFactory theLUCID_Factory(detStore,raccess);

    theLUCID_Factory.create(world);
    m_manager = theLUCID_Factory.getDetectorManager();
    theExpt->addManager(m_manager);

    sc = detStore->record(m_manager,
			  m_manager->getName());
    
    if (sc.isFailure()) { log << MSG::ERROR << "Could not register LUCID detector manager" << endreq; return StatusCode::FAILURE; }
    
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::FAILURE;
}

StatusCode LUCID_DetectorTool::clear(StoreGateSvc* detStore) {

  SG::DataProxy* _proxy = detStore->proxy(ClassID_traits<LUCID_DetectorManager>::ID(),m_manager->getName());

  if(_proxy) {
    _proxy->reset();
    m_manager = 0;
  }
  
  _proxy = detStore->proxy(ClassID_traits<GeoBorderSurfaceContainer>::ID(), "LUCID", false);
  
  if(_proxy) {
    _proxy->reset();
  }

  return StatusCode::SUCCESS;
}
