/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62002Algs/LArDetectorToolH62002.h"
#include "LArGeoH62002Algs/LArDetectorFactoryH62002.h" 

#include "LArReadoutGeometry/HECDetectorManager.h"


#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"


/**
 ** Constructor(s)
 **/
LArDetectorToolH62002::LArDetectorToolH62002( const std::string& type, const std::string& name, const IInterface* parent )
: GeoModelTool( type, name, parent )
{
}

/**
 ** Destructor
 **/
LArDetectorToolH62002::~LArDetectorToolH62002()
{
	// This will need to be modified once we register the Toy DetectorNode in
	// the Transient Detector Store
	if ( 0 != m_detector ) {
		delete m_detector;
		m_detector = 0;
	}
}

/**
 ** Create the Detector Node corresponding to this tool
 **/
StatusCode
LArDetectorToolH62002::create( StoreGateSvc* detStore )
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




  // 
  // Locate the top level experiment node 
  // 
  DataHandle<GeoModelExperiment> theExpt; 
  if (StatusCode::SUCCESS != detStore->retrieve( theExpt, "ATLAS" )) { 
    log << MSG::ERROR 
	<< "Could not find GeoModelExperiment ATLAS" 
	<< endreq; 
    return (StatusCode::FAILURE); 
  } 


  // determine the geometry layout - Atlas/Testbeam
  std::string geometryLayout = "Atlas";
  std::string LArTag = accessSvc->getChildTag("LAr",detectorKey,detectorNode);

  //if(LArTag.find("H6")!=std::string::npos) {
  //  geometryLayout = "H6";
  //}


  LArGeo::LArDetectorFactoryH62002 theLArFactory(detStore);





  if ( 0 == m_detector ) {
    // Create the H62002Node instance
    try {   
      //
      // This strange way of casting is to avoid an
      // utterly brain damaged compiler warning.
      //
      GeoPhysVol *world=&*theExpt->getPhysVol();
      theLArFactory.create(world);
    } catch (std::bad_alloc) {
      log << MSG::FATAL << "Could not create new H62002Node!" << endreq;
      return StatusCode::FAILURE; 
    }
    // Register the H62002Node instance with the Transient Detector Store
    theExpt->addManager(theLArFactory.getDetectorManager());
    detStore->record(theLArFactory.getDetectorManager(),theLArFactory.getDetectorManager()->getName());


    return StatusCode::SUCCESS;


  }


  return StatusCode::FAILURE;
}
