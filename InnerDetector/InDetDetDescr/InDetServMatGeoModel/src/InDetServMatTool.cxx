/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatTool.h"
#include "InDetServMatGeoModel/InDetServMatFactory.h"
#include "InDetServMatGeoModel/InDetServMatFactoryDC2.h"
#include "InDetServMatGeoModel/InDetServMatFactoryDC3.h"
#include "InDetServMatGeoModel/InDetServMatFactoryFS.h"
#include "InDetServMatGeoModel/InDetServMatFactorySLHC.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"
#include "InDetServMatGeoModel/InDetServMatAthenaComps.h"

#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"



/**
 ** Constructor(s)
 **/
InDetServMatTool::InDetServMatTool( const std::string& type, const std::string& name, const IInterface* parent )
  : GeoModelTool( type, name, parent ),
    m_geoDbTagSvc("GeoDbTagSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_geometryDBSvc("InDetGeometryDBSvc",name),
    m_builderTool(""),
    m_devVersion(false),
    m_forFrozenShowers(false),
    m_manager(0),
    m_athenaComps(0)
{
  declareProperty("DevVersion",m_devVersion);
  declareProperty("FrozenShowers",m_forFrozenShowers);
  declareProperty("OverrideVersionName", m_overrideVersionName);
  declareProperty("RDBAccessSvc", m_rdbAccessSvc);
  declareProperty("GeometryDBSvc", m_geometryDBSvc);
  declareProperty("ServiceBuilderTool", m_builderTool);
  declareProperty("GeoDbTagSvc", m_geoDbTagSvc);
  // add here the properties
}
/**
 ** Destructor
 **/
InDetServMatTool::~InDetServMatTool()
{
  delete m_athenaComps;
}

/**
 ** Create the Detector Node corresponding to this tool
 **/
StatusCode InDetServMatTool::create()
{ 

  if (m_devVersion) {
    msg(MSG::WARNING) << "You are using a development version. There are no guarantees of stability" 
	<< endmsg;
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
  
  StatusCode result = StatusCode::SUCCESS;


  // Get the detector configuration.
  StatusCode sc = m_geoDbTagSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoDbTagSvc" << endmsg;
    return (StatusCode::FAILURE);
  } 
  
  DecodeVersionKey versionKey(&*m_geoDbTagSvc, "InnerDetector");
  
  sc = m_rdbAccessSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endmsg;
    return (StatusCode::FAILURE); 
  }  
  
  std::string versionTag = m_rdbAccessSvc->getChildTag("InDetServices", versionKey.tag(), versionKey.node());
  msg(MSG::INFO) << "InDetServMat Package Version: " << PACKAGE_VERSION << endmsg;
  if(msgLvl(MSG::DEBUG)) msg() << "versionTag=" << versionTag <<" %%%"<< endmsg;

  // If versionTag is NULL then don't build.
  if (versionTag.empty()) { 
    msg(MSG::INFO)  << "No InDetService Version. InDetService will not be built." << endmsg;
    if(msgLvl(MSG::DEBUG)) msg() << "InnerDetector Version Tag: " << versionKey.tag() << " at Node: " 
	<< versionKey.node() << endmsg;
    return StatusCode::SUCCESS;
  } 

  if(msgLvl(MSG::DEBUG)) msg() << "Keys for InDetServMat Switches are "  << versionKey.tag()  << "  " << versionKey.node() << endmsg;
  
  std::string versionName;
  std::string descrName="noDescr";
  if (!m_rdbAccessSvc->getChildTag("InDetServSwitches", versionKey.tag(), versionKey.node()).empty()) {
    IRDBRecordset_ptr switchSet = m_rdbAccessSvc->getRecordsetPtr("InDetServSwitches", versionKey.tag(), versionKey.node());
    const IRDBRecord    *switchTable   = (*switchSet)[0];    
    versionName = switchTable->getString("VERSIONNAME"); 
    if (!switchTable->isFieldNull("DESCRIPTION")) descrName = switchTable->getString("DESCRIPTION");
  }

  if (!m_overrideVersionName.empty()) {
    versionName = m_overrideVersionName;
    msg(MSG::INFO) << "Overriding version name: " << versionName << endmsg;
  }
 
  msg(MSG::INFO) << "Building Inner Detector Service Material. Version: " << versionName << endmsg;

  // Retrieve the Geometry DB Interface
  sc = m_geometryDBSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate Geometry DB Interface: " << m_geometryDBSvc.name() << endmsg;
    return (StatusCode::FAILURE); 
  }  

  // Pass athena services to factory, etc
  m_athenaComps = new InDetServMatAthenaComps;
  m_athenaComps->setDetStore(detStore().operator->());
  m_athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);
  m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
  m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);

  // Retrieve builder tool (SLHC only)
  if (versionName == "SLHC") {
    if (!m_builderTool.empty()) {
      sc = m_builderTool.retrieve(); 
      if (!sc.isFailure()) {
	msg(MSG::INFO) << "Service builder tool retrieved: " << m_builderTool << endmsg;
	m_athenaComps->setBuilderTool(&*m_builderTool);
      } else {
	msg(MSG::ERROR) << "Could not retrieve " <<  m_builderTool << ",  some services will not be built." << endmsg;
      }
    } else {
      // This will become an error once the tool is ready.
      //msg(MSG::ERROR) << "Service builder tool not specified. Some services will not be built" << endmsg;
      msg(MSG::INFO) << "Service builder tool not specified." << endmsg; 
    }
  }
  
  if ( 0 == m_detector ) {
    // Create the InDetServMatNode instance
    try {   
      //
      // This strange way of casting is to avoid an
      // utterly brain damaged compiler warning.
      //
      GeoPhysVol *world=&*theExpt->getPhysVol();
      if(m_forFrozenShowers) {
	if(msgLvl(MSG::DEBUG)) msg() << " InDetServMat Factory FS " << endmsg;
	InDetServMatFactoryFS theIDSM(detStore().operator->(),m_rdbAccessSvc);
	theIDSM.create(world);
	m_manager=theIDSM.getDetectorManager();
      } else {
	if(!m_devVersion) {
	  if (versionName.empty() || versionName == "DC2") {
	    // DC2 and Rome
	    if(msgLvl(MSG::DEBUG)) msg() << " InDetServMat Factory DC2 " << endmsg;
	    InDetServMatFactoryDC2 theIDSM(detStore().operator->(), m_rdbAccessSvc);
	    theIDSM.create(world);
	    m_manager=theIDSM.getDetectorManager();
	  } else if (versionName == "DC3") {
	    // DC3 
	    if(msgLvl(MSG::DEBUG)) msg() << " InDetServMat Factory DC3 " << endmsg;
	    InDetServMatFactoryDC3 theIDSM(m_athenaComps);
	    theIDSM.create(world);
	    m_manager=theIDSM.getDetectorManager();
	  } else if (versionName == "CSC") {
	    if(msgLvl(MSG::DEBUG)) msg() << " InDetServMat Factory CSC " << endmsg;
	    InDetServMatFactory theIDSM(m_athenaComps);
	    theIDSM.create(world);
	    m_manager=theIDSM.getDetectorManager();
	  } else if (versionName == "SLHC") {
	    if(msgLvl(MSG::DEBUG)) msg() << " InDetServMat Factory SLHC version " << endmsg;
	    InDetServMatFactorySLHC theIDSM(m_athenaComps);
	    if(descrName.compare("TrackingGeometry")!=0)
	      theIDSM.create(world);
	    else
	      msg(MSG::INFO) << "InDetServices - TrackingGeometry tag - no geometry built" << endmsg; 
	    m_manager=theIDSM.getDetectorManager();    
	  } else {
	    // Unrecognized name.
	    msg(MSG::ERROR) << " Unrecognized VersionName: " << versionName << endmsg;
	    return StatusCode::FAILURE;
	  }
	} else { // Development Versions
	  // CSC 
	  if(msgLvl(MSG::DEBUG)) msg() << " InDetServMat Factory Development version " << endmsg;
	  InDetServMatFactory theIDSM(m_athenaComps);
	  theIDSM.create(world);
	  m_manager=theIDSM.getDetectorManager();
	}
      }

    } catch (std::bad_alloc) {
	msg(MSG::FATAL) << "Could not create new InDetServMatNode!" << endmsg;
      return StatusCode::FAILURE; 
    }
  }
  if (m_manager) {
    theExpt->addManager(m_manager);
    CHECK( detStore()->record (m_manager, m_manager->getName()) );
  } else {
    msg(MSG::FATAL) << "Could not create InDetServMatManager!" << endmsg;
    return StatusCode::FAILURE;     
  }
  return result;
}

StatusCode InDetServMatTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::InDetServMatManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}
