/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <fstream>
#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

using namespace MuonGM;

/**
 ** Constructor(s)
 **/
MuonDetectorTool::MuonDetectorTool( const std::string& type, const std::string& name, const IInterface* parent )
    : GeoModelTool( type, name, parent ),  
      m_layout("P03"),
      m_accessCondDb(1),
      m_asciiCondData(0),
      m_nova(0),
      m_includeCutouts(0),
      m_includeCutoutsBog(0),
      m_includeCtbBis(0),
      m_fillCache_initTime(0),
      m_dumpMemoryBreakDown(false),
      m_enableFineClashFixing(0),
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
      m_cachingFlag(1),
      m_enableMdtDeformations(0),
      m_enableMdtAsBuiltParameters(0),
      m_altMdtAsBuiltFile(""),
      m_condDataTool(),
      m_manager(0)
{
    declareInterface<IGeoModelTool>(this);
    MsgStream log(msgSvc(), "MGM::MuonDetectorTool::MuonDetectorTool"); 
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
    declareProperty("TheMuonAlignmentTool",     m_condDataTool,      "a Tool to read from the condDB and save in StoreGate" );
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
    //MsgStream log(msgSvc(), name());
    //log<<MSG::INFO<<"Initialize"<<endmsg;
    msg(MSG::INFO)<<"Initializing ..."<<endmsg;

    if( !m_condDataTool.empty() ) ATH_CHECK( m_condDataTool.retrieve() );

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
        //MsgStream log(msgSvc(), "MGM::MuonDetectorTool::MuonDetectorTool");
        // Do clear cache built up during the event ...
        //log<<MSG::DEBUG<<"Clearing cache at end of event "<<endmsg;
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
    
    

    //MsgStream log(msgSvc(), name());

    if (msgLvl(MSG::VERBOSE))        
        msg(MSG::VERBOSE)<<" Show properties (user sett.s or default): LayoutName                 "<<m_layout
                         <<endmsg
                         <<" Show properties (user sett.s or default): IncludeCutouts             "<<m_includeCutouts
                         <<endmsg
                         <<" Show properties (user sett.s or default): IncludeCutoutsBog          "<<m_includeCutoutsBog
                         <<endmsg
                         <<" Show properties (user sett.s or default): IncludeCtbBis              "<<m_includeCtbBis
                         <<endmsg;
  
    // Get the detector configuration.
    IGeoModelSvc *geoModel;
    service ("GeoModelSvc",geoModel);
  
    std::string AtlasVersion = geoModel->atlasVersion();
    std::string MuonVersion  = geoModel->muonVersionOverride();

    msg(MSG::INFO)<<"create MuonDetectorTool - package version = "<<PACKAGE_VERSION<<endmsg;
    msg(MSG::INFO)<<"(from GeoModelSvc)    AtlasVersion = <"<< AtlasVersion<<">  MuonVersion = <"<<MuonVersion<<">"<<endmsg;

    // Unless we are using custom muons, the switch positions are going to
    // come from the database:

    std::string detectorKey  = MuonVersion.empty() ? AtlasVersion : MuonVersion;
    std::string detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";
    msg(MSG::INFO)<<"Keys for Muon Switches are  (key) "  << detectorKey  << " (node) " << detectorNode << endmsg;

    std::map<std::string,std::string> altAsciiDBMap = std::map<std::string,std::string>();
    if ( MuonVersion == "CUSTOM" ) 
        msg( MSG::WARNING )<< "Detector Information coming from a custom configuration !!" << endmsg; 
    else
    {
        msg( MSG::DEBUG )<< "Detector Information coming from the database (job options IGNORED)" << endmsg;
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
            msg(MSG::INFO)<<"(from GeoModelSvc) in AtlasVersion = <"<< AtlasVersion<<">  default MuonVersion is <"
                          <<MuonVersion<<">"<<endmsg;
        }

        msg(MSG::DEBUG)<<" m_altAsztFile: "<<m_altAsztFile<<endmsg;
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
    DataHandle<GeoModelExperiment> theExpt; 
    if (StatusCode::SUCCESS != detStore()->retrieve( theExpt, "ATLAS" ))
    { 
        msg(MSG::ERROR) 
            << "Could not find GeoModelExperiment ATLAS" 
            << endmsg; 
        return (StatusCode::FAILURE); 
    }

    if (!m_useCscIntAlines) m_controlCscIntAlines = 0;
    msg(MSG::INFO)
      <<"Properties have been set as follows: "<<endmsg
      <<"    LayoutName                     "<< m_layout.substr(0,1)<<endmsg
      <<"    IncludeCutouts                 "<< m_includeCutouts<<endmsg
      <<"    IncludeCutoutsBog              "<< m_includeCutoutsBog<<endmsg
      <<"    IncludeCtbBis                  "<< m_includeCtbBis<<endmsg
      <<"    ControlAlines                  "<< m_controlAlines<<endmsg
      <<"    MinimalGeoFlag                 "<< m_minimalGeoFlag<<endmsg 
      <<"    EnableCscIntAlignment          "<<m_useCscIntAlines<<endmsg
      <<"    EnableCscIntAlignmentFromGM    "<<m_useCscIntAlinesFromGM<<endmsg;
    if (m_useCscIntAlines) msg(MSG::INFO)
		  <<"    ControlCscIntAlines            "<< m_controlCscIntAlines <<endmsg;
    else msg(MSG::INFO)<<"    ControlCscIntAlines   reset to "<< m_controlCscIntAlines <<endmsg;
    msg(MSG::INFO)
      <<"    EnableMdtDeformations          "<<m_enableMdtDeformations<<endmsg;
    msg(MSG::INFO)
      <<"    EnableMdtAsBuiltParameters     "<<m_enableMdtAsBuiltParameters<<endmsg;

    if ( m_stationSelection > 0) {
		StationSelector::SetSelectionType( m_stationSelection);
        if ( (m_selectedStations.size()+ m_selectedStEta.size()+ m_selectedStPhi.size()) < 1){
            msg(MSG::ERROR) << " **** Badly set Option "<< endmsg
                            << " **** SelectedStations size =" << m_selectedStations.size()<<endmsg
                            << " **** SelectedStJzz    size =" << m_selectedStations.size()<<endmsg
                            << " **** SelectedStJff    size =" << m_selectedStations.size()<<endmsg
                            << " **** while StationSelection = 1"<< endmsg;
            return( StatusCode::FAILURE );
        }
        for (unsigned int i=0; i<m_selectedStations.size() ; i++){
            msg(MSG::INFO) << "          Selected stations      " << m_selectedStations[i] << endmsg;
        }
  
        if ( m_selectedStEta.size() > 0 )
        {
            for (unsigned int i=0; i<m_selectedStEta.size() ; i++)
                msg(MSG::INFO) << "          Selected Jzz locations  " << m_selectedStEta[i] << endmsg;
        }
        if ( m_selectedStPhi.size() > 0 )
        {
            for (unsigned int i=0; i<m_selectedStPhi.size() ; i++)
                msg(MSG::INFO) << "          Selected Jff locations  " << m_selectedStPhi[i] << endmsg;
        }
    }
  
      
    //
    // Locate the material manager:
    //
    DataHandle<StoredMaterialManager> theMaterialManager;
    if (StatusCode::SUCCESS != detStore()->retrieve(theMaterialManager, "MATERIALS")) {
        msg(MSG::ERROR) 
            << "Could not find Material Manager MATERIALS" 
            << endmsg; 
        return (StatusCode::FAILURE); 
    } 


    if (m_dumpMemoryBreakDown)
    {
	umem = GeoPerfUtils::getMem();
	ucpu = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::material mgr done   \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
    }
    
  
    StatusCode result = StatusCode::SUCCESS;
  
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
        if ( m_stationSelection > 0 ) theFactory.setSelection(m_selectedStations, m_selectedStEta, m_selectedStPhi);
		
    
        //if(!m_nova)
        //{
        theFactory.setRDBAccess(access);
        //theFactory.setUseRDB(1);
        theFactory.setAltAsciiDBMap(altAsciiDBMap);
	theFactory.setDumpAlines(m_dumpAlines);
	theFactory.setDumpCscIntAlines(m_dumpCscIntAlines);
	theFactory.setUseCscIntAlinesFromGM(m_useCscIntAlinesFromGM);
        //}
    
        try {   
            //
            // This strange way of casting is to avoid an
            // utterly brain damaged compiler warning.
            //
            GeoPhysVol *world=&*theExpt->getPhysVol();
            theFactory.create(world);
            //      log << MSG::INFO << "CREATING MuonDetectorNode; MM=" << &*theMaterialManager<< endmsg;
        } catch (const std::bad_alloc&) {
            msg(MSG::FATAL)<< "Could not create new MuonDetectorNode!" << endmsg;
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
	msg(MSG::DEBUG)<<"A/BLineMapContainers are @ <"<<(uintptr_t)amap<<"> and <"<<(uintptr_t)bmap<<">"<<endmsg;
	
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
	

        result = detStore()->record(theFactory.getDetectorManager(),theFactory.getDetectorManager()->getName());
        if (result != StatusCode::SUCCESS) return result;

        //if  (m_idhfromconverters == 0)
        //{
        // Register the MuonIdHelper instances with the Transient Detector Store
        // this way any client will ask the DetStore for them will not activate their converters 
        // StatusCode scridh = detStore->record(theFactory.getDetectorManager()->mdtIdHelper(), "MDTIDHELPER");
        // if (scridh != StatusCode::SUCCESS) msg(MSG::ERROR) 
        //                                        << "Could not record MDTIDHELPER" << endmsg;
        // scridh = detStore->record(theFactory.getDetectorManager()->rpcIdHelper(), "RPCIDHELPER");
        // if (scridh != StatusCode::SUCCESS) msg(MSG::ERROR) 
        //                                        << "Could not record RPCIDHELPER" << endmsg; 
        // scridh = detStore->record(theFactory.getDetectorManager()->tgcIdHelper(), "TGCIDHELPER");
        // if (scridh != StatusCode::SUCCESS) msg(MSG::ERROR) 
        //                                        << "Could not record TGCIDHELPER" << endmsg; 
        // scridh = detStore->record(theFactory.getDetectorManager()->cscIdHelper(), "CSCIDHELPER");
        // if (scridh != StatusCode::SUCCESS) msg(MSG::ERROR) 
        //                                        << "Could not record CSCIDHELPER" << endmsg; 
        //}
        theExpt->addManager(theFactory.getDetectorManager());

        //     log<<MSG::INFO<<"ToolHandle in create - "<<m_condDataTool<<endmsg;
        //     IMuonAlignmentDbTool* pAliTool = &*m_condDataTool;
        //     //and why not also print out this pAliTool pointer ? The &* is the trick to get the pointer to the real tool.
        //     log<<MSG::INFO<<"pointer to the concrete tool in create - "<<pAliTool<<endmsg;
        //     std::string afn = m_condDataTool->aLineFolderName();
        //     log << MSG::INFO << "A-line folder name is "<<afn << endmsg;

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
    return result;
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

StatusCode   
MuonDetectorTool::registerCallback()
{

    //MsgStream log(msgSvc(), name());

    if (m_accessCondDb == 0) 
    {
        msg(MSG::INFO)<<"No data will be read from the condition DB"<<endmsg;
        if (m_asciiCondData == 0) return StatusCode::FAILURE;//!< This is OK: We don't want to look at condition data !
    }

    // m_accessCondDb == 1, Amdcsimrec will expect A/B line containers from MuonDetectorManager -> fill the starting point 
    // init A/B line historical container
    // m_manager->initABlineContainers();
    // record in SG the A/B line full containers 
    //if ((detStore->record(m_manager->ALineContainer(),"MDT_A_LINE_CORR")).isFailure()) return StatusCode::FAILURE;
    //if ((detStore->record(m_manager->BLineContainer(),"MDT_B_LINE_CORR")).isFailure()) return StatusCode::FAILURE;

    //

    bool aFolderFound = false;
    std::vector<std::string> foundFolderNames;

    if( !m_condDataTool.empty() ){
      std::vector<std::string> folderNames = m_condDataTool->abLineFolderNames();
      msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endmsg;
      int ic=0;
      for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
      {
          ++ic;
          msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
          if (detStore()->contains<CondAttrListCollection>(*ifld)) {
              aFolderFound=true;
              foundFolderNames.push_back(*ifld);
              msg(MSG::INFO)<<"     found in the DetStore"<<endmsg;
          }
          else
              msg(MSG::INFO)<<" NOT found in the DetStore"<<endmsg;
      }
    }

    if (!aFolderFound) 
    {
        msg(MSG::INFO)<<"CondAttrListCollection not found in the DetectorStore"<<endmsg
                      <<"Unable to register callback on CondAttrListCollection for any folder in the list "<<endmsg
                      <<"This is OK unless you expect to read alignment and deformations from COOL "<<endmsg;
        if (m_asciiCondData!=0) 	
	{
            int dummyint;
            std::list<std::string> dummylist;
            this->align(dummyint,dummylist);
            if (m_accessCondDb==0) return StatusCode::FAILURE;
	}
	else return StatusCode::FAILURE;//!< This is OK for standard reconstruction until we are running with static MS geometry DB !
    }

    //!< register the IGeoModelTool::align interface of this tool for call back after the data
    for (std::vector<std::string>::const_iterator ifld=foundFolderNames.begin(); ifld!=foundFolderNames.end(); ++ifld)
    {
        const DataHandle<CondAttrListCollection> parlineData;
        StatusCode sc = detStore()->regFcn(&IGeoModelTool::align,
                                         dynamic_cast<IGeoModelTool *>(this),
                                         parlineData,
                                         *ifld);
        if (sc.isFailure())
        {
            msg(MSG::WARNING)<<"Unable to register call-back to MuonDetectorTool::align() against folder <"<<*ifld<<">"
               <<" This is OK unless you expect to read alignment and deformations from COOL "<<endmsg;
        }
        else msg(MSG::INFO)<<"Call-back to MuonDetectorTool::align() against folder "<<*ifld<<" registered "<<endmsg;
    }
    
    return StatusCode::SUCCESS;
}

StatusCode MuonDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
    //MsgStream log(msgSvc(), name());
    msg(MSG::INFO)<<"In MuonDetectorTool::align"<<endmsg;
    if (m_accessCondDb == 0) 
    {
        msg(MSG::INFO)<<"Access to the cond DB not enabled by MuonGeoModel specific job-options (UseConditionDb=0)"<<endmsg;
        if (m_asciiCondData==0) return StatusCode::SUCCESS;
    }	
    if (m_asciiCondData==0) msg(MSG::INFO)<<"Access to the cond DB enabled by MuonGeoModel specific job-options (UseConditionDb=1)"<<endmsg;
    
    msg(MSG::DEBUG)<<"ToolHandle in align - "<<m_condDataTool<<endmsg;
    IMuonAlignmentDbTool* pAliTool = &*m_condDataTool;
    //and why not also print out this pAliTool pointer ? The &* is the trick to get the pointer to the real tool.
    msg(MSG::DEBUG)<<"pointer to the concrete tool in align - "<<(uintptr_t)pAliTool<<endmsg;

    std::ofstream geoModelStats;
    int mem = 0;
    float cpu = 0;
    int umem = 0;
    float ucpu = 0;
    
    if (m_dumpMemoryBreakDown){
      geoModelStats.open("MuonGeoModelStatistics_MuonDetectorTool",std::ios::app);
	umem = GeoPerfUtils::getMem();
	ucpu = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::align method just called back   \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
    }

    
    StatusCode sc = m_condDataTool->loadParameters(I, keys);
    if (sc.isRecoverable())
    {
        msg(MSG::WARNING)
	   <<"Recoverable error while loading the Alignment and Deformation parameters"<<endmsg
           <<"part of alignment data requested might be missing"<<endmsg;
    }
    else if (sc.isFailure())
    {
        msg(MSG::ERROR)<<"Unable to load the Alignment and Deformation parameters"<<endmsg;
        return sc;
    }
    msg(MSG::INFO)<<"Alignment and Deformation parameters loaded and stored in the DetectorStore"<<endmsg;

    if (m_dumpMemoryBreakDown)
    {
	umem = GeoPerfUtils::getMem();
	ucpu = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::align method finished reading from COOL   \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
    }

    sc = m_manager->updateAlignment(m_condDataTool->ALineContainer());
    if (sc.isFailure()) msg(MSG::ERROR)<<"Unable to updateAlignment"<<endmsg;
    else msg(MSG::DEBUG)<<"updateAlignment DONE"<<endmsg;

    if (m_dumpMemoryBreakDown)
    {
	umem = GeoPerfUtils::getMem();
	ucpu = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::align method after manager->updateAlignment   \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
    }


    sc = m_manager->updateDeformations(m_condDataTool->BLineContainer());
    if (sc.isFailure()) msg(MSG::ERROR)<<"Unable to updateDeformations"<<endmsg;
    else msg(MSG::DEBUG)<<"updateDeformations DONE"<<endmsg;

    if (m_dumpMemoryBreakDown)
    {
	umem = GeoPerfUtils::getMem();
	ucpu = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::align method after manager->updateDeformations   \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
    }

    sc = m_manager->updateAsBuiltParams(m_condDataTool->AsBuiltContainer());
    if (sc.isFailure()) msg(MSG::ERROR)<<"Unable to updateAsBuiltParams"<<endmsg;
    else msg(MSG::DEBUG)<<"updateAsBuiltParams DONE"<<endmsg;

    if (m_dumpMemoryBreakDown)
    {
	umem = GeoPerfUtils::getMem();
	ucpu = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::align method after manager->updateAsBuiltParams   \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
    }
    
    if (!m_useCscIntAlinesFromGM) sc = m_manager->updateCSCInternalAlignmentMap(m_condDataTool->ILineContainer());
    if (sc.isFailure() && !m_useCscIntAlinesFromGM) msg(MSG::ERROR)<<"Unable to updateCSCInternalAlignmentMap"<<endmsg;
    else msg(MSG::DEBUG)<<"updateCSCInternalAlignmentMap DONE or used via GM"<<endmsg;

    if (m_dumpMemoryBreakDown && !m_useCscIntAlinesFromGM)
    {
	umem = GeoPerfUtils::getMem();
	ucpu = float(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorTool::align method after manager->updateCSCInternalAlignmentMap   \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
    }    

    if (m_dumpMemoryBreakDown)
    {
	geoModelStats.close();
    }    

    return sc;
}
