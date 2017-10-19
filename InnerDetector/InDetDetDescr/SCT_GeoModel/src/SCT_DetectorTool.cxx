/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_DetectorTool.h"
#include "SCT_GeoModel/SCT_DetectorFactory.h" 
#include "SCT_GeoModel/SCT_DetectorFactoryCosmic.h" 
#include "SCT_GeoModel/SCT_GeometryManager.h" 
#include "SCT_GeoModel/SCT_DataBase.h" 
#include "SCT_GeoModel/SCT_MaterialManager.h" 
#include "SCT_GeoModel/SCT_Options.h" 
#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h" 

#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "InDetCondServices/ISiLorentzAngleSvc.h"

#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

using InDetDD::SCT_DetectorManager; 
using InDetDD::SiDetectorManager; 

//
// Constructor
//
SCT_DetectorTool::SCT_DetectorTool( const std::string& type,
				    const std::string& name, 
				    const IInterface* parent )
  : GeoModelTool( type, name, parent ),
    m_detectorName("SCT"),
    m_initialLayout(false),
    m_alignable(true),
    m_cosmic(false),
    m_manager(0), 
    m_athenaComps(0),
    m_geoDbTagSvc("GeoDbTagSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_geometryDBSvc("InDetGeometryDBSvc",name),
    m_lorentzAngleSvc("SCTLorentzAngleSvc", name)
  
{
  // Get parameter values from jobOptions file
  declareProperty("DetectorName", m_detectorName);
  declareProperty("InitialLayout", m_initialLayout=false);
  declareProperty("Alignable", m_alignable);
  declareProperty("RDBAccessSvc", m_rdbAccessSvc);
  declareProperty("GeometryDBSvc", m_geometryDBSvc);
  declareProperty("GeoDbTagSvc", m_geoDbTagSvc);
  declareProperty("LorentzAngleSvc", m_lorentzAngleSvc);
}

//
// Destructor
//
SCT_DetectorTool::~SCT_DetectorTool()
{
  delete m_athenaComps;
}

//
// Create the Geometry via the factory corresponding to this tool
//

StatusCode
SCT_DetectorTool::create()
{ 

  StatusCode result = StatusCode::SUCCESS;

  // Reinit various singleton type objects.
  SCT_DataBase::reinit();
  SCT_MaterialManager::reinit();

  // Get the detector configuration.
  StatusCode sc = m_geoDbTagSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoDbTagSvc" << endmsg;
    return (StatusCode::FAILURE);
  } 
  
  DecodeVersionKey versionKey(&*m_geoDbTagSvc, "SCT");
  
  // Issue error if AUTO.
  if (versionKey.tag() == "AUTO"){
    msg(MSG::ERROR) << "AUTO Atlas version. Please select a version." << endmsg;
  }

 
  msg(MSG::INFO) << "Building SCT with Version Tag: "<< versionKey.tag() << " at Node: " << versionKey.node() << endmsg;

  sc = m_rdbAccessSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endmsg;
    return (StatusCode::FAILURE); 
  }  

  // Print the SCT version tag:
  std::string sctVersionTag;
  sctVersionTag =  m_rdbAccessSvc->getChildTag("SCT", versionKey.tag(), versionKey.node());
  msg(MSG::INFO) << "SCT Version: " << sctVersionTag <<  "  Package Version: " << PACKAGE_VERSION << endmsg;


  // Check if version is empty. If so, then the SCT cannot be built. This may or may not be intentional. We
  // just issue an INFO message. 
  if (sctVersionTag.empty()) { 
    msg(MSG::INFO) << "No SCT Version. SCT will not be built." << endmsg;
     
  } else {

    std::string versionName;

    if (versionKey.custom()) {

      msg(MSG::WARNING) << "SCT_DetectorTool:  Detector Information coming from a custom configuration!!" << endmsg;    
    } else {
      
      if(msgLvl(MSG::DEBUG)) {
	msg(MSG::DEBUG) << "SCT_DetectorTool:  Detector Information coming from the database and job options IGNORED." << endmsg;
	msg(MSG::DEBUG) << "Keys for SCT Switches are "  << versionKey.tag()  << "  " << versionKey.node() << endmsg;
      }
      IRDBRecordset_ptr switchSet = m_rdbAccessSvc->getRecordsetPtr("SctSwitches", versionKey.tag(), versionKey.node());
      const IRDBRecord    *switches   = (*switchSet)[0];
      
      m_detectorName       = switches->getString("DETECTORNAME");

      m_cosmic = false;
      if (!switches->isFieldNull("COSMICLAYOUT")) {
	m_cosmic = switches->getInt("COSMICLAYOUT");
      }

      if (!switches->isFieldNull("VERSIONNAME")) {
	versionName = switches->getString("VERSIONNAME"); 
      } 
	  
	  //    m_initialLayout      = switches->getInt("INITIALLAYOUT");
      
      
    }

    if (versionName.empty()) {
      if (m_cosmic) {
	versionName = "SR1"; 
      }
    }
    {
      if(msgLvl(MSG::DEBUG)) {
	msg(MSG::DEBUG) << "Creating the SCT" << endmsg;
	msg(MSG::DEBUG) << "SCT Geometry Options: " << endmsg;
	msg(MSG::DEBUG) << " InitialLayout:         "  << (m_initialLayout ? "true" : "false")
			<< endmsg;
	msg(MSG::DEBUG) << " Alignable:             " << (m_alignable ? "true" : "false")
			<< endmsg;
	msg(MSG::DEBUG) << " CosmicLayout:          " << (m_cosmic ? "true" : "false")
			<< endmsg;
	msg(MSG::DEBUG) << " VersionName:           " << versionName
			<< endmsg;
	 }
      
      SCT_Options options;
      
      options.setAlignable(m_alignable);
      
      
      m_manager = 0;

      // 
      // Locate the top level experiment node 
      // 
      GeoModelExperiment * theExpt; 
      if (StatusCode::SUCCESS != detStore()->retrieve( theExpt, "ATLAS" )) { 
	msg(MSG::ERROR) 
	    << "Could not find GeoModelExperiment ATLAS" 
	    << endmsg; 
	return (StatusCode::FAILURE); 
      } 
      
      // Retrieve the Geometry DB Interface
      sc = m_geometryDBSvc.retrieve();
      if (sc.isFailure()) {
	msg(MSG::FATAL) << "Could not locate Geometry DB Interface: " << m_geometryDBSvc.name() << endmsg;
	return (StatusCode::FAILURE);
      } 

      // Pass athena services to factory, etc
      m_athenaComps = new SCT_GeoModelAthenaComps;
      m_athenaComps->setDetStore(detStore().operator->());
      m_athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);
      m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);
      m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
      m_athenaComps->setLorentzAngleSvc(m_lorentzAngleSvc);
      const SCT_ID* idHelper;
      if (detStore()->retrieve(idHelper, "SCT_ID").isFailure()) {
	msg(MSG::FATAL) << "Could not get SCT ID helper" << endmsg;
	return StatusCode::FAILURE;
      } else {
	m_athenaComps->setIdHelper(idHelper);
      }

      //
      // LorentzAngleService
      //
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) 
	<< "Lorentz angle service passed to InDetReadoutGeometry with name: " << m_lorentzAngleSvc.name() << endmsg;


      //try {   
      //
      // This strange way of casting is to avoid an
      // utterly brain damaged compiler warning.
      //
      GeoPhysVol *world=&*theExpt->getPhysVol();
      
      SCT_DetectorFactory theSCT(m_athenaComps, options);
      theSCT.create(world);
      m_manager = theSCT.getDetectorManager();
	  
      if (!m_manager) {
	msg(MSG::ERROR) << "SCT_DetectorManager not created" << endmsg;
	return( StatusCode::FAILURE );
      }
      
      // Get the manager from the factory and store it in the detector store.
      //   m_detector is non constant so I can not set it to a const pointer.
      //   m_detector = theSCT.getDetectorManager();
      
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Registering SCT_DetectorManager. " << endmsg;
      
      result = detStore()->record(m_manager, m_manager->getName());
      
      if (result.isFailure() ) {
	msg(MSG::ERROR) << "Could not register SCT_DetectorManager" << endmsg;
	return( StatusCode::FAILURE );
      }
      theExpt->addManager(m_manager);
      
      // Create a symLink to the SiDetectorManager base class
      const SiDetectorManager * siDetManager = m_manager;
      result = detStore()->symLink(m_manager, siDetManager);
      if (result.isFailure() ) {
	msg(MSG::ERROR) << "Could not make link between SCT_DetectorManager and SiDetectorManager" << endmsg;
	return( StatusCode::FAILURE );
      }
      
    }  

    //
    // LorentzAngleService
    // We retrieve it to make sure it is initialized during geometry initialization.
    //
    if (!m_lorentzAngleSvc.empty()) {
      sc = m_lorentzAngleSvc.retrieve();
      if (!sc.isFailure()) {
	msg(MSG::INFO) << "Lorentz angle service retrieved: " << m_lorentzAngleSvc << endmsg;
      } else {
	msg(MSG::INFO) << "Could not retrieve Lorentz angle service:" <<  m_lorentzAngleSvc << endmsg;
      }
    } else {
      msg(MSG::INFO) << "Lorentz angle service not requested." << endmsg;
    } 
  }

  // Delete unneeded singleton objects
  SCT_DataBase::reinit();
  SCT_MaterialManager::reinit();

  return result;
}

StatusCode 
SCT_DetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::SCT_DetectorManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();

    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}

StatusCode 
SCT_DetectorTool::registerCallback()
{

  StatusCode sc = StatusCode::FAILURE;
  if (m_alignable) {

    {
      std::string folderName = "/Indet/AlignL1/ID";
      if (detStore()->contains<CondAttrListCollection>(folderName)) {
	msg(MSG::DEBUG) << "Registering callback on global Container with folder " << folderName << endmsg;
	const DataHandle<CondAttrListCollection> calc;
	StatusCode ibltmp = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), calc, folderName);
	// We don't expect this to fail as we have already checked that the detstore contains the object.                          
	if (ibltmp.isFailure()) {
	  msg(MSG::ERROR) << "Problem when register callback on global Container with folder " << folderName <<endmsg;
	} else {
	  sc =  StatusCode::SUCCESS;
	}
      } else {
        msg(MSG::WARNING) << "Unable to register callback on global Container with folder " << folderName <<endmsg;
        //return StatusCode::FAILURE; 
      }

      folderName = "/Indet/AlignL2/SCT";
      if (detStore()->contains<CondAttrListCollection>(folderName)) {
	msg(MSG::DEBUG) << "Registering callback on global Container with folder " << folderName << endmsg;
	const DataHandle<CondAttrListCollection> calc;
	StatusCode ibltmp = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), calc, folderName);
	// We don't expect this to fail as we have already checked that the detstore contains the object.                          
	if (ibltmp.isFailure()) {
	  msg(MSG::ERROR) << "Problem when register callback on global Container with folder " << folderName <<endmsg;
	} else {
	  sc =  StatusCode::SUCCESS;
	}
      } else {
	msg(MSG::WARNING) << "Unable to register callback on global Container with folder " << folderName <<endmsg;
        //return StatusCode::FAILURE;  
      }

      folderName = "/Indet/AlignL3";
      if (detStore()->contains<AlignableTransformContainer>(folderName)) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Registering callback on AlignableTransformContainer with folder " << folderName << endmsg;
	const DataHandle<AlignableTransformContainer> atc;
	StatusCode sctmp = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName);
	if(sctmp.isFailure()) {
	  msg(MSG::ERROR) << "Problem when register callback on AlignableTransformContainer with folder " << folderName <<endmsg;
	} else {
        sc =  StatusCode::SUCCESS;
	}
      }
      else {
	msg(MSG::WARNING) << "Unable to register callback on AlignableTransformContainer with folder "
			<< folderName << endmsg;
	//return StatusCode::FAILURE;                                                         
      }
    }


    {
      std::string folderName = "/Indet/Align";
      if (detStore()->contains<AlignableTransformContainer>(folderName)) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Registering callback on AlignableTransformContainer with folder " << folderName << endmsg;
	const DataHandle<AlignableTransformContainer> atc;
	StatusCode sctmp =  detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName);
	if(sctmp.isFailure()) {
	  msg(MSG::ERROR) << "Problem when register callback on AlignableTransformContainer with folder " << folderName <<endmsg;
	} else {
	  sc =  StatusCode::SUCCESS;
	}
      } else {
	msg(MSG::WARNING) << "Unable to register callback on AlignableTransformContainer with folder " 
			<< folderName << ", Alignment disabled (only if no Run2 scheme is loaded)!" << endmsg;
	//return StatusCode::FAILURE; 
      }
    }

  } else {
    msg(MSG::INFO) << "Alignment disabled. No callback registered" << endmsg;
    // We return failure otherwise it will try and register
    // a GeoModelSvc callback associated with this callback. 
    return StatusCode::FAILURE;
  }
  return sc;
}
  
StatusCode 
SCT_DetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  if (!m_manager) { 
    msg(MSG::WARNING) << "Manager does not exist" << endmsg;
    return StatusCode::FAILURE;
  }    
  if (m_alignable) {     
    return m_manager->align(I,keys);
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Alignment disabled. No alignments applied" << endmsg;
    return StatusCode::SUCCESS;
  }
}
