/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamPipeDetectorTool.h"
#include "BeamPipeDetectorFactory.h" 
#include "BeamPipeGeoModel/BeamPipeDetectorManager.h" 

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
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


StatusCode BeamPipeDetectorTool::create()
{ 
  MsgStream log(msgSvc(), name()); 

  log << MSG::INFO << "Building Beam Pipe" << endmsg;

  IGeoDbTagSvc *geoDbTag;
  StatusCode sc = service ("GeoDbTagSvc",geoDbTag);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not locate GeoDbTagSvc" << endmsg;
    return StatusCode::FAILURE;
  }
    

  std::string atlasVersion = geoDbTag->atlasVersion();
  if(atlasVersion == "AUTO")
    atlasVersion = "ATLAS-00";
  std::string versionNode = "ATLAS";

  GeoModelExperiment * theExpt; 
  if (StatusCode::SUCCESS != detStore()->retrieve( theExpt, "ATLAS" )) { 
    log << MSG::ERROR 
	<< "Could not find GeoModelExperiment ATLAS" 
	<< endmsg; 
    return (StatusCode::FAILURE); 
  } 

  if ( 0 == m_detector ) 
  {
    GeoPhysVol *world=&*theExpt->getPhysVol();

    IRDBAccessSvc* raccess = 0;
    sc = service("RDBAccessSvc",raccess);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Could not locate RDBAccessSvc" << endmsg;
      return StatusCode::FAILURE;
    }

    // Check we have the beampipe and print its version
    // Print the  version tag:
    std::string beampipeVersionTag;
    beampipeVersionTag = raccess->getChildTag("BeamPipe", atlasVersion,versionNode);
    log << MSG::DEBUG << "Beampipe Version: " << beampipeVersionTag << endmsg;


    if (beampipeVersionTag.empty()) { 
      log << MSG::INFO << "No BeamPipe Version. Beam pipe will not be built." << endmsg;
      
    } else {
      
      BeamPipeDetectorFactory theBeamPipeFactory(detStore().operator->(),raccess);
      theBeamPipeFactory.setTagNode(atlasVersion,versionNode);
      theBeamPipeFactory.create(world);

      m_manager = theBeamPipeFactory.getDetectorManager();
      theExpt->addManager(m_manager);
      sc = detStore()->record(m_manager,
			    m_manager->getName());
    

      if (sc.isFailure()) {
	log << MSG::ERROR << "Could not register BeamPipe detector manager" << endmsg;
	return (StatusCode::FAILURE); 
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode BeamPipeDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<BeamPipeDetectorManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}
