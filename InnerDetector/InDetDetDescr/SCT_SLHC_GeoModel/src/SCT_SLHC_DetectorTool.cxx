/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_SLHC_DetectorTool.h"
#include "SCT_SLHC_GeoModel/SCT_DetectorFactory.h" 
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h" 
#include "SCT_SLHC_GeoModel/SCT_Options.h" 
#include "SCT_SLHC_GeoModel/SCT_GeoModelAthenaComps.h" 
#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

using InDetDD::SCT_DetectorManager; 
using InDetDD::SiDetectorManager; 


SCT_SLHC_DetectorTool::SCT_SLHC_DetectorTool( const std::string& type,
				    const std::string& name, 
				    const IInterface* parent )
  : GeoModelTool( type, name, parent ),
    m_detectorName("SCT"),
    m_alignable(false),
    m_forceBuild(false),
    m_manager(nullptr), 
    m_athenaComps(nullptr),
    m_serviceBuilderTool("", this),
    m_geoDbTagSvc("GeoDbTagSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_geometryDBSvc("InDetGeometryDBSvc",name)
{
    // Get parameter values from jobOptions file
    declareProperty("ForceBuild", m_forceBuild);
    declareProperty("DetectorName", m_detectorName);
    declareProperty("Alignable", m_alignable);
    declareProperty("ServiceBuilderTool", m_serviceBuilderTool);
    declareProperty("RDBAccessSvc", m_rdbAccessSvc);
    declareProperty("GeometryDBSvc", m_geometryDBSvc);
    declareProperty("GeoDbTagSvc", m_geoDbTagSvc);
}

SCT_SLHC_DetectorTool::~SCT_SLHC_DetectorTool()
{
  delete m_athenaComps;
}

// Create the Geometry via the factory corresponding to this tool
StatusCode SCT_SLHC_DetectorTool::create(){ 

  // Get the detector configuration.
  ATH_CHECK(m_geoDbTagSvc.retrieve());

  DecodeVersionKey versionKey(&*m_geoDbTagSvc, "SCT");
 
  // Issue error if AUTO.
  if (versionKey.tag() == "AUTO"){
    ATH_MSG_ERROR("AUTO Atlas version. Please select a version.");
  }

  ATH_MSG_INFO("Building SCT SLHC with Version Tag: "<< versionKey.tag() << " at Node: " << versionKey.node());

  ATH_CHECK(m_rdbAccessSvc.retrieve());

  // Print the SCT version tag:
  std::string sctVersionTag;
  sctVersionTag = m_rdbAccessSvc->getChildTag("SCT", versionKey.tag(), versionKey.node());
  ATH_MSG_INFO("SCT Version: " << sctVersionTag <<  "  Package Version: " << PACKAGE_VERSION);

  // Check if version is empty. If so, then the SCT cannot be built. 
  //This may or may not be intentional. We just issue an INFO message. 
  if (sctVersionTag.empty()) { 
    ATH_MSG_INFO("No SCT Version. SCT_SLHC will not be built.");
  } else {
    std::string versionName;
    std::string descrName="noDescr";

    if (versionKey.custom()){
      ATH_MSG_WARNING("SCT_SLHC_DetectorTool:  Detector Information coming "
                      <<"from a custom configuration!!");
    } else {
      ATH_MSG_DEBUG("SCT_SLHC_DetectorTool:  Detector Information coming from the"
                    <<" database and job options IGNORED.");
      ATH_MSG_DEBUG("Keys for SCT Switches are "  << versionKey.tag()  
                    << "  " << versionKey.node());
      IRDBRecordset_ptr switchSet = m_rdbAccessSvc->getRecordsetPtr("SctSwitches", versionKey.tag(), versionKey.node());
      const IRDBRecord    *switches   = (*switchSet)[0];
        
      m_detectorName       = switches->getString("DETECTORNAME");
      
      if (!switches->isFieldNull("VERSIONNAME")) {
	versionName = switches->getString("VERSIONNAME"); 
      } 
      if (!switches->isFieldNull("DESCRIPTION")) {
	descrName = switches->getString("DESCRIPTION");
      }
  
    }
  
    ATH_MSG_DEBUG("VersioName = " << versionName);

    // Only build if versionName is "SLHC" or ForceBuild is true

    if (!m_forceBuild && versionName != "SLHC") {

      ATH_MSG_INFO("Not SLHC version. SCT_SLHC will not be built.");

    } else { // SLHC version requested.
      
      ATH_MSG_DEBUG("Creating the SCT (SLHC version)");

      // Create the SCT_DetectorFactory
      m_manager = 0;
      // Locate the top level experiment node  
      GeoModelExperiment * theExpt = nullptr; 
      ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS")); 

      // Retrieve the Geometry DB Interface
      ATH_CHECK(m_geometryDBSvc.retrieve());

      // Pass athena services to factory, etc
      m_athenaComps = new InDetDDSLHC::SCT_GeoModelAthenaComps;
      m_athenaComps->setDetStore(detStore().operator->());
      m_athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);
      m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);
      m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);

      const SCT_ID* idHelper = nullptr;
      ATH_CHECK(detStore()->retrieve(idHelper, "SCT_ID"));
      m_athenaComps->setIdHelper(idHelper);

      // Service builder tool
      if (!m_serviceBuilderTool.empty()) {
	StatusCode sc = m_serviceBuilderTool.retrieve(); 
	if (!sc.isFailure()) {
	  ATH_MSG_INFO("Service builder tool retrieved: " << m_serviceBuilderTool);
	  m_athenaComps->setServiceBuilderTool(&*m_serviceBuilderTool);
	} else {
	  ATH_MSG_ERROR("Could not retrieve " <<  m_serviceBuilderTool << ",  some services will not be built.");
	}
      } else {
	// This will become an error once the tool is ready.
	// ATH_MSG_ERROR("Service builder tool not specified. Some services will not be built");
	ATH_MSG_INFO("Service builder tool not specified.");
      } 


      // This strange way of casting is to avoid an
      // utterly brain damaged compiler warning.
      GeoPhysVol *world=&*theExpt->getPhysVol();
      InDetDDSLHC::SCT_Options options;
      options.setAlignable(m_alignable);
      InDetDDSLHC::SCT_DetectorFactory theSCT(m_athenaComps, options);
      if(descrName.compare("TrackingGeometry")!=0) theSCT.create(world);
      else ATH_MSG_INFO("SCT_SLHC - TrackingGeometry tag - no geometry built"); 
      m_manager = theSCT.getDetectorManager();
    
      if (m_manager==nullptr) {
	ATH_MSG_ERROR("SCT_DetectorManager not created");
	return StatusCode::FAILURE;
      }
    
      // Get the manager from the factory and store it in the detector store.
      ATH_MSG_DEBUG("Registering SCT_DetectorManager. ");
      ATH_CHECK(detStore()->record(m_manager, m_manager->getName()));
      theExpt->addManager(m_manager);
      // Create a symLink to the SiDetectorManager base class
      const SiDetectorManager * siDetManager = m_manager;
      ATH_CHECK(detStore()->symLink(m_manager, siDetManager));
    }  
  }
  return StatusCode::SUCCESS;
}

StatusCode 
SCT_SLHC_DetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::SCT_DetectorManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = nullptr;
  }
  return StatusCode::SUCCESS;
}

