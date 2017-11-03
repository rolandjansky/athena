/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetectorToolTBEC.h"
#include "LArDetectorFactoryTBEC.h" 
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

LArDetectorToolTBEC::LArDetectorToolTBEC(const std::string& type, 
				 const std::string& name, 
				 const IInterface* parent)
  : GeoModelTool(type,name,parent), 
    m_ecVisLimit(-1)
{
  declareProperty("ECVisLimit",          m_ecVisLimit);
}

LArDetectorToolTBEC::~LArDetectorToolTBEC()
{
}

StatusCode LArDetectorToolTBEC::create()
{ 
  MsgStream log(msgSvc(), name()); 

  // Get the detector configuration.
  IGeoDbTagSvc *geoDbTag;
  service ("GeoDbTagSvc",geoDbTag);

  std::string AtlasVersion = geoDbTag->atlasVersion();
  std::string LArVersion   = geoDbTag->LAr_VersionOverride();

  IRDBAccessSvc *accessSvc;
  service("RDBAccessSvc",accessSvc);

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  log << MSG::INFO << "Keys for LAr are "  << detectorKey  << "  " << detectorNode << endmsg;


  log << MSG::INFO  << "Creating the LAr " << endmsg;
  log << MSG::INFO  << "LAr Geometry Options:"   << endmsg;

  // Locate the top level experiment node 
  DataHandle<GeoModelExperiment> theExpt; 
  if (StatusCode::SUCCESS != detStore()->retrieve( theExpt, "ATLAS" )) 
  { 
    log << MSG::ERROR << "Could not find GeoModelExperiment ATLAS" << endmsg; 
    return (StatusCode::FAILURE); 
  } 

  // determine the geometry layout - Atlas/Testbeam
  std::string geometryLayout = "Atlas";
  std::string LArTag = accessSvc->getChildTag("LAr",detectorKey,detectorNode);

  LArGeo::LArDetectorFactoryTBEC theLArFactory;

  theLArFactory.setECVisLimit        (m_ecVisLimit);


  if (0 == m_detector) 
  {
    GeoPhysVol *world=&*theExpt->getPhysVol();
    theLArFactory.create(world);

    if (StatusCode::SUCCESS != detStore()->record(theLArFactory.getDetectorManager(),theLArFactory.getDetectorManager()->getName())) 
      { 
	log << MSG::ERROR << "Could not record" << endmsg; 
	return (StatusCode::FAILURE); 
      } 

    theExpt->addManager(theLArFactory.getDetectorManager());


    return StatusCode::SUCCESS;
  }


  return StatusCode::FAILURE;
}
