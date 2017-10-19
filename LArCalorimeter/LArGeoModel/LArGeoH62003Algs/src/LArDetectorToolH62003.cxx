/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetectorToolH62003.h"
#include "LArDetectorFactoryH62003.h" 
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

LArDetectorToolH62003::LArDetectorToolH62003(const std::string& type, 
				 const std::string& name, 
				 const IInterface* parent)
  : GeoModelTool(type,name,parent), 
    _fcalVisLimit(-1),
    _axisVisState(-1)
{
  declareProperty("FCALVisLimit",          _fcalVisLimit);
  declareProperty("axisVisState",          _axisVisState=false);
}

LArDetectorToolH62003::~LArDetectorToolH62003()
{
}

StatusCode LArDetectorToolH62003::create()
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

  //if(LArTag.find("H6")!=std::string::npos) {
  //  geometryLayout = "H6";
  //}
  LArGeo::LArDetectorFactoryH62003 theLArFactory;

  theLArFactory.setFCALVisLimit        (_fcalVisLimit);
  theLArFactory.setAxisVisState(_axisVisState);


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
