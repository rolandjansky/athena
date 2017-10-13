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
#include "GeoDbTagSvc.h"
#include "GeoModelInterfaces/IGeoAlignTool.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"
#include "CxxUtils/make_unique.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "EventInfo/TagInfo.h"

#include "GeoModelKernel/GeoPerfUtils.h"
#include <fstream>

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

GeoModelSvc::GeoModelSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc),
    m_detectorTools(this), 
    m_pSvcLocator(svc),
    m_toolSvc("ToolSvc",name),
    m_detStore("DetectorStore",name),
    m_tagInfoMgr("TagInfoMgr",name),
    m_geoDbTagSvc("GeoDbTagSvc",name),
    m_AtlasVersion("AUTO"),
    m_printMaterials(false),
    m_callBackON(true),
    m_ignoreTagDifference(false),
    m_useTagInfo(true),
    m_useCaloAlign(false),
    m_statisticsToFile(false),
    m_supportedGeometry(0),
    m_ignoreTagSupport(false)
{
  declareProperty( "DetectorTools",               m_detectorTools);
  declareProperty( "PrintMaterials",              m_printMaterials);
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
  declareProperty( "AlignCallbacks",              m_callBackON);
  declareProperty( "IgnoreTagDifference",         m_ignoreTagDifference);
  declareProperty( "UseTagInfo",                  m_useTagInfo);
  declareProperty( "StatisticsToFile",            m_statisticsToFile);
  declareProperty( "SupportedGeometry",           m_supportedGeometry);
  declareProperty( "IgnoreTagSupport",            m_ignoreTagSupport);
}

GeoModelSvc::~GeoModelSvc()
{
}

StatusCode GeoModelSvc::initialize()
{
  if(m_supportedGeometry==0) {
    ATH_MSG_FATAL("The Supported Geometry flag was not set in Job Options! Exiting ...");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_detStore.retrieve() );
  ATH_CHECK( m_toolSvc.retrieve() );

  // --- Sebastien
  // clients (detector tools) are assuming the DetDescrCnvSvc has been
  // correctly initialized.
  // We ensure this is indeed correct by manually initialize it so there is
  // no more service-ordering problem triggered by jobO mix-up
  ServiceHandle<IConversionSvc> conversionSvc("DetDescrCnvSvc", this->name());
  ATH_CHECK( conversionSvc.retrieve() );
  // --- Sebastien

  ATH_CHECK( m_detectorTools.retrieve() );

  ToolHandleArray< IGeoModelTool >::iterator itPriv = m_detectorTools.begin(),
    itPrivEnd = m_detectorTools.end();

  // **** **** **** TagInfo **** **** ****
  std::string tagInfoKey = "";

  if(m_useTagInfo) {
    // get the key
    ATH_CHECK( m_tagInfoMgr.retrieve() );
    tagInfoKey = m_tagInfoMgr->tagInfoKey();
  }

  // build regular geometry
  ATH_CHECK( geoInit() );

  if(!m_callBackON) {
    // _________________ Align functions NOT registered as callbacks _____________

    // Apply possible alignments to detectors.
    // Dummy parameters for the callback
    int par1 = 0;
    std::list<std::string> par2;
    for(; itPriv!=itPrivEnd; ++itPriv) {
      if((*itPriv)->align(par1,par2) != StatusCode::SUCCESS) {
	ATH_MSG_DEBUG("align() failed for the tool " << (*itPriv)->name());
      }
    }
    
    // Fill in the contents of TagInfo
    if(m_useTagInfo) {
      ATH_CHECK(fillTagInfo());
    }
  }
  else {
    // _________________ Align functions NOT registered as callbacks _____________

    // We want to register IGeoModelSvc::align() even if no alignment callback is registered by
    // subsystem tools, such that clients like CaloTowerBuilder can simply go after IGeoModelSvc::align()
    bool alignRegistered = false;

    // Register align() functions for all Tools 
    for (; itPriv!=itPrivEnd; ++itPriv) {
      IGeoModelTool* theTool = &(**itPriv);
	
      if(StatusCode::SUCCESS != theTool->registerCallback()) {
	ATH_MSG_DEBUG("IGeoModelTool::align() was not registerred on CondDB object for the tool " << theTool->name());
      }
      else {
	if(StatusCode::SUCCESS == m_detStore->regFcn(&IGeoModelTool::align,theTool,
						      &IGeoModelSvc::align,dynamic_cast<IGeoModelSvc*>(this))) {
	  ATH_MSG_DEBUG("IGeoModelSvc::align() callback registered for the tool " << theTool->name());
	  alignRegistered = true;

	  // Set useCaloAlign flag if the successful tool is LAr
	  if((*itPriv).typeAndName().find("LAr")!=std::string::npos) {
	      m_useCaloAlign = true;
	  }
	}
	else {
	  ATH_MSG_DEBUG("Unable to register callback on IGeoModelSvc::align() for the tool " << theTool->name());
	}
      }
    }

    // Retrieve a tool for Calo Alignments
    if(m_useCaloAlign) {
      IGeoAlignTool* calo_align{nullptr};
      if(m_toolSvc->retrieveTool("CaloAlignTool",calo_align)!=StatusCode::SUCCESS) {
	ATH_MSG_INFO("Unable to retrieve CaloAlignTool. No Calo alignments in this job");
      }
      else {
	ATH_MSG_DEBUG("CaloAligntTool retrieved successfully");
      }
    }

    // Register a callback on TagInfo in order to compare geometry configurations defined in job options
    // to the one read from the input file
    if(m_useTagInfo) {
      const DataHandle<TagInfo> tagInfoH;
      if(m_detStore->regFcn(&IGeoModelSvc::compareTags,dynamic_cast<IGeoModelSvc*>(this), tagInfoH, tagInfoKey) != StatusCode::SUCCESS) {
	ATH_MSG_WARNING("Cannot register compareTags function for key "  << tagInfoKey);
      }
      else {
	ATH_MSG_DEBUG("Registered compareTags callback for key: " << tagInfoKey);
	
	if(!alignRegistered) {
	  // There is no successfull alignment callback registration from subsystems
	  // Register IGeoModelSvc::align() after IGeoModelSvc::compareTags() then
	  if(m_detStore->regFcn(&IGeoModelSvc::compareTags,dynamic_cast<IGeoModelSvc*>(this),
				 &IGeoModelSvc::align,dynamic_cast<IGeoModelSvc*>(this)) == StatusCode::SUCCESS) {
	    ATH_MSG_DEBUG("Registered IGeoModelSvc::align() after IGeoModelSvc::compareTags()");
	  }
	  else {
	    ATH_MSG_WARNING("Cannot register IGeoModelSvc::align() after IGeoModelSvc::compareTags()");
	  }
	}
      }

      // Fill in the contents of TagInfo
      ATH_CHECK(fillTagInfo());
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode GeoModelSvc::finalize()
{
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode GeoModelSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if(IGeoModelSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = (IGeoModelSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  } 
  else {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
}

StatusCode GeoModelSvc::geoInit()
{
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
  ServiceHandle<IRDBAccessSvc> rdbAccess("RDBAccessSvc",name());
  ATH_CHECK( rdbAccess.retrieve() );

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

  // Create a material manager
  StoredMaterialManager *theMaterialManager{nullptr};
  try{
    theMaterialManager = new RDBMaterialManager(m_pSvcLocator);
  }
  catch(std::runtime_error& e) {
    ATH_MSG_FATAL(e.what());
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_detStore->record(theMaterialManager,"MATERIALS") );
  
  // Setup the GeoDbTagSvc
  ATH_CHECK( m_geoDbTagSvc.retrieve() );
  
  GeoDbTagSvc* dbTagSvc = dynamic_cast<GeoDbTagSvc*>(m_geoDbTagSvc.operator->());
  if(dbTagSvc==nullptr) {
    ATH_MSG_FATAL("Unable to dyn-cast the IGeoDbTagSvc pointer to GeoDbTagSvc");
    return StatusCode::FAILURE;
  }

  dbTagSvc->setAtlasVersion(m_AtlasVersion);
  dbTagSvc->setInDetVersionOverride(m_InDetVersionOverride);
  dbTagSvc->setPixelVersionOverride(m_PixelVersionOverride);
  dbTagSvc->setSCT_VersionOverride(m_SCT_VersionOverride);
  dbTagSvc->setTRT_VersionOverride(m_TRT_VersionOverride);
  dbTagSvc->setLAr_VersionOverride(m_LAr_VersionOverride);
  dbTagSvc->setTileVersionOverride(m_TileVersionOverride);
  dbTagSvc->setMuonVersionOverride(m_MuonVersionOverride);
  dbTagSvc->setCaloVersionOverride(m_CaloVersionOverride);
  dbTagSvc->setMagFieldVersionOverride(m_MagFieldVersionOverride);
  dbTagSvc->setCavernInfraVersionOverride(m_CavernInfraVersionOverride);
  dbTagSvc->setForwardDetectorsVersionOverride(m_ForwardDetectorsVersionOverride);

  if(dbTagSvc->setupTags().isFailure()) {
    ATH_MSG_FATAL("Failed to setup subsystem tags");
    return StatusCode::FAILURE;
  }

  // Build the world node from which everything else will be suspended
  const GeoMaterial* air = theMaterialManager->getMaterial("std::Air");  
  const GeoBox* worldBox = new GeoBox(1000*CLHEP::cm,1000*CLHEP::cm, 1000*CLHEP::cm); 
  const GeoLogVol* worldLog = new GeoLogVol("WorldLog", worldBox, air);
  GeoPhysVol *worldPhys=new GeoPhysVol(worldLog);
  
  // Create AtlasExperiment and register it within the transient detector store
  GeoModelExperiment* theExperiment = new GeoModelExperiment(worldPhys);
  ATH_CHECK( m_detStore->record(theExperiment,"ATLAS") );

  int mem,cpu;
  std::unique_ptr<std::ofstream> geoModelStats;
  if(m_statisticsToFile) {
    geoModelStats = CxxUtils::make_unique<std::ofstream>("GeoModelStatistics");
    *geoModelStats << "Detector Configuration flag = " << m_AtlasVersion << std::endl; 
  }
    
  // Loop over all tools
  ToolHandleArray< IGeoModelTool >::iterator itPriv = m_detectorTools.begin(),
    itPrivEnd = m_detectorTools.end();
  
  for(; itPriv!=itPrivEnd; ++itPriv) {
    IGeoModelTool* theTool = &(**itPriv);
      
    mem = GeoPerfUtils::getMem();
    cpu = GeoPerfUtils::getCpu();
      
    if(theTool->create().isFailure()) {
      ATH_MSG_ERROR("Unable to create detector " << theTool->name());
      return StatusCode::FAILURE;
    }
      
    if(m_statisticsToFile) {
      *geoModelStats << theTool->name() << "\t SZ= " 
		     << GeoPerfUtils::getMem() - mem << "Kb \t Time = " << (GeoPerfUtils::getCpu() - cpu) * 0.01 << "S" << std::endl;
    }
    else {
      ATH_MSG_INFO(theTool->name() << "\t SZ= " 
		   << GeoPerfUtils::getMem() - mem << "Kb \t Time = " << (GeoPerfUtils::getCpu() - cpu) * 0.01 << "S");
    }
  }
    
  if(m_statisticsToFile) {
    geoModelStats->close();
  }
	
  // Close DB connection
  rdbAccess->shutdown();
  if(m_printMaterials)
    theMaterialManager->printAll();
  
  return StatusCode::SUCCESS;
}

StatusCode GeoModelSvc::align(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_DEBUG("GeoModelSvc::align() called");
  return StatusCode::SUCCESS;
}

StatusCode GeoModelSvc::compareTags(IOVSVC_CALLBACK_ARGS)
{
  bool tagsMatch = true;  

  ATH_MSG_DEBUG("GeoModelSvc::compareTags() callback trigerred");

  // Get TagInfo and retrieve tags
  const TagInfo* tagInfo = 0;
  ATH_CHECK( m_detStore->retrieve(tagInfo) );

  TagInfo::NameTagPairVec pairs;
  tagInfo->getInputTags(pairs);
  for( const auto& pair : pairs ) {
    std::string tagPairName = pair.first;
    if(tagPairName=="GeoAtlas") {
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
      std::string currStr = pair.second;
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
	  tagsMatch = (pair.second.substr(0,currStr.size()-6)==m_AtlasVersion.substr(0,m_AtlasVersion.size()-6));
	}
	else {
	  tagsMatch = false;
	}
      }
      else {// Check for the exact match 
	tagsMatch = m_AtlasVersion == pair.second;
      }
    }
    else if(tagPairName=="GeoInDet")
      tagsMatch = m_InDetVersionOverride == pair.second;
    else if(tagPairName=="GeoPixel")
      tagsMatch = m_PixelVersionOverride == pair.second;
    else if(tagPairName=="GeoSCT")
      tagsMatch = m_SCT_VersionOverride == pair.second;
    else if(tagPairName=="GeoTRT")
      tagsMatch = m_TRT_VersionOverride == pair.second;
    else if(tagPairName=="GeoLAr")
      tagsMatch = m_LAr_VersionOverride == pair.second;
    else if(tagPairName=="GeoTile")
      tagsMatch = m_TileVersionOverride == pair.second;
    else if(tagPairName=="GeoMuon")
      tagsMatch = m_MuonVersionOverride == pair.second;
    
    if(!tagsMatch) break;
  }

  if(!tagsMatch) {
    msg((m_ignoreTagDifference? MSG::WARNING : MSG::ERROR)) 
      << "*** *** Geometry configured through jobOptions does not match TagInfo tags! *** ***" << endmsg;
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
    for (const auto& pair : pairs) {
      std::string tagPairName = pair.first;
      if(tagPairName=="GeoAtlas")
	ATH_MSG_INFO("* ATLAS tag: " << pair.second);
      else if(tagPairName=="GeoInDet")
	ATH_MSG_INFO("*InDet  tag: " << pair.second);
      else if(tagPairName=="GeoPixel")
	ATH_MSG_INFO("*Pixel  tag: " << pair.second);
      else if(tagPairName=="GeoSCT")
	ATH_MSG_INFO("*SCT  tag: " << pair.second);
      else if(tagPairName=="GeoTRT")
	ATH_MSG_INFO("*TRT  tag: " << pair.second);
      else if(tagPairName=="GeoLAr")
	ATH_MSG_INFO("*LAr  tag: " << pair.second);
      else if(tagPairName=="GeoTile")
	ATH_MSG_INFO("*Tile  tag: " << pair.second);
      else if(tagPairName=="GeoMuon")
	ATH_MSG_INFO("*Muon  tag: " << pair.second);
      else if(tagPairName=="GeoCalo")
	ATH_MSG_INFO("*Calo  tag: " << pair.second);
      else if(tagPairName=="GeoMagField")
	ATH_MSG_INFO("*MagField  tag: " << pair.second);
      else if(tagPairName=="GeoCavernInfra")
	ATH_MSG_INFO("*CavernInfra  tag: " << pair.second);
      else if(tagPairName=="GeoForwardDetectors")
	ATH_MSG_INFO("*ForwardDetectors  tag: " << pair.second);
    }
    
    if(!m_ignoreTagDifference) {
      ATH_MSG_INFO("*** *** Please fix geometry tag settings *** ***");
      return StatusCode::FAILURE;
    }
  }
  else
    ATH_MSG_DEBUG("Geometry configurations in jobOptions and TagInfo are consistent");

  return StatusCode::SUCCESS;
}

/**********************************************************************************
 ** Private Member Functions
 **********************************************************************************/
StatusCode GeoModelSvc::fillTagInfo() const
{
  if(m_AtlasVersion == "") {
    ATH_MSG_ERROR("ATLAS version is empty");
    return StatusCode::FAILURE; 
  }
  
  if(m_tagInfoMgr->addTag("GeoAtlas",m_AtlasVersion).isFailure()) {
    ATH_MSG_ERROR("GeoModelSvc Atlas tag: " << m_AtlasVersion	<< " not added to TagInfo ");
    return StatusCode::FAILURE; 
  }

  if(m_InDetVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoInDet",m_InDetVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc InDet tag: " << m_InDetVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  } 

  if(m_PixelVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoPixel",m_PixelVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Pixel tag: " << m_PixelVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_SCT_VersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoSCT",m_SCT_VersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc SCT tag: " << m_SCT_VersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_TRT_VersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoTRT",m_TRT_VersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc TRT tag: " << m_TRT_VersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_LAr_VersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoLAr",m_LAr_VersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc LAr tag: " << m_LAr_VersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_TileVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoTile",m_TileVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Tile tag: " << m_TileVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_MuonVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoMuon",m_MuonVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Muon tag: " << m_MuonVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_CaloVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoCalo",m_CaloVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc Calo tag: " << m_CaloVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_MagFieldVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoMagField",m_MagFieldVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc MagField tag: " << m_MagFieldVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_CavernInfraVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoCavernInfra",m_CavernInfraVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc CavernInfra tag: " << m_CavernInfraVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  if(m_ForwardDetectorsVersionOverride != "") {
    if(m_tagInfoMgr->addTag("GeoForwardDetectors",m_ForwardDetectorsVersionOverride).isFailure()) {
      ATH_MSG_ERROR("GeoModelSvc ForwardDetectors tag: " << m_ForwardDetectorsVersionOverride << " not added to TagInfo ");
      return StatusCode::FAILURE; 
    }
  }

  return StatusCode::SUCCESS;
}

const IGeoModelTool* GeoModelSvc::getTool(std::string toolName) const
{
  ToolHandleArray< IGeoModelTool >::const_iterator itPriv = m_detectorTools.begin();

  for(; itPriv!=m_detectorTools.end(); itPriv++) {
    const IGeoModelTool* theTool = &(**itPriv);
    if(theTool->name().find(toolName)!=std::string::npos)
      return theTool;
  }

  return 0;
}

StatusCode GeoModelSvc::clear()
{
  ATH_MSG_DEBUG("In clear()"); 

  // Call clear() for all tools
  ToolHandleArray< IGeoModelTool >::iterator itPriv = m_detectorTools.begin();
  for(; itPriv!=m_detectorTools.end(); itPriv++) {
    IGeoModelTool* theTool = &(**itPriv);
    if(theTool->clear().isFailure()) {
      ATH_MSG_ERROR("clear() failed for the tool: " << theTool->name());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG(theTool->name() << " tool released");
    }
  }

  // Delete GeoModelExperiment - cascade delete of the entire GeoModel tree
  std::vector<std::string>::const_iterator it;
  std::vector<std::string> sgkeysExp;
  m_detStore->keys<GeoModelExperiment>(sgkeysExp);
  for(it=sgkeysExp.begin(); it!=sgkeysExp.end(); it++) {
    SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<GeoModelExperiment>::ID(),*it);
    if(proxy) {
      proxy->reset();
      ATH_MSG_DEBUG(*it << " GeoModel experiment released");
    }
  }

  // Release stored material manager
  std::vector<std::string> sgkeysMat;
  m_detStore->keys<StoredMaterialManager>(sgkeysMat);
  for(it=sgkeysMat.begin(); it!=sgkeysMat.end(); it++) {
    SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<StoredMaterialManager>::ID(),*it);
    if(proxy) {
      proxy->reset();
      ATH_MSG_DEBUG(*it << " material manager released");
    }
  }

  return StatusCode::SUCCESS;
}
