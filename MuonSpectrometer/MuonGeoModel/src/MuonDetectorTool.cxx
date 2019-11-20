/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MuonDetectorTool.h" 
#include "MuonGeoModel/MuonDetectorFactory001.h"
#include "MuonGeoModel/StationSelector.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoPerfUtils.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <fstream>
#include "AthenaKernel/ClassID_traits.h"

using namespace MuonGM;

/**
 ** Constructor(s)
 **/
MuonDetectorTool::MuonDetectorTool( const std::string& type, const std::string& name, const IInterface* parent )
    : GeoModelTool( type, name, parent ),  
      m_layout("R.08"),
      m_accessCondDb(1),
      m_asciiCondData(0),
      m_nova(0),
      m_includeCutouts(0),
      m_includeCutoutsBog(0),
      m_includeCtbBis(0),
      m_fillCache_initTime(0),
      m_dumpMemoryBreakDown(false),
      m_enableFineClashFixing(0),
      m_hasCSC(true),
      m_hasSTgc(true),
      m_hasMM(true),
      m_stationSelection(0),
      m_controlAlines(111111),
      m_dumpAlines(false),
      m_altAsztFile(""),
      m_minimalGeoFlag(false),
      m_useCscIntAlines(false),
      m_controlCscIntAlines(0),
      m_dumpCscIntAlines(false),
      m_useCscIntAlinesFromGM(true),
      m_altCscIntAlinesFile(""),
      m_enableMdtDeformations(0),
      m_enableMdtAsBuiltParameters(0),
      m_altMdtAsBuiltFile(""),
      m_manager(0)
{
    declareInterface<IGeoModelTool>(this);
    declareProperty("LayoutName"			, m_layout );
    declareProperty("UseConditionDb"			, m_accessCondDb);
    declareProperty("UseAsciiConditionData"		, m_asciiCondData);
    declareProperty("BuildFromNova",m_nova);
    declareProperty("IncludeCutouts"			, m_includeCutouts);
    declareProperty("IncludeCutoutsBog"			, m_includeCutoutsBog);
    declareProperty("IncludeCtbBis"			, m_includeCtbBis);
    //
    declareProperty("FillCacheInitTime"			, m_fillCache_initTime = 0);
    declareProperty("DumpMemoryBreakDown"		, m_dumpMemoryBreakDown = false);
    //
    declareProperty("EnableFineClashFixing"		, m_enableFineClashFixing = 0);
    declareProperty("HasCSC", m_hasCSC);
    declareProperty("HasSTgc", m_hasSTgc);
    declareProperty("HasMM", m_hasMM);
    //
    declareProperty("StationSelection"			, m_stationSelection = 0);
    declareProperty("SelectedStations"			, m_selectedStations);
    declareProperty("SelectedStJzz"			, m_selectedStEta);
    declareProperty("SelectedStJff"			, m_selectedStPhi);
    //
    declareProperty("ControlAlines"			, m_controlAlines = 111111); // allowed values are   0 -  111111
    declareProperty("DumpAlines"			, m_dumpAlines = false);
    declareProperty("AlternateASZTFile"			, m_altAsztFile);
    declareProperty("MinimalGeoFlag"                    , m_minimalGeoFlag = 0);
    //
    declareProperty("EnableCscInternalAlignment"	, m_useCscIntAlines = false);
    declareProperty("ControlCscInternalAlines"		, m_controlCscIntAlines = 111111); // allowed values are 0 - 111111
    declareProperty("DumpCscInternalAlines"		, m_dumpCscIntAlines = false);
    declareProperty("UseIlinesFromGM"			, m_useCscIntAlinesFromGM = true);
    declareProperty("AlternateCscIntAlignFile"		, m_altCscIntAlinesFile);
    //
    declareProperty("EnableMdtDeformations"		, m_enableMdtDeformations = 0);
    //
    declareProperty("EnableMdtAsBuiltParameters"	, m_enableMdtAsBuiltParameters = 0);
    declareProperty("AlternateAsBuiltParamAlignFile"    , m_altMdtAsBuiltFile);
    //
    // THESE ALLOW TO RESET THE MUON SWITCHES IN THE oracle TABLES:
    // to reset (for example) BUILDBARRELTOROID use ForceSwitchOnOff_BUILDBARRELTOROID = 1001/1000 to have/not have the BARRELTOROID
    // i.e  you must set 1000 to force resetting + 1/0 (enable/disable)
    // ForceSwitchOnOff_BUILDBARRELTOROID = XX with XX <1000 will have no effect => muon switches from Oracle will be used
    declareProperty("ForceSwitchOnOff_BUILDINERTMATERIALS",  m_switchOnOff_BUILDINERTMATERIALS = 0);
    declareProperty("ForceSwitchOnOff_MINIMALGEO",           m_switchOnOff_MINIMALGEO          = 0);
    declareProperty("ForceSwitchOnOff_BUILDENDCAP",          m_switchOnOff_BUILDENDCAP         = 0);
    declareProperty("ForceSwitchOnOff_BUILDCALOSADDLE",      m_switchOnOff_BUILDCALOSADDLE     = 0);
    declareProperty("ForceSwitchOnOff_BUILDBARRELTOROID",    m_switchOnOff_BUILDBARRELTOROID   = 0);
    declareProperty("ForceSwitchOnOff_BUILDENDCAPTOROID",    m_switchOnOff_BUILDENDCAPTOROID   = 0);
    declareProperty("ForceSwitchOnOff_BUILDFEET",            m_switchOnOff_BUILDFEET           = 0);
    declareProperty("ForceSwitchOnOff_BUILDDISKSHIELD",      m_switchOnOff_BUILDDISKSHIELD     = 0);
    declareProperty("ForceSwitchOnOff_BUILDTOROIDSHIELD",    m_switchOnOff_BUILDTOROIDSHIELD   = 0);
    declareProperty("ForceSwitchOnOff_BUILDFORWARDSHIELD",   m_switchOnOff_BUILDFORWARDSHIELD  = 0);

}

/**
 ** Destructor
 **/
MuonDetectorTool::~MuonDetectorTool()
{
	// This will need to be modified once we register the Muon DetectorNode in
	// the Transient Detector Store
	if ( 0 != m_detector ) {
		delete m_detector;
		m_detector = 0;
	}
}

StatusCode
MuonDetectorTool::initialize()
{
    ATH_MSG_INFO("Initializing ...");

    // Incident Svc 
    ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
    ATH_CHECK( incidentSvc.retrieve() );
    //Cannot remove DE at End of Event (potentially before other components performe some clean-up still using DE) 
    //incidentSvc->addListener(this, "EndEvent");   
    //StoreCleared still takes place at each event and as soon as the event store is cleared
    incidentSvc->addListener(this, "StoreCleared");    
    
    return StatusCode::SUCCESS;
}

void MuonDetectorTool::handle(const Incident& inc)
{
    //go to StoreCleared
    //if (inc.type()=="EndEvent" && m_fillCache_initTime ==0 && m_manager!=0)
    if (inc.type()=="StoreCleared" && m_fillCache_initTime ==0 && m_manager!=0)
    {
        // Do clear cache built up during the event ...
      ATH_MSG_DEBUG ("Clearing cache at end of event after EventStore is cleared"); 
      //only for Mdt tubes which are able to rebuild cache on demand 
      m_manager->clearMdtCache();
    }
}

/**
 ** Create the Detector Node corresponding to this tool
 **/
StatusCode
MuonDetectorTool::create()
{ 

    std::ofstream geoModelStats;
    int mem = 0;
    float cpu = 0;
    int umem = 0;
    float ucpu = 0;
    
    if (m_dumpMemoryBreakDown){
	geoModelStats.open("MuonGeoModelStatistics_MuonDetectorTool");
	mem  = GeoPerfUtils::getMem();
	cpu  = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::create entry point: \t SZ= " <<mem << " Kb \t Time = " << cpu << " seconds " << std::endl; 

	umem = GeoPerfUtils::getMem();
	ucpu = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::resetting to 0:     \t SZ= " <<umem-mem << " Kb \t Time = " << ucpu-cpu << " seconds " << std::endl;
	mem = umem;
	cpu = ucpu;
    }

    ATH_MSG_VERBOSE(" Show properties (user sett.s or default): LayoutName                 "<< m_layout <<endmsg
                  <<" Show properties (user sett.s or default): IncludeCutouts             "<< m_includeCutouts <<endmsg
                  <<" Show properties (user sett.s or default): IncludeCutoutsBog          "<< m_includeCutoutsBog <<endmsg
                  <<" Show properties (user sett.s or default): IncludeCtbBis              "<< m_includeCtbBis );
  
    // Get the detector configuration.
    IGeoModelSvc *geoModel;
    service ("GeoModelSvc",geoModel);
  
    std::string AtlasVersion = geoModel->atlasVersion();
    std::string MuonVersion  = geoModel->muonVersionOverride();

    ATH_MSG_INFO("create MuonDetectorTool - package version = "<<PACKAGE_VERSION );
    ATH_MSG_INFO("(from GeoModelSvc)    AtlasVersion = <"<< AtlasVersion<<">  MuonVersion = <"<<MuonVersion<<">" );

    // Unless we are using custom muons, the switch positions are going to
    // come from the database:

    std::string detectorKey  = MuonVersion.empty() ? AtlasVersion : MuonVersion;
    std::string detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";
    ATH_MSG_INFO("Keys for Muon Switches are  (key) "  << detectorKey  << " (node) " << detectorNode );

    std::map<std::string,std::string> altAsciiDBMap = std::map<std::string,std::string>();
    if ( MuonVersion == "CUSTOM" ) 
        ATH_MSG_WARNING("Detector Information coming from a custom configuration !!" );
    else
    {
        ATH_MSG_DEBUG("Detector Information coming from the database (job options IGNORED)" );
        IRDBAccessSvc *accessSvc;
        service("RDBAccessSvc",accessSvc);
        IRDBRecordset_ptr switchSet = accessSvc->getRecordsetPtr("MuonSwitches", detectorKey, detectorNode);
        if ((*switchSet).size()==0) return StatusCode::FAILURE;
        const IRDBRecord    *switches   = (*switchSet)[0];

        m_layout                = switches->getString("LAYOUTNAME");
        //       m_includeInertMaterials = switches->getInt("BUILDINERTMATERIALS");
        //       m_minimalgeo            = switches->getInt("MINIMALGEO");
        if (MuonVersion == "") {
            MuonVersion = accessSvc->getChildTag("MuonSpectrometer",detectorKey,detectorNode);
            ATH_MSG_INFO("(from GeoModelSvc) in AtlasVersion = <"<< AtlasVersion<<">  default MuonVersion is <"
                          <<MuonVersion<<">" );
        }

        ATH_MSG_DEBUG(" m_altAsztFile: "<<m_altAsztFile );
        // use ascii file to read in ASZT parameters
	if (m_altAsztFile != "" )
	  altAsciiDBMap.insert(std::make_pair("ASZT",m_altAsztFile));  	
	if (m_altCscIntAlinesFile != "") 
	  altAsciiDBMap.insert(std::make_pair("IACSC",m_altCscIntAlinesFile));  	
	if (m_altMdtAsBuiltFile != "") 
	  altAsciiDBMap.insert(std::make_pair("XAMDT",m_altMdtAsBuiltFile));  	
    }
  
    // 
    // Locate the top level experiment node 
    // 
    GeoModelExperiment* theExpt = nullptr;
    ATH_CHECK( detStore()->retrieve( theExpt, "ATLAS" ) );

    if (!m_useCscIntAlines) m_controlCscIntAlines = 0;
    ATH_MSG_INFO("Properties have been set as follows: " <<endmsg
               <<"    LayoutName                     "<< m_layout.substr(0,1) <<endmsg
               <<"    IncludeCutouts                 "<< m_includeCutouts <<endmsg
               <<"    IncludeCutoutsBog              "<< m_includeCutoutsBog <<endmsg
               <<"    IncludeCtbBis                  "<< m_includeCtbBis <<endmsg
               <<"    ControlAlines                  "<< m_controlAlines <<endmsg
               <<"    MinimalGeoFlag                 "<< m_minimalGeoFlag <<endmsg
               <<"    EnableCscIntAlignment          "<< m_useCscIntAlines <<endmsg
               <<"    EnableCscIntAlignmentFromGM    "<< m_useCscIntAlinesFromGM );
    if (m_useCscIntAlines) ATH_MSG_INFO("    ControlCscIntAlines            "<< m_controlCscIntAlines );
    else ATH_MSG_INFO("    ControlCscIntAlines   reset to "<< m_controlCscIntAlines );
    ATH_MSG_INFO("    EnableMdtDeformations          "<< m_enableMdtDeformations );
    ATH_MSG_INFO("    EnableMdtAsBuiltParameters     "<< m_enableMdtAsBuiltParameters );

    if ( m_stationSelection > 0) {
		StationSelector::SetSelectionType( m_stationSelection);
        if ( (m_selectedStations.size()+ m_selectedStEta.size()+ m_selectedStPhi.size()) < 1){
            ATH_MSG_ERROR( " **** Badly set Option "<< endmsg
                            << " **** SelectedStations size =" << m_selectedStations.size()<<endmsg
                            << " **** SelectedStJzz    size =" << m_selectedStations.size()<<endmsg
                            << " **** SelectedStJff    size =" << m_selectedStations.size()<<endmsg
                            << " **** while StationSelection = 1");
            return( StatusCode::FAILURE );
        }
        for (unsigned int i=0; i<m_selectedStations.size() ; i++){
            ATH_MSG_INFO("          Selected stations      " << m_selectedStations[i] );
        }
  
        if ( m_selectedStEta.size() > 0 )
        {
            for (unsigned int i=0; i<m_selectedStEta.size() ; i++)
                ATH_MSG_INFO("          Selected Jzz locations  " << m_selectedStEta[i] );
        }
        if ( m_selectedStPhi.size() > 0 )
        {
            for (unsigned int i=0; i<m_selectedStPhi.size() ; i++)
                ATH_MSG_INFO("          Selected Jff locations  " << m_selectedStPhi[i] );
        }
    }
      
    if (m_dumpMemoryBreakDown)
    {
	umem = GeoPerfUtils::getMem();
	ucpu = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::material mgr done   \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
    }
    
    if ( 0 == m_detector ) {
        IRDBAccessSvc* access = 0;
        service("RDBAccessSvc",access);

        MuonDetectorFactory001 theFactory(detStore().operator->());
    
        theFactory.setDBAtlasVersion(AtlasVersion);
        theFactory.setDBMuonVersion(MuonVersion);
        theFactory.setDBkey( detectorKey );
        theFactory.setDBnode(detectorNode);
    
        theFactory.setLayout(m_layout);
        //theFactory.setIncludeInertMats(m_includeInertMaterials);
        //theFactory.setIdhFromCnv(m_idhfromconverters);
        //theFactory.setMinimalGeoFlag(m_minimalgeo);
        theFactory.setCutoutsFlag(m_includeCutouts);
        theFactory.setCutoutsBogFlag(m_includeCutoutsBog);
        theFactory.setCtbBisFlag(m_includeCtbBis);
        theFactory.setControlAlines(m_controlAlines);
	theFactory.setMinimalGeoFlag(m_minimalGeoFlag);
	if (!m_useCscIntAlines) m_controlCscIntAlines = 0;
	theFactory.setControlCscIntAlines(m_controlCscIntAlines);
        theFactory.setDumpMemoryBreakDown(m_dumpMemoryBreakDown);
        theFactory.setCachingFlag(m_cachingFlag);
        theFactory.setCacheFillingFlag(m_fillCache_initTime);
        theFactory.setMdtDeformationFlag(m_enableMdtDeformations);
        theFactory.setMdtAsBuiltParaFlag(m_enableMdtAsBuiltParameters);
        theFactory.setFineClashFixingFlag(m_enableFineClashFixing);
        theFactory.hasCSC(m_hasCSC);
        theFactory.hasSTgc(m_hasSTgc);
        theFactory.hasMM(m_hasMM);
        if ( m_stationSelection > 0 ) theFactory.setSelection(m_selectedStations, m_selectedStEta, m_selectedStPhi);
		
        theFactory.setRDBAccess(access);
        //theFactory.setUseRDB(1);
        theFactory.setAltAsciiDBMap(altAsciiDBMap);
	theFactory.setDumpAlines(m_dumpAlines);
	theFactory.setDumpCscIntAlines(m_dumpCscIntAlines);
	theFactory.setUseCscIntAlinesFromGM(m_useCscIntAlinesFromGM);
    
        try {   
            //
            // This strange way of casting is to avoid an
            // utterly brain damaged compiler warning.
            //
            GeoPhysVol *world=&*theExpt->getPhysVol();
            theFactory.create(world);
        } catch (const std::bad_alloc&) {
            ATH_MSG_FATAL("Could not create new MuonDetectorNode!" );
            return StatusCode::FAILURE; 
        }

	if (m_dumpMemoryBreakDown)
	{
	    umem = GeoPerfUtils::getMem();
	    ucpu = float(GeoPerfUtils::getCpu()/100.);
	    geoModelStats <<"At MuonDetectorTool::factory created     \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	    mem = umem;
	    cpu = ucpu;
	}
	
        // Register the MuonDetectorNode instance with the Transient Detector Store
        m_manager = theFactory.getDetectorManager();
	//Init ABline historical container --- will write there A/B lines from ORACLE / ascii file if any
	m_manager->initABlineContainers();
	if ((m_manager->initCSCInternalAlignmentMap()).isFailure()) return StatusCode::FAILURE; // does nothing other then checking the size (map is built while reading data from the primary source)
        //Recod in StoreGate the ABline historical containers
	ALineMapContainer * amap = m_manager->ALineContainer();
	BLineMapContainer * bmap = m_manager->BLineContainer();
	ATH_MSG_DEBUG("A/BLineMapContainers are @ <"<<(uintptr_t)amap<<"> and <"<<(uintptr_t)bmap<<">" );
	
	// is this really needed ???????????
        if ((detStore()->record(amap,"MDT_A_LINE_CORR")).isFailure()) return StatusCode::FAILURE;
        if ((detStore()->record(bmap,"MDT_B_LINE_CORR")).isFailure()) return StatusCode::FAILURE;
        // if we want to fill the whole cache init time
        if (m_fillCache_initTime) {
	  m_manager->fillCache();
	}
	else
	  {
	    // cache for RPC / TGC / CSC must be filled once forever 
	    m_manager->fillRpcCache();
	    m_manager->fillTgcCache();
	    m_manager->fillCscCache();
	  }
	

	if (m_dumpMemoryBreakDown)
	{
	    umem = GeoPerfUtils::getMem();
	    ucpu = float(GeoPerfUtils::getCpu()/100.);
	    geoModelStats <<"At MuonDetectorTool::trk cache done      \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	    mem = umem;
	    cpu = ucpu;
	}
	
        ATH_CHECK( detStore()->record(theFactory.getDetectorManager(),theFactory.getDetectorManager()->getName()) );

        //if  (m_idhfromconverters == 0)
        //{
        // Register the MuonIdHelper instances with the Transient Detector Store
        // this way any client will ask the DetStore for them will not activate their converters 
        // StatusCode scridh = detStore->record(theFactory.getDetectorManager()->mdtIdHelper(), "MDTIDHELPER");
        // if (scridh != StatusCode::SUCCESS) ATH_MSG_ERROR("Could not record MDTIDHELPER" );
        // scridh = detStore->record(theFactory.getDetectorManager()->rpcIdHelper(), "RPCIDHELPER");
        // if (scridh != StatusCode::SUCCESS) ATH_MSG_ERROR("Could not record RPCIDHELPER" );
        // scridh = detStore->record(theFactory.getDetectorManager()->tgcIdHelper(), "TGCIDHELPER");
        // if (scridh != StatusCode::SUCCESS) ATH_MSG_ERROR("Could not record TGCIDHELPER" );
        // scridh = detStore->record(theFactory.getDetectorManager()->cscIdHelper(), "CSCIDHELPER");
        // if (scridh != StatusCode::SUCCESS) ATH_MSG_ERROR("Could not record CSCIDHELPER" );
        //}
        theExpt->addManager(theFactory.getDetectorManager());

        //     ATH_MSG_INFO("ToolHandle in create - "<<m_condDataTool );
        //     IMuonAlignmentDbTool* pAliTool = &*m_condDataTool;
        //     //and why not also print out this pAliTool pointer ? The &* is the trick to get the pointer to the real tool.
        //     ATH_MSG_INFO("pointer to the concrete tool in create - "<<pAliTool );
        //     std::string afn = m_condDataTool->aLineFolderName();
        //     ATH_MSG_INFO("A-line folder name is "<<afn );

    }

    if (m_dumpMemoryBreakDown)
    {
	umem = GeoPerfUtils::getMem();
	ucpu = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::everything done     \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	geoModelStats.close();
    }
    Muon::MuonSectorMapping mapping;
    /// add hash look-up for TGC sectors to the detector store here
    if( m_manager && m_manager->tgcIdHelper() ){
      unsigned int tgcHashMax = m_manager->tgcIdHelper()->detectorElement_hash_max();
      std::vector<int>* sectorHashVec = new std::vector<int>( tgcHashMax, 0 );
      for( unsigned int i=0;i<tgcHashMax;++i ){
        const MuonGM::TgcReadoutElement* detEl = m_manager->getTgcReadoutElement(i);
        if( detEl ) (*sectorHashVec)[i] = mapping.getSector(detEl->center().phi());
      }
      if( detStore()->record(sectorHashVec,"TGC_SectorMapping").isFailure()) {
        ATH_MSG_WARNING(" failed to record TGC sector mapping ");
      }
    }
    return StatusCode::SUCCESS;
}

StatusCode
MuonDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<MuonGM::MuonDetectorManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}
