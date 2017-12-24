/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetectorToolH62004.h"
#include "LArDetectorFactoryH62004.h" 

#include "LArG4RunControl/LArGeoTB2004Options.h"
#include "LArReadoutGeometry/HECDetectorManager.h"

#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
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
LArDetectorToolH62004::LArDetectorToolH62004( const std::string& type, const std::string& name, const IInterface* parent )
: GeoModelTool( type, name, parent )
{
   declareProperty("XCryoPos",m_xcryo=0.);
   declareProperty("YTablePos",m_ytable=0.);
   declareProperty("isRun1",m_isrun1=false);
   declareProperty("isEmec",m_emec=true);
   declareProperty("isHec",m_hec=true);
   declareProperty("isFcal",m_fcal=true);
   declareProperty("isColdnose",m_coldnose=true);
   declareProperty("PrintStep",m_printstep=false);
   declareProperty("CheckPrim",m_checkprim=false);
   declareProperty("CheckOther",m_checkother=false);
}

/**
 ** Destructor
 **/
LArDetectorToolH62004::~LArDetectorToolH62004()
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
LArDetectorToolH62004::create()
{ 
  MsgStream log(msgSvc(), name()); 

  LArGeoTB2004Options *geotbopt = new LArGeoTB2004Options();
  geotbopt->CryoXPosition(m_xcryo);
  geotbopt->TableYPosition(m_ytable);
  geotbopt->Run1(m_isrun1);
  geotbopt->Emec(m_emec);
  geotbopt->Hec(m_hec);
  geotbopt->Fcal(m_fcal);
  geotbopt->Coldnose(m_coldnose);
  geotbopt->Printstep(m_printstep);
  geotbopt->Checkprim(m_checkprim);
  geotbopt->Checkother(m_checkother);
  geotbopt->saveMe();



  // Get the detector configuration.
  IGeoDbTagSvc *geoDbTag;
  StatusCode sc = service ("GeoDbTagSvc",geoDbTag);
  if (StatusCode::SUCCESS != sc) {
    log << MSG::ERROR << "Could not find GeoDbTagSvc" << endmsg;
    return (StatusCode::FAILURE);
  }

  std::string AtlasVersion = geoDbTag->atlasVersion();
  std::string LArVersion   = geoDbTag->LAr_VersionOverride();

  IRDBAccessSvc *accessSvc;
  sc=service("RDBAccessSvc",accessSvc);
  if (StatusCode::SUCCESS != sc) { 
    log << MSG::ERROR 
	<< "Could not find RDBAccessSvc" 
	<< endmsg; 
    return (StatusCode::FAILURE); 
  } 

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  log << MSG::INFO << "Keys for LAr are "  << detectorKey  << "  " << detectorNode << endmsg;


  log << MSG::INFO  << "Creating the LAr " << endmsg;
  log << MSG::INFO  << "LAr Geometry Options:"   << endmsg;




  // 
  // Locate the top level experiment node 
  // 
  DataHandle<GeoModelExperiment> theExpt; 
  sc=detStore()->retrieve( theExpt, "ATLAS" );
  if (StatusCode::SUCCESS != sc) { 
    log << MSG::ERROR 
	<< "Could not find GeoModelExperiment ATLAS" 
	<< endmsg; 
    return (StatusCode::FAILURE); 
  } 


  // determine the geometry layout - Atlas/Testbeam
  std::string geometryLayout = "Atlas";
  std::string LArTag = accessSvc->getChildTag("LAr",detectorKey,detectorNode);

  //if(LArTag.find("H6")!=std::string::npos) {
  //  geometryLayout = "H6";
  //}


  LArGeo::LArDetectorFactoryH62004 theLArFactory(detStore().operator->());





  if ( 0 == m_detector ) {
    // Create the H62004Node instance
    try {   
      //
      // This strange way of casting is to avoid an
      // utterly brain damaged compiler warning.
      //
      GeoPhysVol *world=&*theExpt->getPhysVol();
      theLArFactory.create(world);
    } catch (const std::bad_alloc&) {
      log << MSG::FATAL << "Could not create new H62004Node!" << endmsg;
      return StatusCode::FAILURE; 
    }
    // Register the H62004Node instance with the Transient Detector Store
    theExpt->addManager(theLArFactory.getDetectorManager());
    sc=detStore()->record(theLArFactory.getDetectorManager(),theLArFactory.getDetectorManager()->getName());
  if (StatusCode::SUCCESS != sc) { 
    log << MSG::ERROR 
	<< "Could not record LArFactory to DetStore" 
	<< endmsg; 
    return (StatusCode::FAILURE); 
  } 


    return StatusCode::SUCCESS;


  }


  return StatusCode::FAILURE;
}
