/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
    m_manager(nullptr), 
    m_athenaComps(nullptr),
    m_geoDbTagSvc("GeoDbTagSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_geometryDBSvc("InDetGeometryDBSvc",name)
{
  // Get parameter values from jobOptions file
  declareProperty("DetectorName", m_detectorName);
  declareProperty("InitialLayout", m_initialLayout=false);
  declareProperty("Alignable", m_alignable);
  declareProperty("RDBAccessSvc", m_rdbAccessSvc);
  declareProperty("GeometryDBSvc", m_geometryDBSvc);
  declareProperty("GeoDbTagSvc", m_geoDbTagSvc);
}

//
// Destructor
//
SCT_DetectorTool::~SCT_DetectorTool()
{
  delete m_athenaComps;
}

//
// Initialize
//
StatusCode
SCT_DetectorTool::initialize()
{
  // LorentzAngleTool
  if (not m_lorentzAngleTool.empty()) {
    ATH_CHECK(m_lorentzAngleTool.retrieve());
  } else {
    m_lorentzAngleTool.disable();
  }

  return StatusCode::SUCCESS;
}

//
// Create the Geometry via the factory corresponding to this tool
//

StatusCode
SCT_DetectorTool::create()
{ 

  // Reinit various singleton type objects.
  SCT_DataBase::reinit();
  SCT_MaterialManager::reinit();

  // Get the detector configuration.
  ATH_CHECK(m_geoDbTagSvc.retrieve());
  
  DecodeVersionKey versionKey(&*m_geoDbTagSvc, "SCT");
  
  // Issue error if AUTO.
  if (versionKey.tag() == "AUTO"){
    ATH_MSG_ERROR("AUTO Atlas version. Please select a version.");
  }

  ATH_MSG_INFO("Building SCT with Version Tag: "<< versionKey.tag() << " at Node: " << versionKey.node());

  ATH_CHECK(m_rdbAccessSvc.retrieve());

  // Print the SCT version tag:
  std::string sctVersionTag;
  sctVersionTag =  m_rdbAccessSvc->getChildTag("SCT", versionKey.tag(), versionKey.node());
  ATH_MSG_INFO("SCT Version: " << sctVersionTag <<  "  Package Version: " << PACKAGE_VERSION);


  // Check if version is empty. If so, then the SCT cannot be built. This may or may not be intentional. We
  // just issue an INFO message. 
  if (sctVersionTag.empty()) { 
    ATH_MSG_INFO("No SCT Version. SCT will not be built.");
     
  } else {

    std::string versionName;

    if (versionKey.custom()) {

      ATH_MSG_WARNING("SCT_DetectorTool:  Detector Information coming from a custom configuration!!");
    } else {
      
      ATH_MSG_DEBUG("SCT_DetectorTool:  Detector Information coming from the database and job options IGNORED.");
      ATH_MSG_DEBUG("Keys for SCT Switches are "  << versionKey.tag()  << "  " << versionKey.node());

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
    }

    if (versionName.empty()) {
      if (m_cosmic) {
	versionName = "SR1"; 
      }
    }

    {
      ATH_MSG_DEBUG("Creating the SCT");
      ATH_MSG_DEBUG("SCT Geometry Options: ");
      ATH_MSG_DEBUG(" InitialLayout:         "  << (m_initialLayout ? "true" : "false"));
      ATH_MSG_DEBUG(" Alignable:             " << (m_alignable ? "true" : "false"));
      ATH_MSG_DEBUG(" CosmicLayout:          " << (m_cosmic ? "true" : "false"));
      ATH_MSG_DEBUG(" VersionName:           " << versionName);
      
      SCT_Options options;
      
      options.setAlignable(m_alignable);
      
      m_manager = nullptr;

      // 
      // Locate the top level experiment node 
      // 
      GeoModelExperiment * theExpt = nullptr;
      ATH_CHECK(detStore()->retrieve( theExpt, "ATLAS" ));
      
      // Retrieve the Geometry DB Interface
      ATH_CHECK(m_geometryDBSvc.retrieve());

      // Pass athena services to factory, etc
      m_athenaComps = new SCT_GeoModelAthenaComps;
      m_athenaComps->setDetStore(detStore().operator->());
      m_athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);
      m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);
      m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
      m_athenaComps->setLorentzAngleTool(m_lorentzAngleTool.get());
      const SCT_ID* idHelper = nullptr;
      ATH_CHECK(detStore()->retrieve(idHelper, "SCT_ID"));

      m_athenaComps->setIdHelper(idHelper);

      //
      // LorentzAngleTool
      //
      ATH_MSG_DEBUG("Lorentz angle service passed to InDetReadoutGeometry with name: " << m_lorentzAngleTool.name());


      //try {   
      //
      // This strange way of casting is to avoid an
      // utterly brain damaged compiler warning.
      //
      GeoPhysVol *world=&*theExpt->getPhysVol();
      
      SCT_DetectorFactory theSCT(m_athenaComps, options);
      theSCT.create(world);
      m_manager = theSCT.getDetectorManager();
	  
      if (m_manager==nullptr) {
	ATH_MSG_ERROR("SCT_DetectorManager not created");
	return StatusCode::FAILURE;
      }
      
      // Get the manager from the factory and store it in the detector store.
      //   m_detector is non constant so I can not set it to a const pointer.
      //   m_detector = theSCT.getDetectorManager();
      
      ATH_MSG_DEBUG("Registering SCT_DetectorManager. ");
      
      ATH_CHECK(detStore()->record(m_manager, m_manager->getName()));
      theExpt->addManager(m_manager);
      
      // Create a symLink to the SiDetectorManager base class
      const SiDetectorManager * siDetManager = m_manager;
      ATH_CHECK(detStore()->symLink(m_manager, siDetManager));
      
    }  
    // LorentzAngleTool
    if (m_lorentzAngleTool.get()) {
      // SCT_DetectorManager is not available at initialize of m_lorentzAngleTool
      ATH_CHECK(m_lorentzAngleTool->retrieveDetectorManager());
    }
  }

  // Delete unneeded singleton objects
  SCT_DataBase::reinit();
  SCT_MaterialManager::reinit();

  return StatusCode::SUCCESS;
}

StatusCode 
SCT_DetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::SCT_DetectorManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();

    m_manager = nullptr;
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
	ATH_MSG_DEBUG("Registering callback on global Container with folder " << folderName);
	const DataHandle<CondAttrListCollection> calc;
	StatusCode ibltmp = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), calc, folderName);
	// We don't expect this to fail as we have already checked that the detstore contains the object.                          
	if (ibltmp.isFailure()) {
	  ATH_MSG_ERROR("Problem when register callback on global Container with folder " << folderName);
	} else {
	  sc =  StatusCode::SUCCESS;
	}
      } else {
        ATH_MSG_WARNING("Unable to register callback on global Container with folder " << folderName);
      }

      folderName = "/Indet/AlignL2/SCT";
      if (detStore()->contains<CondAttrListCollection>(folderName)) {
	ATH_MSG_DEBUG("Registering callback on global Container with folder " << folderName);
	const DataHandle<CondAttrListCollection> calc;
	StatusCode ibltmp = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), calc, folderName);
	// We don't expect this to fail as we have already checked that the detstore contains the object.                          
	if (ibltmp.isFailure()) {
	  ATH_MSG_ERROR("Problem when register callback on global Container with folder " << folderName);
	} else {
	  sc =  StatusCode::SUCCESS;
	}
      } else {
	ATH_MSG_WARNING("Unable to register callback on global Container with folder " << folderName);
      }

      folderName = "/Indet/AlignL3";
      if (detStore()->contains<AlignableTransformContainer>(folderName)) {
	ATH_MSG_DEBUG("Registering callback on AlignableTransformContainer with folder " << folderName);
	const DataHandle<AlignableTransformContainer> atc;
	StatusCode sctmp = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName);
	if(sctmp.isFailure()) {
	  ATH_MSG_ERROR("Problem when register callback on AlignableTransformContainer with folder " << folderName);
	} else {
        sc =  StatusCode::SUCCESS;
	}
      } else {
	ATH_MSG_WARNING("Unable to register callback on AlignableTransformContainer with folder " << folderName);
      }
    }


    {
      std::string folderName = "/Indet/Align";
      if (detStore()->contains<AlignableTransformContainer>(folderName)) {
	ATH_MSG_DEBUG("Registering callback on AlignableTransformContainer with folder " << folderName);
	const DataHandle<AlignableTransformContainer> atc;
	StatusCode sctmp =  detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName);
	if(sctmp.isFailure()) {
	  ATH_MSG_ERROR("Problem when register callback on AlignableTransformContainer with folder " << folderName);
	} else {
	  sc =  StatusCode::SUCCESS;
	}
      } else {
	ATH_MSG_WARNING("Unable to register callback on AlignableTransformContainer with folder "
			<< folderName << ", Alignment disabled (only if no Run2 scheme is loaded)!");
      }
    }

  } else {
    ATH_MSG_INFO("Alignment disabled. No callback registered");
    // We return failure otherwise it will try and register
    // a GeoModelSvc callback associated with this callback. 
    return StatusCode::FAILURE;
  }
  return sc;
}
  
StatusCode 
SCT_DetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  if (m_manager==nullptr) { 
    ATH_MSG_WARNING("Manager does not exist");
    return StatusCode::FAILURE;
  }    
  if (m_alignable) {     
    return m_manager->align(I,keys);
  } else {
    ATH_MSG_DEBUG("Alignment disabled. No alignments applied");
    return StatusCode::SUCCESS;
  }
}
