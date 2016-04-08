/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamPipeGeoModel/BeamPipeDetectorTool.h"
#include "BeamPipeGeoModel/BeamPipeDetectorFactory.h" 
#include "BeamPipeGeoModel/BeamPipeDetectorManager.h" 

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

BeamPipeDetectorTool::BeamPipeDetectorTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent ): 
  GeoModelTool(type,name,parent),
  m_manager(0)
{
}

BeamPipeDetectorTool::~BeamPipeDetectorTool()
{
}


StatusCode BeamPipeDetectorTool::create( StoreGateSvc* detStore )
{ 
  MsgStream log(msgSvc(), name()); 

  log << MSG::INFO << "Building Beam Pipe" << endreq;

  IGeoModelSvc *geoModel;
  StatusCode sc = service ("GeoModelSvc",geoModel);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not locate GeoModelSvc" << endreq;
    return StatusCode::FAILURE;
  }
    

  std::string atlasVersion = geoModel->atlasVersion();
  if(atlasVersion == "AUTO")
    atlasVersion = "ATLAS-00";
  std::string versionNode = "ATLAS";

  GeoModelExperiment * theExpt; 
  if (StatusCode::SUCCESS != detStore->retrieve( theExpt, "ATLAS" )) { 
    log << MSG::ERROR 
	<< "Could not find GeoModelExperiment ATLAS" 
	<< endreq; 
    return (StatusCode::FAILURE); 
  } 

  if ( 0 == m_detector ) 
  {
    GeoPhysVol *world=&*theExpt->getPhysVol();

    IRDBAccessSvc* raccess = 0;
    sc = service("RDBAccessSvc",raccess);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Could not locate RDBAccessSvc" << endreq;
      return StatusCode::FAILURE;
    }

    // Check we have the beampipe and print its version
    // Print the  version tag:
    std::string beampipeVersionTag;
    beampipeVersionTag = raccess->getChildTag("BeamPipe", atlasVersion,versionNode, false);
    log << MSG::DEBUG << "Beampipe Version: " << beampipeVersionTag << endreq;


    if (beampipeVersionTag.empty()) { 
      log << MSG::INFO << "No BeamPipe Version. Beam pipe will not be built." << endreq;
      
    } else {
      
      BeamPipeDetectorFactory theBeamPipeFactory(detStore,raccess);
      theBeamPipeFactory.setTagNode(atlasVersion,versionNode);
      theBeamPipeFactory.create(world);

      m_manager = theBeamPipeFactory.getDetectorManager();
      theExpt->addManager(m_manager);
      sc = detStore->record(m_manager,
			    m_manager->getName());
    

      if (sc.isFailure()) {
	log << MSG::ERROR << "Could not register BeamPipe detector manager" << endreq;
	return (StatusCode::FAILURE); 
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode BeamPipeDetectorTool::clear(StoreGateSvc* detStore)
{
  SG::DataProxy* _proxy = detStore->proxy(ClassID_traits<BeamPipeDetectorManager>::ID(),m_manager->getName());
  if(_proxy) {
    _proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}
