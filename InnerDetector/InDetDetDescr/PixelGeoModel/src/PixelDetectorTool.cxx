/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelGeoModel/PixelDetectorTool.h"
#include "PixelGeoModel/PixelDetectorFactory.h" 
#include "PixelGeoModel/PixelDetectorFactorySR1.h" 
#include "PixelGeoModel/PixelDetectorFactoryDC2.h" 
#include "PixelGeoModel/PixelGeometryManager.h" 
#include "PixelGeoModel/PixelSwitches.h" 
#include "PixelGeoModel/IBLParameterSvc.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h" 
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "InDetCondServices/ISiLorentzAngleSvc.h"
#include "PixelGeoModel/PixelGeoModelAthenaComps.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoSubDetTool.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include "SGTools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

using InDetDD::PixelDetectorManager; 
using InDetDD::SiDetectorManager; 

/**
 ** Constructor(s)
 **/
PixelDetectorTool::PixelDetectorTool( const std::string& type, const std::string& name, const IInterface* parent )
  : GeoModelTool( type, name, parent ),
    m_detectorName("PixelDetector"),
    m_IBLParameterSvc("IBLParameterSvc",name),
    m_buildDBM(0),
    m_bcmTool(""),
    m_blmTool(""),
    m_serviceBuilderTool(""),
    m_geoDbTagSvc("GeoDbTagSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_geometryDBSvc("InDetGeometryDBSvc",name),
    m_lorentzAngleSvc("PixelLorentzAngleSvc", name),
    m_manager(0),
    m_athenaComps(0)
{
  declareProperty("Services",m_services=true);
  declareProperty("ServicesOnLadder",m_servicesOnLadder=true); ///JBdV
  declareProperty("Alignable", m_alignable=true);
  declareProperty("TweakIBLDist", m_tweakIBLDist=true);
  declareProperty("DC1Geometry",m_dc1Geometry=false); 
  declareProperty("InitialLayout",m_initialLayout=false);
  declareProperty("DevVersion", m_devVersion=false);
  declareProperty("BCM_Tool", m_bcmTool);
  declareProperty("BLM_Tool", m_blmTool);
  declareProperty("ServiceBuilderTool", m_serviceBuilderTool);
  declareProperty("GeoDbTagSvc", m_geoDbTagSvc);
  declareProperty("RDBAccessSvc", m_rdbAccessSvc);
  declareProperty("GeometryDBSvc", m_geometryDBSvc);
  declareProperty("LorentzAngleSvc", m_lorentzAngleSvc);
  declareProperty("OverrideVersionName", m_overrideVersionName);
}
/**
 ** Destructor
 **/
PixelDetectorTool::~PixelDetectorTool()
{
  // This will need to be modified once we register the Pixel DetectorNode in
  // the Transient Detector Store
  delete m_detector; // Needs checking if this is really needed or not.
  delete m_athenaComps;
}

/**
 ** Create the Detector Node corresponding to this tool
 **/
StatusCode PixelDetectorTool::create()
{ 
  StatusCode result = StatusCode::SUCCESS;


  if (m_devVersion) {
    msg(MSG::WARNING) << "You are using a development version. There are no guarantees of stability"
	<< endmsg;
  }
   

  // Get the detector configuration.
  StatusCode sc = m_geoDbTagSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoDbTagSvc" << endmsg;
    return (StatusCode::FAILURE);
  } 
  
  DecodeVersionKey versionKey(&*m_geoDbTagSvc, "Pixel");

  msg(MSG::INFO) << "Building Pixel Detector with Version Tag: " << versionKey.tag() << " at Node: " 
		 << versionKey.node() << endmsg;

  std::string pixelVersionTag;

  sc = m_rdbAccessSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endmsg;
    return (StatusCode::FAILURE); 
  }  

  // Print the version tag:
  pixelVersionTag = m_rdbAccessSvc->getChildTag("Pixel", versionKey.tag(), versionKey.node());
  msg(MSG::INFO) << "Pixel Version: " << pixelVersionTag << "  Package Version: " << PACKAGE_VERSION << endmsg;
  
  
  // Check if version is empty. If so, then the SCT cannot be built. This may or may not be intentional. We
  // just issue an INFO message. 
  if (pixelVersionTag.empty()) { 
    msg(MSG::INFO) << "No Pixel Version. Pixel Detector will not be built." << endmsg;
     
  } else {
  
    // Unless we are using custom pixel, the switch positions are going to
    // come from the database:
    
    std::string versionName;
    std::string descrName="noDescr";

    if (versionKey.custom()) {

      msg(MSG::WARNING) << "PixelDetectorTool:  Detector Information coming from a custom configuration!!" << endmsg;
 
    } else {

      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "PixelDetectorTool:  Detector Information coming from the database and job options IGNORED." << endmsg;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Keys for Pixel Switches are "  << versionKey.tag()  << "  " << versionKey.node() << endmsg;
      IRDBRecordset_ptr switchSet = m_rdbAccessSvc->getRecordsetPtr("PixelSwitches", versionKey.tag(), versionKey.node());
      const IRDBRecord    *switchTable   = (*switchSet)[0];
      
      //m_services           = switchTable->getInt("BUILDSERVICES");
      //m_alignable          = switcheTable->getInt("ALIGNABLE");
      m_dc1Geometry        = switchTable->getInt("DC1GEOMETRY");
      m_initialLayout      = switchTable->getInt("INITIALLAYOUT");
      if (!switchTable->isFieldNull("VERSIONNAME")) {
	versionName        = switchTable->getString("VERSIONNAME");
      }
      if (!switchTable->isFieldNull("DESCRIPTION")) {
	descrName        = switchTable->getString("DESCRIPTION");
      }
      m_buildDBM        = switchTable->getInt("BUILDDBM");
   }

   if (versionName.empty()) {
      if (m_dc1Geometry) {
	versionName = "DC1"; 
      } else {
	versionName = "DC2"; 
      } 
   }

   if (!m_overrideVersionName.empty()) {
     versionName = m_overrideVersionName;
     msg(MSG::INFO) << "Overriding version name: " << versionName << endmsg;
   }

   if(msgLvl(MSG::DEBUG)) {  
     msg(MSG::DEBUG)  << "Creating the Pixel " << endmsg;
     msg(MSG::DEBUG)  << "Pixel Geometry Options:" << endmsg;
     msg(MSG::DEBUG)  << "  Services           = " << (m_services ? "true" : "false") << endmsg;
     msg(MSG::DEBUG)  << "  Alignable          = " << (m_alignable ? "true" : "false") <<endmsg;
     msg(MSG::DEBUG)  << "  DC1Geometry        = " << (m_dc1Geometry ? "true" : "false") <<endmsg;
     msg(MSG::DEBUG)  << "  InitialLayout      = " << (m_initialLayout ? "true" : "false") <<endmsg;
     msg(MSG::DEBUG)  << "  VersioName         = " << versionName  << endmsg;
   }
    if (m_IBLParameterSvc.retrieve().isFailure()) {
       ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc");
    }
    else {
	m_IBLParameterSvc->setBoolParameters(m_alignable,"alignable");
    }

    //
    // Initialize the geometry manager
    //

    // Initialize switches
    PixelSwitches switches;
    
    switches.setServices(m_services);
    switches.setDC1Geometry(m_dc1Geometry);
    switches.setAlignable(m_alignable);
    switches.setInitialLayout(m_initialLayout);
    if (versionName == "SLHC") switches.setSLHC();
    if (versionName == "IBL") switches.setIBL();
    switches.setDBM(m_buildDBM); //DBM flag

    //JBdV
    switches.setServicesOnLadder(m_servicesOnLadder);
    switches.setServices(m_services); //Overwrite there for the time being.

    const PixelID * idHelper = 0;
    if (detStore()->retrieve(idHelper, "PixelID").isFailure()) {
      msg(MSG::FATAL) << "Could not get Pixel ID helper" << endmsg;
      return StatusCode::FAILURE;
    }


    // Retrieve the Geometry DB Interface
    sc = m_geometryDBSvc.retrieve();
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Could not locate Geometry DB Interface: " << m_geometryDBSvc.name() << endmsg;
      return (StatusCode::FAILURE); 
    }  

    // Pass athena services to factory, etc
    m_athenaComps = new PixelGeoModelAthenaComps;
    m_athenaComps->setDetStore(detStore().operator->());
    m_athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);
    m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
    m_athenaComps->setLorentzAngleSvc(m_lorentzAngleSvc);
    m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);
    m_athenaComps->setIdHelper(idHelper);

    //
    // LorentzAngleService
    //
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Lorentz angle service passed to InDetReadoutGeometry with name: " 
					   << m_lorentzAngleSvc.name() << endmsg;
     

    // BCM Tool.
    if (!m_bcmTool.empty()) {
      sc = m_bcmTool.retrieve();
      if (!sc.isFailure()) {
	msg(MSG::INFO) << "BCM_GeoModel tool retrieved: " << m_bcmTool << endmsg;
      } else {
	msg(MSG::INFO) << "Could not retrieve " << m_bcmTool << " -  BCM will not be built" << endmsg;
      }
      m_athenaComps->setBCM(&*m_bcmTool);
      //IGeoSubDetTool* tt = m_bcmTool;

    } else {
      msg(MSG::INFO) << "BCM not requested." << endmsg;
    }


   // BLM Tool.
    if (!m_blmTool.empty()) {
      sc = m_blmTool.retrieve();
      if (!sc.isFailure()) {
	msg(MSG::INFO) << "BLM_GeoModel tool retrieved: " << m_blmTool << endmsg;
      } else {
	msg(MSG::INFO) << "Could not retrieve " << m_blmTool << " -  BLM will not be built" << endmsg;
      }
      m_athenaComps->setBLM(&*m_blmTool);

    } else {
      msg(MSG::INFO) << "BLM not requested." << endmsg;
    }

    // Service builder tool
    if (!m_serviceBuilderTool.empty()) {
      sc = m_serviceBuilderTool.retrieve(); 
      if (!sc.isFailure()) {
	msg(MSG::INFO) << "Service builder tool retrieved: " << m_serviceBuilderTool << endmsg;
	m_athenaComps->setServiceBuilderTool(&*m_serviceBuilderTool);
      } else {
	msg(MSG::ERROR) << "Could not retrieve " <<  m_serviceBuilderTool << ",  some services will not be built." << endmsg;
      }
    } else {
      if (versionName == "SLHC") {
	// This will become an error once the tool is ready.
	//msg(MSG::ERROR) << "Service builder tool not specified. Some services will not be built" << endmsg;
	msg(MSG::INFO) << "Service builder tool not specified." << endmsg; 
      } else {
	msg(MSG::INFO) << "Service builder tool not specified." << endmsg; 
      }	
    }


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
    
    GeoPhysVol *world=&*theExpt->getPhysVol();
    m_manager = 0;
 
    if (!m_devVersion) {
      
      if(versionName == "DC1" || versionName == "DC2") {	
        // DC1/DC2 version
        PixelDetectorFactoryDC2 thePixel(m_athenaComps, switches);
        thePixel.create(world);      
	m_manager  = thePixel.getDetectorManager();
      } else if (versionName == "SR1") {
	// SR1. Same a DC3 but only 1 part (barrel, ec A or ec C) built
	PixelDetectorFactorySR1 thePixel(m_athenaComps, switches);
        thePixel.create(world);      
        m_manager  = thePixel.getDetectorManager();
      } else {
	// DC3, SLHC, IBL
        PixelDetectorFactory thePixel(m_athenaComps, switches);
	if(descrName.compare("TrackingGeometry")!=0)
	  thePixel.create(world);      
	else
	  msg(MSG::INFO) << "Pixel - TrackingGeometry tag - no geometry built" << endmsg; 
        m_manager  = thePixel.getDetectorManager();
      }	  
      


    } else {
      //
      // DEVELOPMENT VERSIONS
      //
      PixelDetectorFactory thePixel(m_athenaComps, switches);
      thePixel.create(world);      
      m_manager  = thePixel.getDetectorManager();
    }

    // Register the manager to the Det Store    
    if (StatusCode::FAILURE == detStore()->record(m_manager, m_manager->getName()) ) {
      msg(MSG::ERROR) << "Could not register Pixel detector manager" << endmsg;
      return( StatusCode::FAILURE );
    }
    // Add the manager to the experiment 
    theExpt->addManager(m_manager);
    
    // Symlink the manager
    const SiDetectorManager * siDetManager = m_manager;
    if (StatusCode::FAILURE == detStore()->symLink(m_manager, siDetManager) ) { 
      msg(MSG::ERROR) << "Could not make link between PixelDetectorManager and SiDetectorManager" << endmsg;
      return( StatusCode::FAILURE );
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

  return result;
}

StatusCode PixelDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::PixelDetectorManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}
  
StatusCode   
PixelDetectorTool::registerCallback()
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

      folderName = "/Indet/AlignL2/PIX";
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
		      << folderName <<  endmsg;
	//return StatusCode::FAILURE;
      }
    }

    
    {
      std::string folderName = "/Indet/Align";
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
			<< folderName << ", Alignment disabled (only if no Run2 scheme is loaded)!" << endmsg;
	//return StatusCode::FAILURE; 
      }
    }
    
    if (m_tweakIBLDist) {
      //IBLDist alignment should be made optional; Will not be available prior to period G in Run2
      std::string ibl_folderName = "/Indet/IBLDist";
      if (detStore()->contains<CondAttrListCollection>(ibl_folderName)) {
	msg(MSG::DEBUG) << "Registering callback on IBLDist with folder " << ibl_folderName << endmsg;
	const DataHandle<CondAttrListCollection> calc;
	StatusCode ibltmp = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), calc, ibl_folderName);
	// We don't expect this to fail as we have already checked that the detstore contains the object.
	if (ibltmp.isFailure()) {
	  msg(MSG::ERROR) << "Problem when register callback on IBLDist with folder " << ibl_folderName <<endmsg;
	} else {
	  sc =  StatusCode::SUCCESS;
	}
      } else {
	// We don't return false, as it might be possible that we run an old configuration without new DB;
	// Return a clear warning msg for now.
	msg(MSG::WARNING) << "Unable to register callback on IBLDist with folder " << ibl_folderName <<endmsg;
	msg(MSG::WARNING) << "This should not happen that  no LB-IOV IBL-bowing DB is provided for this run " <<endmsg;
      }
    }// end of tweakIBLDist

  } else {
    msg(MSG::INFO) << "Alignment disabled. No callback registered" << endmsg;
    // We return failure otherwise it will try and register
    // a GeoModelSvc callback associated with this callback.
  }
  return sc;

}
  
StatusCode 
PixelDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I,keys))
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
