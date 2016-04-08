/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_GeoModel/TRT_DetectorTool.h"
#include "TRT_GeoModel/TRTDetectorFactory_Full.h" 

#include "GeoModelUtilities/GeoModelExperiment.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "DetDescrConditions/AlignableTransformContainer.h"
#include "TRT_ConditionsData/StrawDxContainer.h" 

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Please consult the README for more information about which options to set in your joboptions file. //
////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////// Constructor //////////////////////////////////
//
TRT_DetectorTool::TRT_DetectorTool( const std::string& type, const std::string& name, const IInterface* parent )
  : GeoModelTool( type, name, parent ), 
    m_initialLayout(true),
    m_geoModelSvc("GeoModelSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_geometryDBSvc("InDetGeometryDBSvc",name),
    m_sumSvc("TRT_StrawStatusSummarySvc", name),
    m_doArgonMixture(0),
    m_doKryptonMixture(0),
    m_manager(0),
    m_athenaComps(0)
{
  declareProperty("UseOldActiveGasMixture", m_useOldActiveGasMixture = false );
  declareProperty("DC2CompatibleBarrelCoordinates",  m_DC2CompatibleBarrelCoordinates = false );
  declareProperty("OverrideDigVersion",  m_overridedigversion = -999 );
  declareProperty("Alignable", m_alignable = true);
  declareProperty("RDBAccessSvc", m_rdbAccessSvc);
  declareProperty("GeoModelSvc", m_geoModelSvc);
  declareProperty("GeometryDBSvc", m_geometryDBSvc);
  declareProperty("InDetTRTStrawStatusSummarySvc", m_sumSvc);  // need for Argon
  declareProperty("DoXenonArgonMixture", m_doArgonMixture); // Set to 1 to use argon. DEFAULT VALUE is 0. Overridden by DOARGONMIXTURE switch
  declareProperty("DoKryptonMixture", m_doKryptonMixture); // Set to 1 to use krypton. DEFAULT VALUE is 0. Overridden by DOKRYPTONMIXTURE switch
}



/////////////////////////////////// Destructor ///////////////////////////////////
//
TRT_DetectorTool::~TRT_DetectorTool() 
{
  delete m_athenaComps;
}


//////////////  Create the Detector Node corresponding to this tool //////////////
//
StatusCode TRT_DetectorTool::create( StoreGateSvc* detStore )
{ 
  //MsgStream log(msgSvc(), name()); 
  //msg(MSG::INFO) << " hello " << endreq;


  // Get the detector configuration.
  StatusCode sc = m_geoModelSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endreq;
    return (StatusCode::FAILURE); 
  }  
 
  DecodeVersionKey versionKey(&*m_geoModelSvc, "TRT");

  // Unless we are using custom trt, the switch positions are going to
  // come from the database:
  msg(MSG::INFO) << "Building TRT with Version Tag: "<< versionKey.tag() << " at Node: " << versionKey.node() << endreq;

  
  sc = m_rdbAccessSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endreq;
    return (StatusCode::FAILURE); 
  }  
 
  // Print the TRT version tag:
  std::string trtVersionTag = m_rdbAccessSvc->getChildTag("TRT", versionKey.tag(), versionKey.node(), false);
  msg(MSG::INFO) << "TRT Version: " << trtVersionTag << "  Package Version: " << PACKAGE_VERSION << endreq;
 

  // Check if version is empty. If so, then the TRT cannot be built. This may or may not be intentional. We
  // just issue an INFO message. 
  if (trtVersionTag.empty()) { 
     msg(MSG::INFO) << "No TRT Version. TRT will not be built." << endreq;
     return StatusCode::SUCCESS;
  }

  std::string versionName;
  if (versionKey.custom()) {
    
    msg(MSG::WARNING) << "TRT_DetectorTool:  Detector Information coming from a custom configuration!!" << endreq;
 
  } else {
    msg(MSG::DEBUG) << "TRT_DetectorTool:  Detector Information coming from the database and job options IGNORED." << endreq;
    
    msg(MSG::DEBUG) << "Keys for TRT Switches are "  << versionKey.tag()  << "  " << versionKey.node() << endreq;
    IRDBRecordset_ptr switchSet =  m_rdbAccessSvc->getRecordsetPtr("TRTSwitches", versionKey.tag(), versionKey.node());
    const IRDBRecord    *switches   = (*switchSet)[0];
    
    //Should be stored as booleans?
    if (switches->getInt("DC1COMPATIBLE")) {
      msg(MSG::ERROR) << "DC1COMPATIBLE flag set in database,"
	  << " but DC1 is no longer supported in the code!!" << endreq;
    }
    m_DC2CompatibleBarrelCoordinates = switches->getInt("DC2COMPATIBLE");
    m_useOldActiveGasMixture         	= ( switches->getInt("GASVERSION") == 0 );
    m_initialLayout                  	= switches->getInt("INITIALLAYOUT"); 


	// Check if the new switches exists:
    //bool result = true;
    if ((m_doArgonMixture == 1) ||( m_doKryptonMixture == 1) ){
     try {
      if(!switches->isFieldNull( "DOARGONMIXTURE")) {
        if      ( switches->getInt("DOARGONMIXTURE") == 0) m_doArgonMixture = 0;
        else if ( switches->getInt("DOARGONMIXTURE") == 1) m_doArgonMixture = 1;
      } else {
        if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Parameter DOARGONMIXTURE not available, m_doArgonMixture= " << m_doArgonMixture << endreq;
      }

      if(!switches->isFieldNull( "DOKRYPTONMIXTURE")) {
        if      ( switches->getInt("DOKRYPTONMIXTURE") == 0) m_doKryptonMixture = 0;
        else if ( switches->getInt("DOKRYPTONMIXTURE") == 1) m_doKryptonMixture = 1;
      } else {
        if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Parameter DOKRYPTONMIXTURE not available, m_doKryptonMixture= " << m_doKryptonMixture << endreq;
      }
     }
      catch(std::runtime_error& ex) {
       if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Exception caught: " << ex.what() << endreq;
       // result = false;
      }
    }
    if (!switches->isFieldNull("VERSIONNAME")) {
      versionName                    	= switches->getString("VERSIONNAME");
    }
  };

  // Set version name if it empty. This is only needed for preDC3 geometries.
  if (versionName.empty()) {
    if (m_DC2CompatibleBarrelCoordinates) {
      versionName  = "DC2";
    } else {
      versionName = "Rome";
    }
  }
  msg(MSG::INFO)  << "Creating the TRT" << endreq;
  msg(MSG::INFO)  << "TRT Geometry Options:" << endreq;
  msg(MSG::INFO)  << "  UseOldActiveGasMixture         = " << (m_useOldActiveGasMixture 	? "true" : "false") <<endreq;
  msg(MSG::INFO)  << "  Do Argon    = " << (m_doArgonMixture   ? "true" : "false") <<endreq;
  msg(MSG::INFO)  << "  Do Krypton  = " << (m_doKryptonMixture ? "true" : "false") <<endreq;
  msg(MSG::INFO)  << "  DC2CompatibleBarrelCoordinates = " << (m_DC2CompatibleBarrelCoordinates ? "true" : "false") <<endreq;
  msg(MSG::INFO)  << "  InitialLayout                  = " << (m_initialLayout ? "true" : "false") <<endreq;
  msg(MSG::INFO)  << "  Alignable                      = " << (m_alignable ? "true" : "false") <<endreq;
  msg(MSG::INFO)  << "  VersioName                     = " << versionName  <<endreq;
   
  // Retrieve the Geometry DB Interface
  sc = m_geometryDBSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate Geometry DB Interface: " << m_geometryDBSvc.name() << endreq;
    return (StatusCode::FAILURE);
  } 

  // Pass athena services to factory, etc
  m_athenaComps = new InDetDD::AthenaComps("TRT_GeoModel");
  m_athenaComps->setDetStore(detStore);
  m_athenaComps->setGeoModelSvc(&*m_geoModelSvc);
  m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
  m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);
    
  
  // 
  // Locate the top level experiment node 
  // 
  GeoModelExperiment * theExpt; 
  if (StatusCode::SUCCESS != detStore->retrieve( theExpt, "ATLAS" )) { 
    msg(MSG::ERROR) 
	<< "Could not find GeoModelExperiment ATLAS" 
	<< endreq; 
    return (StatusCode::FAILURE); 
  } 

  if ( 0 == m_detector ) {
    GeoPhysVol *world = theExpt->getPhysVol();

    msg(MSG::INFO) << " Building TRT geometry from GeoModel factory TRTDetectorFactory_Full" << endreq;

    TRTDetectorFactory_Full theTRTFactory(m_athenaComps, 
					  m_sumSvc,
					  m_useOldActiveGasMixture,
					  m_DC2CompatibleBarrelCoordinates,
					  m_overridedigversion,
					  m_alignable,
					  m_doArgonMixture,
					  m_doKryptonMixture
    );
    theTRTFactory.create(world);
    m_manager=theTRTFactory.getDetectorManager();
    
    // Register the TRTDetectorNode instance with the Transient Detector Store
    if (m_manager) {
      theExpt->addManager(m_manager);
      
      sc = detStore->record(m_manager,m_manager->getName());
      if (sc.isFailure() ) {
	msg(MSG::ERROR) << "Could not register TRT_DetectorManager" << endreq;
	return( StatusCode::FAILURE );
      }
      
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}



StatusCode 
TRT_DetectorTool::registerCallback( StoreGateSvc* detStore)
{

  MsgStream log(msgSvc(), name());

  // If we fail to register any callbacks we return FAILURE. This just tells GeoModelSvc that 
  // no callbacks were registered. It will continue normally but without any alignments.
  StatusCode sc = StatusCode::FAILURE;

  if (m_alignable) {

    // Regular alignment new shema   
    {
      std::string folderName = "/TRT/AlignL1/TRT";
      if (detStore->contains<CondAttrListCollection>(folderName)) {
        msg(MSG::DEBUG) << "Registering callback on global Container with folder " << folderName << endreq;
        const DataHandle<CondAttrListCollection> calc;
        StatusCode trttmp = detStore->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), calc, folderName);
        // We don't expect this to fail as we have already checked that the detstore contains the object. 
        if (trttmp.isFailure()) {
          msg(MSG::ERROR) << "Problem when register callback on global Container with folder " << folderName <<endreq;
        } else {
          sc =  StatusCode::SUCCESS;
        }
      } else {
        msg(MSG::WARNING) << "Unable to register callback on global Container with folder " << folderName <<endreq;
        //return StatusCode::FAILURE;
      }

      folderName = "/TRT/AlignL2";
      if (detStore->contains<AlignableTransformContainer>(folderName)) {
        if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Registering callback on AlignableTransformContainer with folder " << folderName << endreq;
        const DataHandle<AlignableTransformContainer> atc;
        StatusCode sctmp = detStore->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName);
        if(sctmp.isFailure()) {
          msg(MSG::ERROR) << "Problem when register callback on AlignableTransformContainer with folder " << folderName <<endreq;
        } else {
          sc =  StatusCode::SUCCESS;
        }
      }
      else {
	msg(MSG::WARNING) << "Unable to register callback on AlignableTransformContainer with folder "
                          << folderName <<  endreq;
        //return StatusCode::FAILURE;
      }
    }


    // Regular alignment
    {
      std::string folderName = "/TRT/Align";
      if (detStore->contains<AlignableTransformContainer>(folderName)) {
	msg(MSG::DEBUG) << "Registering callback on AlignableTransformContainer with folder " << folderName << endreq;
	const DataHandle<AlignableTransformContainer> atc;
	StatusCode sctmp = detStore->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName);
	// We don't expect this to fail as we have already checked that the detstore contains the object.
	if (sctmp.isFailure()) {
	  msg(MSG::ERROR) << "Problem when register callback on AlignableTransformContainer with folder " << folderName <<endreq;
	} else {
	  sc =  StatusCode::SUCCESS;
	}
      } else {
	msg(MSG::WARNING) << "Unable to register callback on AlignableTransformContainer with folder "
			  << folderName << ", Alignments disabled! (Only if no Run2 schema is loaded)" << endreq;
      }
    }

    // Fine alignment
    {
      std::string folderName = "/TRT/Calib/DX";
      if (detStore->contains<TRTCond::StrawDxContainer>(folderName)) {
        msg(MSG::DEBUG) << "Registering callback on StrawDxContainer with folder " << folderName << endreq;
        const DataHandle<TRTCond::StrawDxContainer> sdc;
	StatusCode sctmp = detStore->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), sdc, folderName);
	// We don't expect this to fail as we have already checked that the detstore contains the object.
	if (sctmp.isFailure()) {
	  msg(MSG::ERROR) << "Problem when register callback on StrawDxContainer with folder " << folderName <<endreq;
	} else {
	  sc =  StatusCode::SUCCESS;
	}
      } else {
        msg(MSG::DEBUG) << "Unable to register callback on StrawDxContainer with folder " << folderName <<endreq;
      }
    }    
   
  } else {
    msg(MSG::INFO) << "Alignment disabled. No callback registered" << endreq;
    // We return failure otherwise it will try and register
    // a GeoModelSvc callback associated with this callback.
  }

  return sc;
}

StatusCode TRT_DetectorTool::clear(StoreGateSvc* detStore)
{
  SG::DataProxy* _proxy = detStore->proxy(ClassID_traits<InDetDD::TRT_DetectorManager>::ID(),m_manager->getName());
  if(_proxy) {
    _proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}

  
StatusCode 
TRT_DetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  MsgStream log(msgSvc(), name()); 
  if (!m_manager) { 
    msg(MSG::WARNING) << "Manager does not exist" << endreq;
    return StatusCode::FAILURE;
  }    
  if (m_alignable) {     
    return m_manager->align(I,keys);
  } else {
    msg(MSG::DEBUG) << "Alignment disabled. No alignments applied" << endreq;
    return StatusCode::SUCCESS;
  }
}

