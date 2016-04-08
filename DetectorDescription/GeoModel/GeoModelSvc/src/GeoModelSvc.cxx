/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h" 
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelSvc.h"
#include "RDBMaterialManager.h"
#include "GeoModelInterfaces/IGeoAlignTool.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include "GeoModelKernel/GeoPerfUtils.h"
#include <fstream>

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

/**
 ** Constructor(s)
 **/
GeoModelSvc::GeoModelSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc),
    m_detectorTools(this), 
    m_print(false),
    m_pSvcLocator(svc),
    m_pToolSvc(0),
    m_pDetStore(0),
    m_tagInfoMgr(0),
    m_AtlasVersion("AUTO"),
    m_InDetVersion(""),
    m_PixelVersion(""),
    m_SCT_Version(""),
    m_TRT_Version(""),
    m_LAr_Version(""),
    m_TileVersion(""),
    m_MuonVersion(""),
    m_CaloVersion(""),
    m_MagFieldVersion(""),
    m_CavernInfraVersion(""),
    m_ForwardDetectorsVersion(""),
    m_InDetVersionOverride(""),
    m_PixelVersionOverride(""),
    m_SCT_VersionOverride(""),
    m_TRT_VersionOverride(""),
    m_LAr_VersionOverride(""),
    m_TileVersionOverride(""),
    m_MuonVersionOverride(""),
    m_CaloVersionOverride(""),
    m_MagFieldVersionOverride(""),
    m_CavernInfraVersionOverride(""),
    m_ForwardDetectorsVersionOverride(""),
    m_geoConfig(GeoModel::GEO_RUN1),
    m_automaticGeomVersion(false),
    m_callBackON(true),
    m_geoInit(false),
    m_ignoreTagDifference(false),
    m_useTagInfo(true),
    m_useCaloAlign(false),
    m_statisticsToFile(false),
    m_supportedGeometry(0),
    m_ignoreTagSupport(false)
{
  declareProperty( "DetectorTools",               m_detectorTools);
  declareProperty( "PrintMaterials",              m_print);
  declareProperty( "AtlasVersion",                m_AtlasVersion);
  declareProperty( "InDetVersionOverride",        m_InDetVersionOverride);
  declareProperty( "PixelVersionOverride",        m_PixelVersionOverride);
  declareProperty( "SCT_VersionOverride",         m_SCT_VersionOverride);
  declareProperty( "TRT_VersionOverride",         m_TRT_VersionOverride);
  declareProperty( "LAr_VersionOverride",         m_LAr_VersionOverride);
  declareProperty( "TileVersionOverride",         m_TileVersionOverride);
  declareProperty( "MuonVersionOverride",         m_MuonVersionOverride);
  declareProperty( "CaloVersionOverride",         m_CaloVersionOverride);
  declareProperty( "MagFieldVersionOverride",     m_MagFieldVersionOverride);
  declareProperty( "CavernInfraVersionOverride",  m_CavernInfraVersionOverride);
  declareProperty( "ForwardDetectorsVersionOverride",  m_ForwardDetectorsVersionOverride);
  declareProperty( "AutomaticGeometryVersion",    m_automaticGeomVersion);
  declareProperty( "AlignCallbacks",              m_callBackON);
  declareProperty( "IgnoreTagDifference",         m_ignoreTagDifference);
  declareProperty( "UseTagInfo",                  m_useTagInfo);
  declareProperty( "StatisticsToFile",            m_statisticsToFile);
  declareProperty( "SupportedGeometry",           m_supportedGeometry);
  declareProperty( "IgnoreTagSupport",            m_ignoreTagSupport);
}


/**
 ** Destructor
 **/
GeoModelSvc::~GeoModelSvc()
{
}

/**
 ** Initialize Service
 **/
StatusCode
GeoModelSvc::initialize()
{
  if(m_supportedGeometry==0) {
    ATH_MSG_FATAL("The Supported Geometry flag was not set in Job Options! Exiting ...");
    return StatusCode::FAILURE;
  }

  StatusCode result = service("DetectorStore", m_pDetStore );  
  if (result.isFailure()) 
  {
    ATH_MSG_FATAL("DetectorStore service not found!");
    return result;
  }

  // --- Sebastien
  // clients (detector tools) are assuming the DetDescrCnvSvc has been
  // correctly initialized.
  // We ensure this is indeed correct by manually initialize it so there is
  // no more service-ordering problem triggered by jobO mix-up
  ServiceHandle<IConversionSvc> svc("DetDescrCnvSvc", this->name());
  if(!svc.retrieve().isSuccess()) 
  {
    ATH_MSG_FATAL("Could not retrieve service [" << svc.typeAndName() << "] !");
    return StatusCode::FAILURE;
  } 
  else 
  {
    ATH_MSG_DEBUG("retrieved service [" << svc.typeAndName() << "]");
  }
  // --- Sebastien

  if(m_detectorTools.retrieve().isFailure()) 
  {
    ATH_MSG_ERROR("Failed to retreive " << m_detectorTools);
    return StatusCode::FAILURE;
  } 
  else 
  {
    ATH_MSG_INFO("Retrieved " << m_detectorTools);
  }

  ToolHandleArray< IGeoModelTool >::iterator itPriv = m_detectorTools.begin(),
    itPrivEnd = m_detectorTools.end();

  // **** **** **** TagInfo **** **** ****
  const DataHandle<TagInfo> tagInfoH;
  std::string tagInfoKey = "";

  if(m_useTagInfo)
  {
    // get the key
    result = service("TagInfoMgr", m_tagInfoMgr);
 	  	 
    if(result.isFailure() || m_tagInfoMgr==0)
      ATH_MSG_WARNING("Unable to locate TagInfoMgr service");
    else
      tagInfoKey = m_tagInfoMgr->tagInfoKey();
  }

  // Locate and cache the Tool Service
  result = service( "ToolSvc", m_pToolSvc );
  if(result.isFailure())
  {
    ATH_MSG_ERROR("Could not locate ToolSvc");
    return result;
  }
  
  // dummy parameters for the callback
  int par1 = 0;
  std::list<std::string> par2;

  IGeoAlignTool* calo_align = 0;

  if(!m_automaticGeomVersion)
  {
    if(!m_callBackON)
    {
      //
      // **** **** **** Geometry configured manually through jobOptions **** **** ****
      // **** **** **** ** Align functions NOT registered as callbacks **** **** ****
      //
      
      // build regular geometry
      result = geoInit(par1,par2);
      if (result.isFailure())
      { 
	ATH_MSG_FATAL("Unable to build the detector geometry!");
	return result;
      }

      // apply possible alignments for detectors
      itPriv = m_detectorTools.begin();
      for (; itPriv!=itPrivEnd; ++itPriv)
	if((*itPriv)->align(par1,par2) != StatusCode::SUCCESS)
	  ATH_MSG_DEBUG("align failed for the tool");


      // In this case we are not comparing automatic and manual configuration.
      // Simply fill in the contents of TagInfo

      if(m_useTagInfo)
      {
	if(fillTagInfo() != StatusCode::SUCCESS)
	{
	  ATH_MSG_WARNING("Unable to fill Tag Info ");
	  return StatusCode::FAILURE;
	}
	else
	  ATH_MSG_DEBUG("Tag info filled successfully");
      }
    }
    else
    {
      //
      // **** **** **** Geometry configured manually through jobOptions **** **** ****
      // **** **** **** ** Align functions registered as callbacks **** **** ****
      //
      result = geoInit(par1,par2);
      if (result.isFailure())
      { 
	ATH_MSG_FATAL("Unable to build the detector geometry!");
	return result;
      }

      // I want to register IGeoModelSvc::align() even if no alignment callback is registered by
      // subsystem tool. So that clients like CaloTowerBuilder can simply go after IGeoModelSvc::align()
      bool alignRegistered = false;

      // register align() functions for all Tools 
      itPriv = m_detectorTools.begin();
      for (; itPriv!=itPrivEnd; ++itPriv)
      {
	IGeoModelTool* theTool = &(**itPriv);
	
	if(StatusCode::SUCCESS != theTool->registerCallback(m_pDetStore))
	  ATH_MSG_DEBUG("IGeoModelTool::align() was not registerred on CondDB object");
	else
	{
	  if(StatusCode::SUCCESS == m_pDetStore->regFcn(&IGeoModelTool::align,theTool,
							&IGeoModelSvc::align,dynamic_cast<IGeoModelSvc*>(this)))
	  {
	    ATH_MSG_DEBUG("IGeoModelSvc::align() callback registered");
	    alignRegistered = true;

	    // Set useCaloAlign flag if the successful tool is LAr
	    if((*itPriv).typeAndName().find("LAr")!=std::string::npos)
	      m_useCaloAlign = true;
	  }
	  else
	    ATH_MSG_DEBUG("Unable to register callback on IGeoModelSvc::align()");
	}
      }


      // Retrieve a tool for Calo Alignments
      if(m_useCaloAlign) {
	if(m_pToolSvc->retrieveTool("CaloAlignTool",calo_align)!=StatusCode::SUCCESS)
	  ATH_MSG_INFO("Unable to retrieve CaloAlignTool. No Calo alignments in this job");
	else
	  ATH_MSG_DEBUG("CaloAligntTool retrieved successfully");
      }

      // register a callback on TagInfo in order to compare 
      // manual and automatic geometry configurations
      if(m_useTagInfo)
      {
	if(m_pDetStore->regFcn(&IGeoModelSvc::compareTags,dynamic_cast<IGeoModelSvc*>(this), tagInfoH, tagInfoKey) != StatusCode::SUCCESS)
	  ATH_MSG_WARNING("Cannot register compareTags function for key "  << tagInfoKey);
	else
	{
	  ATH_MSG_DEBUG("Registered compareTags callback for key: " << tagInfoKey);

	  if(!alignRegistered)
	  {
	    // There is no successfull alignment callback registration from subsystems
	    // Register IGeoModelSvc::align() after IGeoModelSvc::compareTags() then
	    if(m_pDetStore->regFcn(&IGeoModelSvc::compareTags,dynamic_cast<IGeoModelSvc*>(this),
				   &IGeoModelSvc::align,dynamic_cast<IGeoModelSvc*>(this)) == StatusCode::SUCCESS)
	      ATH_MSG_DEBUG("Registered IGeoModelSvc::align() after IGeoModelSvc::compareTags()");
	    else
	      ATH_MSG_WARNING("Cannot register IGeoModelSvc::align() after IGeoModelSvc::compareTags()");
	  }
	}

	// Fill Tag Info here because it seems to cause troubles when this is done in callback
	if(fillTagInfo() != StatusCode::SUCCESS)
	{
	  ATH_MSG_WARNING("Unable to fill Tag Info ");
	  return StatusCode::FAILURE;
	}
	else
	  ATH_MSG_DEBUG("Tag info filled successfully");
      }
    }
  }
  else
  {
    //
    // **** **** **** Automatic geometry configuration through TagInfo **** **** ****
    //
    
    // Register callback to check when TagInfo has changed
    result =  m_pDetStore->regFcn(&IGeoModelSvc::geoInit,dynamic_cast<IGeoModelSvc*>(this), tagInfoH, tagInfoKey);

    if (result.isFailure()) 
    {
      ATH_MSG_FATAL("Cannot register geoInit function for key "  << tagInfoKey);
      return result;
    }
    else
      ATH_MSG_DEBUG("Registered geoInit callback for key: " << tagInfoKey);

    // I want to register IGeoModelSvc::align() even if no alignment callback is registered by
    // subsystem tool. So that DD clients can go after IGeoModelSvc::align()
    bool alignRegistered = false;

    // register align() functions for all Tools 
    // *** 1. after geoInit
    // *** 2. for their specific objects

    itPriv = m_detectorTools.begin();
    for (; itPriv!=itPrivEnd; ++itPriv) 
    {
      IGeoModelTool* theTool = &(**itPriv);
      if(StatusCode::SUCCESS != m_pDetStore->regFcn(&IGeoModelSvc::geoInit,dynamic_cast<IGeoModelSvc*>(this),
						    &IGeoModelTool::align,theTool))
	ATH_MSG_DEBUG("Unable to register callback on IGeoModelTool::align() after GeoModelSvc::geoInit()");
      else
      {
	if(StatusCode::SUCCESS != theTool->registerCallback(m_pDetStore))
	  ATH_MSG_DEBUG("IGeoModelTool::align() was not registerred on CondDB object");
	else
	{
	  if(StatusCode::SUCCESS == m_pDetStore->regFcn(&IGeoModelTool::align,theTool,
							&IGeoModelSvc::align,dynamic_cast<IGeoModelSvc*>(this)))
	  {
	    ATH_MSG_DEBUG("IGeoModelSvc::align() callback registered");
	    alignRegistered = true;

	    // Set useCaloAlign flag if the successful tool is LAr
	    if((*itPriv).typeAndName().find("LAr")!=std::string::npos)
	      m_useCaloAlign = true;
	  }
	  else
	    ATH_MSG_DEBUG("Unable to register callback on IGeoModelSvc::align()");
	}
      }      
    }

    if(!alignRegistered)
    {
      // There is no successfull alignment callback registration from subsystems
      // Register IGeoModelSvc::align() after IGeoModelSvc::geoInit() then
      if(m_pDetStore->regFcn(&IGeoModelSvc::geoInit,dynamic_cast<IGeoModelSvc*>(this),
			     &IGeoModelSvc::align,dynamic_cast<IGeoModelSvc*>(this)) == StatusCode::SUCCESS)
	ATH_MSG_DEBUG("Registered IGeoModelSvc::align() after IGeoModelSvc::geoInit()");
      else
	ATH_MSG_WARNING("Cannot register IGeoModelSvc::align() after IGeoModelSvc::geoInit()");
    }

    if(m_useCaloAlign) {
      if(m_pToolSvc->retrieveTool("CaloAlignTool",calo_align)!=StatusCode::SUCCESS)
	ATH_MSG_INFO("Unable to retrieve CaloAlignTool. No Calo alignments in this job");
      else
	ATH_MSG_DEBUG("CaloAligntTool retrieved successfully");
    }

  }
  return result;
}

/**
 ** Finalize Service
 **/
StatusCode
GeoModelSvc::finalize()
{
	return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode
GeoModelSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
   if ( IGeoModelSvc::interfaceID().versionMatch(riid) )    {
     *ppvInterface = (IGeoModelSvc*)this;
     addRef();
     return StatusCode::SUCCESS;
   } else  {
     // Interface is not directly available: try out a base class
     return AthService::queryInterface(riid, ppvInterface);
   }

}

StatusCode 
GeoModelSvc::geoInit(IOVSVC_CALLBACK_ARGS)
{
  if(m_geoInit)
    return  StatusCode::SUCCESS;
  else
  {
    StatusCode result;
    m_geoInit = true; 

    // This variable is declared here because only
    // muon id filename is specified explicitely if the automatic
    // configuration fails
    std::string muonIDFileName = "";

    if(m_automaticGeomVersion)
    {
      // Get TagInfo and retrieve tags
      const TagInfo* tagInfo = 0;
      result = m_pDetStore->retrieve(tagInfo);

      if (result.isFailure() || tagInfo==0) 
      {
	ATH_MSG_DEBUG("No TagInfo in DetectorStore while attempting to configure geometry automatically! Using DC2 configuration");
	m_AtlasVersion = "ATLAS-00";
	muonIDFileName = "IdDictMuonSpectrometer_P.03.xml";
      } 
      else
      {
	// This flag actually indicates that the geoInit method has already been called     
	ATH_MSG_DEBUG("Getting geometry tags from TagInfo");

	m_AtlasVersion="";
	m_InDetVersionOverride="";
	m_PixelVersionOverride="";
	m_SCT_VersionOverride="";
	m_TRT_VersionOverride="";
	m_LAr_VersionOverride="";
	m_TileVersionOverride="";
	m_MuonVersionOverride="";
	m_CaloVersionOverride="";
	m_MagFieldVersionOverride="";
	m_CavernInfraVersionOverride="";
	m_ForwardDetectorsVersionOverride="";

	TagInfo::NameTagPairVec pairs;
	tagInfo->getInputTags(pairs);
	for (unsigned int i = 0; i < pairs.size(); i++) 
	{
	  std::string tagPairName = pairs[i].first;
	  if(tagPairName=="GeoAtlas")
	    m_AtlasVersion = pairs[i].second;
	  else if(tagPairName=="GeoInDet")
	    m_InDetVersionOverride = pairs[i].second;
	  else if(tagPairName=="GeoPixel")
	    m_PixelVersionOverride = pairs[i].second;
	  else if(tagPairName=="GeoSCT")
	    m_SCT_VersionOverride = pairs[i].second;
	  else if(tagPairName=="GeoTRT")
	    m_TRT_VersionOverride = pairs[i].second;
	  else if(tagPairName=="GeoLAr")
	    m_LAr_VersionOverride = pairs[i].second;
	  else if(tagPairName=="GeoTile")
	    m_TileVersionOverride = pairs[i].second;
	  else if(tagPairName=="GeoMuon")
	    m_MuonVersionOverride = pairs[i].second;
	  else if(tagPairName=="GeoCalo")
	    m_CaloVersionOverride = pairs[i].second;
	  else if(tagPairName=="GeoMagField")
	    m_MagFieldVersionOverride = pairs[i].second;
	  else if(tagPairName=="GeoCavernInfra")
	    m_CavernInfraVersionOverride = pairs[i].second;
	  else if(tagPairName=="GeoForwardDetectors")
	    m_ForwardDetectorsVersionOverride = pairs[i].second;
	}
      }
    }

    ATH_MSG_DEBUG("** Building geometry configuration: ");
    ATH_MSG_DEBUG("* ATLAS tag: " << m_AtlasVersion);
    ATH_MSG_DEBUG("* InDet tag: " << m_InDetVersionOverride);
    ATH_MSG_DEBUG("* Pixel tag: " << m_PixelVersionOverride);
    ATH_MSG_DEBUG("* SCT   tag: " << m_SCT_VersionOverride);
    ATH_MSG_DEBUG("* TRT   tag: " << m_TRT_VersionOverride);
    ATH_MSG_DEBUG("* LAr   tag: " << m_LAr_VersionOverride);
    ATH_MSG_DEBUG("* Tile  tag: " << m_TileVersionOverride);
    ATH_MSG_DEBUG("* Muon  tag: " << m_MuonVersionOverride);
    ATH_MSG_DEBUG("* Calo  tag: " << m_CaloVersionOverride);
    ATH_MSG_DEBUG("* MagField  tag: " << m_MagFieldVersionOverride);
    ATH_MSG_DEBUG("* CavernInfra  tag: " << m_CavernInfraVersionOverride);
    ATH_MSG_DEBUG("* ForwardDetectors  tag: " << m_ForwardDetectorsVersionOverride);

    // GetRDBAccessSvc and open connection to DB
    IRDBAccessSvc* rdbAccess = 0;
    result = service("RDBAccessSvc",rdbAccess);
    
    if(result.isFailure()) {
      ATH_MSG_ERROR("Unable to get RDBAccessSvc");
      return result;
    }

    if(!rdbAccess->connect()) {
      ATH_MSG_ERROR("Unable to connect to the Geometry DB");
      return StatusCode::FAILURE;
    }

    // Check the existence of ATLAS tag in the database
    if(rdbAccess->getChildTag("ATLAS",m_AtlasVersion,"ATLAS")=="") {
      ATH_MSG_FATAL(" *** *** Wrong ATLAS layout: " << m_AtlasVersion << " *** ***");
      ATH_MSG_FATAL(" Either ATLAS geometry tag has been misspelled, or the DB Release does not contain the geometry specified.");
      ATH_MSG_FATAL(" In latter case please update DB Release version");
      return StatusCode::FAILURE;
    }

    if(!m_ignoreTagSupport) {
      RDBTagDetails atlasTagDetails = rdbAccess->getTagDetails(m_AtlasVersion);
      const coral::AttributeSpecification& supportedSpec = atlasTagDetails["SUPPORTED"].specification();
      if(supportedSpec.type()==typeid(bool)) {
	if(!atlasTagDetails["SUPPORTED"].data<bool>()) {
	  ATH_MSG_FATAL(" *** *** ATLAS layout " << m_AtlasVersion << " is OBSOLETE and can NOT be supported any more! *** ***");
	  return StatusCode::FAILURE;
	}
      }
      else if(supportedSpec.type()==typeid(int)) {
	if(atlasTagDetails["SUPPORTED"].data<int>()<m_supportedGeometry) {
	  ATH_MSG_FATAL(" *** *** ATLAS layout " << m_AtlasVersion 
			<< " is OBSOLETE in rel " << m_supportedGeometry 
			<< " and can NOT be supported any more! *** ***");
	  return StatusCode::FAILURE;
	}
      }
    }

    //
    // Create a material manager.., put it in storegate.
    //
    StoredMaterialManager *theMaterialManager = new RDBMaterialManager(m_pDetStore,m_pSvcLocator);
    result = m_pDetStore->record(theMaterialManager, "MATERIALS" );

    if(result.isFailure()) {
      ATH_MSG_DEBUG("Unable to record Stored Material Manager");
      return result;
    }

    // Get top level tags for subsystems from database

    if(m_InDetVersionOverride=="")
      m_InDetVersion = rdbAccess->getChildTag("InnerDetector",m_AtlasVersion,"ATLAS");
    else
      m_InDetVersion = m_InDetVersionOverride;
    
    if(m_PixelVersionOverride=="")
      m_PixelVersion = rdbAccess->getChildTag("Pixel",m_InDetVersion,"InnerDetector");
    else
      m_PixelVersion = m_PixelVersionOverride;

    if(m_SCT_VersionOverride=="")
      m_SCT_Version = rdbAccess->getChildTag("SCT",m_InDetVersion,"InnerDetector");
    else
      m_SCT_Version = m_SCT_VersionOverride;
    
    if(m_TRT_VersionOverride=="")
      m_TRT_Version = rdbAccess->getChildTag("TRT",m_InDetVersion,"InnerDetector");
    else
      m_TRT_Version = m_TRT_VersionOverride;
    
    if(m_LAr_VersionOverride=="")
      m_LAr_Version = rdbAccess->getChildTag("LAr",m_AtlasVersion,"ATLAS");
    else
      m_LAr_Version = m_LAr_VersionOverride;
    
    if(m_TileVersionOverride=="")
      m_TileVersion = rdbAccess->getChildTag("TileCal",m_AtlasVersion,"ATLAS");
    else
      m_TileVersion = m_TileVersionOverride;
    
    if(m_MuonVersionOverride=="")
      m_MuonVersion = rdbAccess->getChildTag("MuonSpectrometer",m_AtlasVersion,"ATLAS");
    else
      m_MuonVersion = m_MuonVersionOverride;
    
    if(m_CaloVersionOverride=="")
      m_CaloVersion = rdbAccess->getChildTag("Calorimeter",m_AtlasVersion,"ATLAS");
    else
      m_CaloVersion = m_CaloVersionOverride;

    if(m_MagFieldVersionOverride=="")
      m_MagFieldVersion = rdbAccess->getChildTag("MagneticField",m_AtlasVersion,"ATLAS");
    else
      m_MagFieldVersion = m_MagFieldVersionOverride;

    if(m_CavernInfraVersionOverride=="")
      m_CavernInfraVersion = rdbAccess->getChildTag("CavernInfra",m_AtlasVersion,"ATLAS");
    else
      m_CavernInfraVersion = m_CavernInfraVersionOverride;

    if(m_ForwardDetectorsVersionOverride=="")
      m_ForwardDetectorsVersion = rdbAccess->getChildTag("ForwardDetectors",m_AtlasVersion,"ATLAS");
    else
      m_ForwardDetectorsVersion = m_ForwardDetectorsVersionOverride;

    // Retrieve geometry config information from the database (RUN1, RUN2, etc...)
    IRDBRecordset_ptr atlasCommonRec = rdbAccess->getRecordsetPtr("AtlasCommon",m_AtlasVersion,"ATLAS");
    if(atlasCommonRec->size()==0) {
      m_geoConfig = GeoModel::GEO_RUN1;
    }
    else {
      std::string configVal = (*atlasCommonRec)[0]->getString("CONFIG");
      if(configVal=="RUN1")
	m_geoConfig = GeoModel::GEO_RUN1;
      else if(configVal=="RUN2")
	m_geoConfig = GeoModel::GEO_RUN2;
      else if(configVal=="RUN3")
	m_geoConfig = GeoModel::GEO_RUN2;
      else if(configVal=="RUN4")
	m_geoConfig = GeoModel::GEO_RUN4;
      else if(configVal=="ITk")
	m_geoConfig = GeoModel::GEO_ITk;
      else if(configVal=="TESTBEAM")
	m_geoConfig = GeoModel::GEO_TESTBEAM;
      else {
	ATH_MSG_FATAL("Unexpected value for geometry config read from the database: " << configVal);
	return StatusCode::FAILURE;
      }
    }

    // Build the world node from which everything else will be suspended
    const GeoMaterial* air = theMaterialManager->getMaterial("std::Air");
	
    const GeoBox* worldBox = new GeoBox(1000*CLHEP::cm,1000*CLHEP::cm, 1000*CLHEP::cm); 
    // Bundle this with a material into a logical volume:                               
    const GeoLogVol* worldLog = new GeoLogVol("WorldLog", worldBox, air);
    // ..And create a physical volume:
    GeoPhysVol *worldPhys=new GeoPhysVol(worldLog);
	
    GeoModelExperiment* theExperiment = new GeoModelExperiment(worldPhys);
    // Register this within the transient detector store
	
    result = m_pDetStore->record( theExperiment, "ATLAS" );
    if ( result.isSuccess( ) ) {
      int mem,cpu;
      std::ofstream* geoModelStats = 0;
      if(m_statisticsToFile) {
	geoModelStats = new std::ofstream("GeoModelStatistics");
	*geoModelStats << "Detector Configuration flag = " << m_AtlasVersion << std::endl; 
      }

      // Loop over all tools
      ToolHandleArray< IGeoModelTool >::iterator itPriv = m_detectorTools.begin(),
	itPrivEnd = m_detectorTools.end();

      for (; itPriv!=itPrivEnd; ++itPriv) 
      {
	IGeoModelTool* theTool = &(**itPriv);
	
	mem = GeoPerfUtils::getMem();
	cpu = GeoPerfUtils::getCpu();

	result = theTool->create( m_pDetStore );
	
	if( result.isFailure() ) 
	{
	  ATH_MSG_ERROR("Unable to create detector " << theTool->name());
	  return result;
	}
	
	if(m_statisticsToFile)
	  *geoModelStats << theTool->name() << "\t SZ= " << 
	    GeoPerfUtils::getMem() - mem << "Kb \t Time = " << (GeoPerfUtils::getCpu() - cpu) * 0.01 << "S" << std::endl;
	else
	  ATH_MSG_INFO(theTool->name() << "\t SZ= " << GeoPerfUtils::getMem() - mem << "Kb \t Time = " << (GeoPerfUtils::getCpu() - cpu) * 0.01 << "S");
	
      }

      if(m_statisticsToFile) {
	geoModelStats->close();
	delete geoModelStats;
      }
    } else {
      ATH_MSG_ERROR("Could not register ATLAS Experiment within transient detector store");
      result = StatusCode::FAILURE;
    }
	
    // Close DB connection
    rdbAccess->shutdown();
    if(m_print)
      theMaterialManager->printAll();

    return result;
  }
}

StatusCode 
GeoModelSvc::align(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_DEBUG("GeoModelSvc::align() called");
  return StatusCode::SUCCESS;
}

StatusCode 
GeoModelSvc::compareTags(IOVSVC_CALLBACK_ARGS)
{
  StatusCode result;
  bool tagsMatch = true;  

  ATH_MSG_DEBUG("GeoModelSvc::compareTags() callback trigerred");

  // Get TagInfo and retrieve tags
  const TagInfo* tagInfo = 0;
  result = m_pDetStore->retrieve(tagInfo);

  if (result.isFailure() || tagInfo==0) 
    ATH_MSG_DEBUG("No TagInfo in DetectorStore while attempting to compare tags");
  else
  {
    ATH_MSG_DEBUG("TagInfo retrieved from Detector Store");

    TagInfo::NameTagPairVec pairs;
    tagInfo->getInputTags(pairs);
    for (unsigned int i = 0; i < pairs.size(); i++) 
    {
      std::string tagPairName = pairs[i].first;
      if(tagPairName=="GeoAtlas")
      {
	// ** Two possible cases need to be taken into account
	// ** 
	// ** 1. The retrieved ATLAS tag is following naming schema ATLAS-...-XX-YY-ZZ
	// ** where '...' can be anything, it may also containg one or more '-'.
	// ** If this is the case, then we need to check whether the job option tag
	// ** is also following the same schema and they have the same 'ATLAS-...-XX' part
	// **
	// ** 2. The retrieved ATLAS tag is not following the schema mentioned above
	// ** If this is the case, we just need to check the exact match

	std::vector<std::string> tokensTagInfo, tokensJobOpt;

	// Parse Tag Info tag
	std::string::size_type startpos = 0;
	std::string currStr = pairs[i].second;
	for(std::string::size_type endpos=currStr.find("-"); endpos!=std::string::npos; endpos=currStr.find("-",startpos)) {
	  tokensTagInfo.push_back(currStr.substr(startpos,endpos-startpos));
	  startpos = endpos+1;
	}
	tokensTagInfo.push_back(currStr.substr(startpos));

	size_t tokensTagInfoSize = tokensTagInfo.size();
	bool tagInfoFollowsTheScheme = (tokensTagInfoSize>=5 
					&& tokensTagInfo[tokensTagInfoSize-1].size()==2
					&& tokensTagInfo[tokensTagInfoSize-2].size()==2
					&& tokensTagInfo[tokensTagInfoSize-3].size()==2);

	if(tagInfoFollowsTheScheme) {
	  // Parse Job Options tag
	  startpos = 0;
	  currStr = m_AtlasVersion;
	  for(std::string::size_type endpos=currStr.find("-"); endpos!=std::string::npos; endpos=currStr.find("-",startpos)) {
	    tokensJobOpt.push_back(currStr.substr(startpos,endpos-startpos));
	    startpos = endpos+1;
	  }
	  tokensJobOpt.push_back(currStr.substr(startpos));

	  size_t tokensJobOptSize = tokensJobOpt.size();
	  bool jobOptFollowsTheScheme = (tokensJobOptSize>=5
					 && tokensJobOpt[tokensJobOptSize-1].size()==2
					 && tokensJobOpt[tokensJobOptSize-2].size()==2
					 && tokensJobOpt[tokensJobOptSize-3].size()==2);
	  if(jobOptFollowsTheScheme) {
	    tagsMatch = (pairs[i].second.substr(0,currStr.size()-6)==m_AtlasVersion.substr(0,m_AtlasVersion.size()-6));
	  }
	  else {
	    tagsMatch = false;
	  }
	}
	else {// Check for the exact match 
	  tagsMatch = m_AtlasVersion == pairs[i].second;
	}
      }
      else if(tagPairName=="GeoInDet")
	tagsMatch = m_InDetVersionOverride == pairs[i].second;
      else if(tagPairName=="GeoPixel")
	tagsMatch = m_PixelVersionOverride == pairs[i].second;
      else if(tagPairName=="GeoSCT")
	tagsMatch = m_SCT_VersionOverride == pairs[i].second;
      else if(tagPairName=="GeoTRT")
	tagsMatch = m_TRT_VersionOverride == pairs[i].second;
      else if(tagPairName=="GeoLAr")
	tagsMatch = m_LAr_VersionOverride == pairs[i].second;
      else if(tagPairName=="GeoTile")
	tagsMatch = m_TileVersionOverride == pairs[i].second;
      else if(tagPairName=="GeoMuon")
	tagsMatch = m_MuonVersionOverride == pairs[i].second;

      if(!tagsMatch) break;
    }

    if(!tagsMatch)
    {
      msg((m_ignoreTagDifference? MSG::WARNING : MSG::ERROR)) 
	<< "*** *** Geometry configured through jobOptions does not match TagInfo tags! *** ***" << endreq;
      ATH_MSG_INFO("** Job Option configuration: ");
      ATH_MSG_INFO("* ATLAS tag: " << m_AtlasVersion);
      ATH_MSG_INFO("* InDet tag: " << m_InDetVersionOverride);
      ATH_MSG_INFO("* Pixel tag: " << m_PixelVersionOverride);
      ATH_MSG_INFO("* SCT   tag: " << m_SCT_VersionOverride);
      ATH_MSG_INFO("* TRT   tag: " << m_TRT_VersionOverride);
      ATH_MSG_INFO("* LAr   tag: " << m_LAr_VersionOverride);
      ATH_MSG_INFO("* Tile  tag: " << m_TileVersionOverride);
      ATH_MSG_INFO("* Muon  tag: " << m_MuonVersionOverride);
      ATH_MSG_INFO("* Calo  tag: " << m_CaloVersionOverride);
      ATH_MSG_INFO("* MagField  tag: " << m_MagFieldVersionOverride);
      ATH_MSG_INFO("* CavernInfra  tag: " << m_CavernInfraVersionOverride);
      ATH_MSG_INFO("* ForwardDetectors  tag: " << m_ForwardDetectorsVersionOverride);
      ATH_MSG_INFO("** TAG INFO configuration: ");
      for (unsigned int i = 0; i < pairs.size(); i++) 
      {
	std::string tagPairName = pairs[i].first;
	if(tagPairName=="GeoAtlas")
	  ATH_MSG_INFO("* ATLAS tag: " << pairs[i].second);
	else if(tagPairName=="GeoInDet")
	  ATH_MSG_INFO("*InDet  tag: " << pairs[i].second);
	else if(tagPairName=="GeoPixel")
	  ATH_MSG_INFO("*Pixel  tag: " << pairs[i].second);
	else if(tagPairName=="GeoSCT")
	  ATH_MSG_INFO("*SCT  tag: " << pairs[i].second);
	else if(tagPairName=="GeoTRT")
	  ATH_MSG_INFO("*TRT  tag: " << pairs[i].second);
	else if(tagPairName=="GeoLAr")
	  ATH_MSG_INFO("*LAr  tag: " << pairs[i].second);
	else if(tagPairName=="GeoTile")
	  ATH_MSG_INFO("*Tile  tag: " << pairs[i].second);
	else if(tagPairName=="GeoMuon")
	  ATH_MSG_INFO("*Muon  tag: " << pairs[i].second);
	else if(tagPairName=="GeoCalo")
	  ATH_MSG_INFO("*Calo  tag: " << pairs[i].second);
	else if(tagPairName=="GeoMagField")
	  ATH_MSG_INFO("*MagField  tag: " << pairs[i].second);
	else if(tagPairName=="GeoCavernInfra")
	  ATH_MSG_INFO("*CavernInfra  tag: " << pairs[i].second);
	else if(tagPairName=="GeoForwardDetectors")
	  ATH_MSG_INFO("*ForwardDetectors  tag: " << pairs[i].second);
      }

      if(!m_ignoreTagDifference) {
	ATH_MSG_INFO("*** *** Please fix geometry tag settings *** ***");
	return StatusCode::FAILURE;
      }
    }
    else
      ATH_MSG_DEBUG("Geometry configurations in jobOptions and TagInfo are consistent");
  }
  return StatusCode::SUCCESS;
}


/**********************************************************************************
 ** Private Member Functions
 **********************************************************************************/

bool GeoModelSvc::geoInitialized() const
{
  return m_geoInit;
}

StatusCode GeoModelSvc::fillTagInfo() const
{
  if(m_AtlasVersion == "")
  {
    ATH_MSG_ERROR("ATLAS version is empty");
    return StatusCode::FAILURE; 
  }
  
  StatusCode status = m_tagInfoMgr->addTag("GeoAtlas",m_AtlasVersion);
  if(status.isFailure())
  {
    ATH_MSG_WARNING("GeoModelSvc Atlas tag: " << m_AtlasVersion	 
	<< " not added to TagInfo ");
    return status;
  }

  if(m_InDetVersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoInDet",m_InDetVersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc InDet tag: " << m_InDetVersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  } 

  if(m_PixelVersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoPixel",m_PixelVersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc Pixel tag: " << m_PixelVersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  }

  if(m_SCT_VersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoSCT",m_SCT_VersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc SCT tag: " << m_SCT_VersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  }

  if(m_TRT_VersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoTRT",m_TRT_VersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc TRT tag: " << m_TRT_VersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  }

  if(m_LAr_VersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoLAr",m_LAr_VersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc LAr tag: " << m_LAr_VersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  }

  if(m_TileVersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoTile",m_TileVersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc Tile tag: " << m_TileVersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  }

  if(m_MuonVersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoMuon",m_MuonVersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc Muon tag: " << m_MuonVersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  }

  if(m_CaloVersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoCalo",m_CaloVersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc Calo tag: " << m_CaloVersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  }

  if(m_MagFieldVersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoMagField",m_MagFieldVersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc MagField tag: " << m_MagFieldVersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  }

  if(m_CavernInfraVersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoCavernInfra",m_CavernInfraVersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc CavernInfra tag: " << m_CavernInfraVersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  }

  if(m_ForwardDetectorsVersionOverride != "")
  {
    status = m_tagInfoMgr->addTag("GeoForwardDetectors",m_ForwardDetectorsVersionOverride);
    if(status.isFailure())
    {
      ATH_MSG_WARNING("GeoModelSvc ForwardDetectors tag: " << m_ForwardDetectorsVersionOverride
	  << " not added to TagInfo ");
      return status;
    }
  }

  return StatusCode::SUCCESS;
}

const IGeoModelTool* GeoModelSvc::getTool(std::string toolName) const
{
  ToolHandleArray< IGeoModelTool >::const_iterator itPriv = m_detectorTools.begin();

  for(; itPriv!=m_detectorTools.end(); itPriv++) {
    IGeoModelTool* theTool = &(**itPriv);
    if(theTool->name().find(toolName)!=std::string::npos)
      return theTool;
  }

  return 0;
}

StatusCode GeoModelSvc::clear()
{
  ATH_MSG_DEBUG("In clear()"); 
  StatusCode retval = StatusCode::SUCCESS;

  // Call clear() for all tools
  ToolHandleArray< IGeoModelTool >::const_iterator itPriv = m_detectorTools.begin();
  for(; itPriv!=m_detectorTools.end(); itPriv++) {
    IGeoModelTool* theTool = &(**itPriv);
    retval = theTool->clear(m_pDetStore);
    if(retval.isFailure()) {
      ATH_MSG_WARNING("clear() failed for the tool: " << theTool->name());
      return retval;
    } else {
      ATH_MSG_DEBUG(theTool->name() << " tool released");
    }
  }

  // Delete GeoModelExperiment - cascade delete of the entire GeoModel tree
  std::vector<std::string>::const_iterator it;
  std::vector<std::string> sgkeysExp;
  m_pDetStore->keys<GeoModelExperiment>(sgkeysExp);
  for(it=sgkeysExp.begin(); it!=sgkeysExp.end(); it++) {
    SG::DataProxy* proxy = m_pDetStore->proxy(ClassID_traits<GeoModelExperiment>::ID(),*it);
    if(proxy) {
      proxy->reset();
      ATH_MSG_DEBUG(*it << " GeoModel experiment released");
    }
  }

  // Release stored material manager
  std::vector<std::string> sgkeysMat;
  m_pDetStore->keys<StoredMaterialManager>(sgkeysMat);
  for(it=sgkeysMat.begin(); it!=sgkeysMat.end(); it++) {
    SG::DataProxy* proxy = m_pDetStore->proxy(ClassID_traits<StoredMaterialManager>::ID(),*it);
    if(proxy) {
      proxy->reset();
      ATH_MSG_DEBUG(*it << " material manager released");
    }
  }

  return retval;
}
