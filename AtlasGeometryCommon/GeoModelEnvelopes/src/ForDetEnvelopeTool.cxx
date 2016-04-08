/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelEnvelopes/ForDetEnvelopeTool.h"
#include "GeoModelEnvelopes/ForDetEnvelopeFactory.h" 
#include "GeoModelEnvelopes/ForDetEnvelopeManager.h" 

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

ForDetEnvelopeTool::ForDetEnvelopeTool(const std::string& type, 
				       const std::string& name, 
				       const IInterface* parent): 
  GeoModelTool(type,name,parent),
  m_manager(0)
{
}

ForDetEnvelopeTool::~ForDetEnvelopeTool()
{
}


StatusCode ForDetEnvelopeTool::create(StoreGateSvc* detStore)
{ 
  MsgStream log(msgSvc(), name()); 

  log << MSG::INFO << "Building Forward Detectors Envelope" << endreq;

  IGeoModelSvc *geoModel;
  StatusCode sc = service ("GeoModelSvc",geoModel);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not locate GeoModelSvc" << endreq;
    return StatusCode::FAILURE;
  }

  GeoModelExperiment* theExpt; 
  if (StatusCode::SUCCESS != detStore->retrieve(theExpt,"ATLAS")) { 
    log << MSG::ERROR 
	<< "Could not find GeoModelExperiment ATLAS" 
	<< endreq; 
    return StatusCode::FAILURE; 
  } 

  if(0==m_manager) {
    GeoPhysVol *world=&*theExpt->getPhysVol();

    ForDetEnvelopeFactory theFactory(detStore);
    theFactory.create(world);

    m_manager = theFactory.getDetectorManager();
    theExpt->addManager(m_manager);
    sc = detStore->record(m_manager,
			  m_manager->getName());
    
    if (sc.isFailure()) {
      log << MSG::ERROR << "Could not register ForDetEnvelope detector manager" << endreq;
      return (StatusCode::FAILURE); 
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode ForDetEnvelopeTool::clear(StoreGateSvc* detStore)
{
  SG::DataProxy* _proxy = detStore->proxy(ClassID_traits<ForDetEnvelopeManager>::ID(),m_manager->getName());
  if(_proxy) {
    _proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}
