/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoTBEC/LArDetectorToolTBEC.h"
#include "LArGeoTBEC/LArDetectorFactoryTBEC.h" 
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

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

StatusCode LArDetectorToolTBEC::create(StoreGateSvc* detStore)
{ 
  MsgStream log(msgSvc(), name()); 

  // Get the detector configuration.
  IGeoModelSvc *geoModel;
  service ("GeoModelSvc",geoModel);
  
  std::string AtlasVersion = geoModel->atlasVersion();
  std::string LArVersion   = geoModel->LAr_VersionOverride();
  
  IRDBAccessSvc *accessSvc;
  service("RDBAccessSvc",accessSvc);

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  log << MSG::INFO << "Keys for LAr are "  << detectorKey  << "  " << detectorNode << endreq;


  log << MSG::INFO  << "Creating the LAr " << endreq;
  log << MSG::INFO  << "LAr Geometry Options:"   << endreq;

  // Locate the top level experiment node 
  DataHandle<GeoModelExperiment> theExpt; 
  if (StatusCode::SUCCESS != detStore->retrieve( theExpt, "ATLAS" )) 
  { 
    log << MSG::ERROR << "Could not find GeoModelExperiment ATLAS" << endreq; 
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

    if (StatusCode::SUCCESS != detStore->record(theLArFactory.getDetectorManager(),theLArFactory.getDetectorManager()->getName())) 
      { 
	log << MSG::ERROR << "Could not record" << endreq; 
	return (StatusCode::FAILURE); 
      } 

    theExpt->addManager(theLArFactory.getDetectorManager());


    return StatusCode::SUCCESS;
  }


  return StatusCode::FAILURE;
}
