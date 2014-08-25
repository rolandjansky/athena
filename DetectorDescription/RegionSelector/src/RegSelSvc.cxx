/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// #include "RegionSelector/RegSelSvc.h"
#include "RegSelSvc.h"
#include "RegionSelector/RegSelEnums.h"
#include "RegionSelector/RegSelectorMap.h"
#include "RegionSelector/RegSelectorHashMap.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "RegionSelector/StoreGateRS_ClassDEF.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"

// #include "RegSelLUT/RegSelTimer.h"
#include "RegSelLUT/RegSelRoI.h"
#include "RegSelLUT/StoreGateIDRS_ClassDEF.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
// #include "PixelConditionsServices/PixelConditionsSummarySvc.h"
// #include "SCT_ConditionsServices/SCT_ConditionsSummarySvc.h"
// #include "TRT_ConditionsServices/TRT_ConditionsSummarySvc.h"


//#include <pthread.h>
//static pthread_mutex_t regselsvcmutex = PTHREAD_MUTEX_INITIALIZER;

//-----------------------------------------------------------------------------
//   Implementation file for class : RegionSelector
//
//   22Feb06 : Aline Gesualdi Mello, Gabriel Mayrink, Marcio Albuquerque, 
//             Steve Armstrong, Simon George
//  ---------------------------------------------------------------------------

//! Constructor
RegSelSvc::RegSelSvc(const std::string& name, ISvcLocator* sl)
  : Service(name,sl),
    m_detStore(0),
    m_regionType("n/a"),
    m_geometryVersionProp("use_geomodel"),
    m_initOnlyID(false),
    m_initSCT(true),
    m_initPixel(true),
    m_initTRT(true),
    m_initOnlyCalo(false),
    m_initOnlyMuon(false),
    m_dumpTable(false), 
    m_disableFromConditions(false),
    m_disablePixelFromConditions(true),
    m_disableSCTFromConditions(true),
    m_disableTRTFromConditions(true),
    m_roiFileName        ("RSOutputTable.txt"),
    m_LArFebRodMapKey    ("/LAR/Identifier/FebRodMap"),
    m_msgOutputLevel(MSG::NIL),
    m_lutCreatorToolPixel("SiRegionSelectorTable/PixelRegionSelectorTable"),
    m_lutCreatorToolSCT  ("SiRegionSelectorTable/SCT_RegionSelectorTable"),
    m_lutCreatorToolTRT  ("TRT_RegionSelectorTable/TRT_RegionSelectorTable"),
    m_lutCreatorToolLAR  ("LArRegionSelectorTable"),
    m_lutCreatorToolTile ("TileRegionSelectorTable"),
    m_geoModelSvc("GeoModelSvc",name),
    m_log(0),
    m_DeltaZ(168),
    m_initRPC(true),
    m_initMDT(true),
    m_initTGC(true),
    m_initCSC(true),
    m_lutCreatorToolRPC  ("RPC_RegionSelectorTable"),
    m_lutCreatorToolMDT  ("MDT_RegionSelectorTable"),
    m_lutCreatorToolTGC  ("TGC_RegionSelectorTable"),
    m_lutCreatorToolCSC  ("CSC_RegionSelectorTable"),
    m_initFTK(false),
    m_lutCreatorToolFTK  ("FTK_RegionSelectorTable"),
    m_duplicateRemoval( true )
{
  //! Declare properties
  declareProperty( "RegionType", m_regionType, "this is not used" );
  declareProperty( "GeometryLayout", m_geometryVersionProp, "set detector description for muon layout; default is to use version provided by GeoModel" );
  declareProperty( "enableID",    m_initOnlyID,   "enable inner detector maps" );
  declareProperty( "enableSCT",   m_initSCT,      "enable SCT maps" );
  declareProperty( "enablePixel", m_initPixel,    "enable Pixel maps" );
  declareProperty( "enableTRT",   m_initTRT,      "enable TRT map" );
  declareProperty( "enableCalo",  m_initOnlyCalo, "enable calorimeter maps" );
  declareProperty( "enableMuon",  m_initOnlyMuon, "enable muon maps" );
  declareProperty( "enableRPC",   m_initRPC,      "enable RPC map" );
  declareProperty( "enableMDT",   m_initMDT,      "enable MDT map" );
  declareProperty( "enableTGC",   m_initTGC,      "enable TGC map" );
  declareProperty( "enableCSC",   m_initCSC,      "enable CSC map" );
  declareProperty( "enableFTK",   m_initFTK,      "enable FTK map" );
  declareProperty( "WriteTables", m_dumpTable,    "write out maps to files for debugging" );
  declareProperty( "OutputFile",  m_roiFileName,  "base filename used to write maps to" );
  declareProperty( "PixelRegionLUT_CreatorTool", m_lutCreatorToolPixel);
  declareProperty( "SCT_RegionLUT_CreatorTool",  m_lutCreatorToolSCT);
  declareProperty( "TRT_RegionLUT_CreatorTool",  m_lutCreatorToolTRT);
  declareProperty( "LArRegionSelectorTable",     m_lutCreatorToolLAR);
  declareProperty( "readSiROBListFromOKS",       m_readSiROBListFromOKS, "read silicon rob list to from oks");
  declareProperty( "DeletePixelHashList",        m_deletePixelHashList,  "delete pixel modules with these ids");
  declareProperty( "DeleteSCTHashList",          m_deleteSCTHashList,    "delete sct modules with these ids");
  declareProperty( "DeleteTRTHashList",          m_deleteTRTHashList,    "delete trt modules with these ids");
  declareProperty( "DeleteSiRobList",            m_deleteRobList,        "delete indet modules corresponding to these robs ids");
  declareProperty( "LArFebRodMapKey",            m_LArFebRodMapKey,      "Condition to trigger RegSelSvc initialization");
  declareProperty( "DeltaZ",                     m_DeltaZ,               "z vertex range");
  declareProperty( "DisableFromConditions",      m_disableFromConditions=false,  "disable id modules based on the conditions summary svc");
  declareProperty( "DisablePixelFromConditions", m_disablePixelFromConditions=true, "disable Pixel modules based on the conditions summary svc");
  declareProperty( "DisableSCTFromConditions",   m_disableSCTFromConditions=true,   "disable SCT modules based on the conditions summary svc");
  declareProperty( "DisableTRTFromConditions",   m_disableTRTFromConditions=true,   "disable TRT modules based on the conditions summary svc");
  declareProperty( "PixConditionsSvc",           m_PixConditionsSvc="PixelConditionsSummarySvc",  "name of conditions summary svc for the pixels");
  declareProperty( "SCTConditionsSvc",           m_SCTConditionsSvc="SCT_ConditionsSummarySvc",  "name of conditions summary svc for the SCT");
  declareProperty( "TRTConditionsSvc",           m_TRTConditionsSvc="TRT_ConditionsSummarySvc",  "name of conditions summary svc for the TRT");
}


//! Standard destructor
RegSelSvc::~RegSelSvc() { }

//! queryInterface
StatusCode 
RegSelSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( IRegSelSvc::interfaceID().versionMatch(riid) )    {
    //if (IID_gSelSvc == riid){
    *ppvInterface = (IRegSelSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode RegSelSvc::initialize() {

  m_log = new MsgStream( msgSvc(), name() );
  m_errorFlag=false;
  StatusCode sc;
  sc = Service::initialize();
  if (sc.isFailure()){
    (*m_log) << MSG::ERROR << "Service base class initialization failed" << endreq;
    return sc;
  }
  
  // set detector description directly from GeoModel by default
  if (m_geometryVersionProp.value() == "use_geomodel"){
    m_geometryVersion = m_geoModelSvc->atlasVersion();
  } else {
    m_geometryVersion = m_geometryVersionProp.value();
  }

  (*m_log) << MSG::INFO << "Initializing " << name()
      << " - package version " << PACKAGE_VERSION << endreq ;
  (*m_log) << MSG::INFO
      << "DetectorDescription version " << m_geometryVersion;
  if (m_geometryVersionProp.value() == "use_geomodel"){
    (*m_log) << " (obtained from GeoModelSvc)" << endreq;
  } else {
    (*m_log) << " (supplied as property)" << endreq;
  }
  (*m_log) << MSG::DEBUG << "property value was " 
      << m_geometryVersionProp.value() << ")" << endreq;

  (*m_log) << MSG::INFO << "DeltaZ = " << m_DeltaZ << endreq; 

  // cache message output level
  m_msgOutputLevel = (*m_log).currentLevel();

  
  std::string sctflag("enabled");
  std::string pixelflag("enabled");
  std::string trtflag("enabled");

  if ( m_initOnlyID.value() ) { 
    if ( !m_initSCT.value() )   sctflag   = "disabled"; 
    if ( !m_initPixel.value() ) pixelflag = "disabled"; 
    if ( !m_initTRT.value() )   trtflag   = "disabled"; 
  }
   
  std::string ftkflag("enabled");  
  if ( !m_initFTK.value() )  ftkflag = "disabled"; 
  

  std::string rpcflag("enabled");
  std::string mdtflag("enabled");
  std::string tgcflag("enabled");
  std::string cscflag("enabled");

  if( m_initOnlyMuon.value() ){
    if ( !m_initRPC.value() )  rpcflag = "disabled"; 
    if ( !m_initMDT.value() )  mdtflag = "disabled"; 
    if ( !m_initTGC.value() )  tgcflag = "disabled"; 
    if ( !m_initTGC.value() )  cscflag = "disabled"; 
  }
  
  
  (*m_log)                 << MSG::INFO << "detector switches:" 
			   << " indet=" << (m_initOnlyID.value() ? "enabled":"disabled");
  if( m_initOnlyID.value() )   (*m_log) << " ( sct=" << sctflag << " pixel=" << pixelflag << " trt=" << trtflag << " ftk=" << ftkflag << " )"; 

  (*m_log)                              << " calo="  << (m_initOnlyCalo.value() ? "enabled":"disabled") 
				        << " muon="  << (m_initOnlyMuon.value() ? "enabled":"disabled");

  if( m_initOnlyMuon.value() ) (*m_log) << " ( rpc=" << rpcflag << " mdt=" << mdtflag << " tgc=" << tgcflag << " csc=" << cscflag << " )";
  (*m_log)                              << endreq;
  
     
  //! Access StoreGateSvc 
  // only needed for indet currently, but get it regardless of detector choice
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()|| 0 == m_detStore ){
    (*m_log) << MSG::ERROR << "Could not find DetectorStore Service" << endreq;
    return StatusCode::FAILURE;
  }

  bool errorFlag=false; // true indicates an error occured somewhere
  
  m_newpixel = NULL;
  m_newsct   = NULL;
  m_newtrt   = NULL;



#if 1
  // FIXME W.L. July 3rd 2013:  This is not a good critera! 
  if( m_initOnlyCalo ){ // Only if Calo is needed
    // Event handling LAr Only
    const DataHandle<AthenaAttributeList> febrodmap;
    std::string key(m_LArFebRodMapKey);
    if ( m_detStore->regFcn(&IRegSelSvc::handle,
			    (IRegSelSvc*)this,febrodmap, key.c_str(),true ).isFailure() ) {
      (*m_log) << MSG::ERROR << " Can't regFcn for conditions"
	       << endreq;
      return StatusCode::FAILURE;
    }
  }
  
#endif

  // add the incident handler for the calo and id initialisation during BeginRun  
  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc",name());
  sc = incidentSvc.retrieve(); 	 
  if (sc.isFailure() ) { 	 
    (*m_log) << MSG::ERROR << " Failed to register Listener with IncidentSvc" << endreq; 	 
    //    return StatusCode::FAILURE;
    errorFlag = true; 	 
  }  
  else { 	 
    incidentSvc->addListener( this , "BeginRun"); 	 
    (*m_log) << MSG::INFO << " registered Listener with IncidentSvc" << endreq; 	 
  } 	 

  // if we need to initialise from oks, then get the list of robs 
  // to initialise

  // don't do anything if rob list isn't to be read from OKS
  if ( m_readSiROBListFromOKS.value()==true ) { 
    // try to get the new list of rob ids to enable from OKS  
    if ( !GetEnabledROBsFromOKS() ) {
      (*m_log) << MSG::ERROR << "Could not get rob list from OKS" << endreq;
      errorFlag = true;
    }
  }

  m_errorFlag=errorFlag; // Treatment later on

  return StatusCode::SUCCESS;
}



// callback needed for cal backwards compatability
StatusCode RegSelSvc::handle( IOVSVC_CALLBACK_ARGS ) {
  // nnaughty, naughty, and a bit clumsy, handleClo() returns
  // errorFlag which is *true* if an error has occured, 
  // so handleCalo() == true actually means something has
  // *failed*, clumsy, but that's how it's done internally
  if ( handleCalo() ) return StatusCode::FAILURE;
  else                return StatusCode::SUCCESS;
}



// StatusCode RegSelSvc::handle( IOVSVC_CALLBACK_ARGS ) {
void RegSelSvc::handle(const Incident& incident) { 

  //static bool initialised = false;
  
  // listen for BeginRun
  if (incident.type()=="BeginRun") { 
    
    //    MuonCreator m;

    // only initialise if this is the first BeginRun
    //   if ( initialised ) return;

    (*m_log) << MSG::INFO << " handle incident type " << incident.type() << endreq; 	 


    // call Innr detector, Calo and Muon handlers 
    if ( m_initOnlyID )    handleID();
    if ( m_initOnlyMuon )  handleMuon();
    //    if ( m_initOnlyCalo )  handleCalo();

    if(!m_errorFlag){
      (*m_log) << MSG::INFO << " >>>>> ";
      std::vector<std::string>::const_iterator d;
      for (d=m_enabledDetectors.begin(); d!=m_enabledDetectors.end(); ++d)  (*m_log) << " " << *d;
      (*m_log) << " detectors have been initialized" << endreq; // " with Region Type " << m_regionType << endreq;
    }
    
    if ( m_dumpTable ) { 
      RegSelSiLUT* tables[7] = { m_newpixel, m_newsct, m_newtrt, m_newrpc, m_newmdt, m_newtgc, m_newcsc };
      for ( int it=0 ; it<7 ; it++ ) { 
	if ( tables[it] ) { 
	  char table[64];
	  std::sprintf( table, "table-%d.map", it ); 
	  (*m_log) << MSG::INFO << "dumping table " << it << " to file: " << table << endreq;
	  tables[it]->write(table);
	}
      }
    }
    
    //  if ( m_errorFlag ) return StatusCode::FAILURE;
    //  else               return StatusCode::SUCCESS;
  }

}





void RegSelSvc::disableIDFromConditions(RegSelSiLUT* detector, const std::string& serviceName) { 

  (*m_log) << MSG::INFO << " reading detector conditions from " << serviceName << endreq;

  if (serviceName.empty()){
    (*m_log) << MSG::INFO << "Unspecified conditions summary service, skipping disableIDFromConditions" << endreq;
    return;
  }

  if ( detector ) { 

    // get ConditionsSummarySvc

    ServiceHandle<IInDetConditionsSvc> condsummary(serviceName, name());

    if ( condsummary.retrieve().isFailure() ) { 
      (*m_log) << MSG::ERROR << "failed to get " << serviceName << endreq;
      return;
    } 

    // get list of all detector elements for this detector

    std::vector<IdentifierHash> IDList;

    if ( detector==m_newpixel )  DetHashIDList(PIXEL, IDList);
    if ( detector==m_newsct )    DetHashIDList(SCT,   IDList);
    if ( detector==m_newtrt )    DetHashIDList(TRT,   IDList);
    
    // there must be a nicer way to do this.
    // basically, I have to enable all modules, 
    // then go through them getting the list of 
    // modules to enable and the list to disable, 
    // then I have to enable and disable them 
    // appropriately. hmmm 

    // get lists of modules to enable and disable
    std::vector<IdentifierHash> DisableList;
    std::vector<IdentifierHash> EnableList;

    int active_modules   = 0;
    int disabled_modules = 0;

    std::vector<IdentifierHash>::iterator mitr(IDList.begin());
    std::vector<IdentifierHash>::iterator mend(IDList.end());
    
    while ( mitr!=mend ) {  
     
      if ( condsummary->isActive(*mitr) )  { 
	EnableList.push_back(*mitr);
	active_modules++;
      }
      else {
	DisableList.push_back(*mitr);
	disabled_modules++; 
      }
      if(m_msgOutputLevel <= MSG::VERBOSE ) { 
	(*m_log) << MSG::VERBOSE << serviceName << " module 0x" << std::hex << *mitr << std::dec 
		 << " isActive()=" << condsummary->isActive(*mitr) << endreq;
      }
      mitr++;
    }

    (*m_log) << MSG::INFO << serviceName << " Number of modules active   " << active_modules << endreq;
    (*m_log) << MSG::INFO << serviceName << " Number of modules disabled " << disabled_modules << endreq;
    
    // these methods have to be tidied up, since they 
    // each rebuild the Disabled module maps, I need 
    // methods to enable/disable, and *after* to rebuild 
    // the disabled module map 
    
    detector->disableModuleList(DisableList);
    //    detector->enableModuleList(EnableList);
	
  }

}







bool RegSelSvc::handleID() { 

  bool errorFlag = false;

  (*m_log) << MSG::INFO << " Initialising Inner Detector maps" << endreq;

  // new region selector code, we make the (reasonably simple) 
  // structures in the SiRegionSelectorTable so we don't need to 
  // do any more than just extract the tables.
  

  if ( m_initFTK.value() ) { 

    (*m_log) << MSG::INFO << "setting up the FTK tables " << endreq;

    StatusCode sc = readFromSG(m_lutCreatorToolFTK, m_ftklut);

    if (sc.isFailure()){
      if(m_msgOutputLevel <= MSG::WARNING ) 
	(*m_log) << MSG::WARNING << "Failed to initialize ftk lut" << endreq;
      errorFlag = true;
    } 
    else { 
      if ( m_ftklut ) {
	if(m_msgOutputLevel <= MSG::INFO ) 
	  (*m_log) << MSG::INFO << "retrieved ftk RegSelEtaPhiLUT" << endreq;
      }
      else { 
	if(m_msgOutputLevel <= MSG::WARNING ) 
	  (*m_log) << MSG::ERROR << "retrieved ftk RegSelEtaPhiLUT is NULL" << endreq;
        errorFlag = true;
      }
    }
    
  }
  else { 
    (*m_log) << MSG::INFO << "not setting up the FTK tables " << endreq;
  }


  //! Read PIXEL data from Detector Store
  if ( m_initPixel.value() ) { 
    StatusCode sc = readFromSG(m_lutCreatorToolPixel, m_newpixel);
    //     sc = m_detStore->retrieve(m_newpixel, "PixelRegSelSiLUT");
    if (sc.isFailure()){
      if(m_msgOutputLevel <= MSG::WARNING ) 
	(*m_log) << MSG::WARNING << "Failed to initialize pixel data" << endreq;
      errorFlag = true;
    } 
    else { 
      if ( m_newpixel ) { 
	if(m_msgOutputLevel <= MSG::INFO ) 
	  (*m_log) << MSG::INFO << "retrieved new pixel RegSelSiLUT map " << m_newpixel->getName() << endreq;
      }
      else { 
	if(m_msgOutputLevel <= MSG::ERROR )  
	  (*m_log) << MSG::ERROR << "retrieved new pixel RegSelSiLUT map is NULL" << endreq;
	errorFlag = true;
      }
    }      
  }
  
  //! Read SCT data from Detector Store
  if ( m_initSCT.value() ) {  
    StatusCode sc = readFromSG(m_lutCreatorToolSCT, m_newsct);
    //     sc = m_detStore->retrieve(m_newsct, "SCTRegSelSiLUT");
    if (sc.isFailure()){
      (*m_log) <<  MSG::WARNING << "Failed to initialize SCT data" << endreq;
      errorFlag = true;
    }
    else { 
      if ( m_newsct ) 
	(*m_log) << MSG::INFO << "retrieved new sct RegSelSiLUT map " << m_newsct->getName() << endreq;
      else {
	(*m_log) << MSG::ERROR << "retrieved new sct RegSelSiLUT map is NULL" << endreq;   
	errorFlag = true;
      }
    }
  }
  
  
  //! Read TRT data from Detector Store
  
  if ( m_initTRT.value() ) { 
    StatusCode sc = readFromSG(m_lutCreatorToolTRT, m_newtrt);
    //     sc = m_detStore->retrieve(m_newtrt, "TRTRegSelSiLUT");
    if (sc.isFailure()){
      (*m_log) <<  MSG::WARNING << "Failed to initialize TRT data" << endreq;
      errorFlag = true;
    }
    else { 
      if ( m_newtrt ) 
	(*m_log) << MSG::INFO << "retrieved new trt RegSelSiLUT map " << m_newtrt->getName() << endreq;
      else {  
	(*m_log) << MSG::ERROR << "retrieved new trt RegSelSiLUT is NULL" << endreq;
	errorFlag = true;
      }
    }    
  }      

  // now enable/disable requested robs and modules from OKS 
  if ( !reinitialiseInternalFromOKS() )  { 
    (*m_log) << MSG::WARNING << " could not enable robs from OKS " << endreq; 	 
  }
  
  if ( !reinitialiseInternal() )  {
    (*m_log) << MSG::WARNING << " could not disable requested detector elements " << endreq; 	 
  }

  m_enabledDetectors.push_back("Inner");
  
  m_errorFlag |= errorFlag; 

  return errorFlag; 
}





bool RegSelSvc::handleMuon() { 

  bool errorFlag = false;

  (*m_log) << MSG::INFO << " Initialising Muon maps" << endreq;

  // new region selector code, we make the (reasonably simple) 
  // structures in the Tables so we don't need to 
  // do any more than just extract the tables.
  
  //! Read RPC data from Detector Store
  if ( m_initRPC.value() ) {  
    StatusCode sc = readFromSG(m_lutCreatorToolRPC, m_newrpc);
    if (sc.isFailure()){
      (*m_log) << MSG::WARNING << "Failed to initialize rpc data" << endreq;
      errorFlag = true;
    } 
    else { 
      if ( m_newrpc ) 
	(*m_log) << MSG::INFO << "retrieved new rpc RegSelSiLUT map " << m_newrpc->getName() << endreq;
      else { 
	(*m_log) << MSG::ERROR << "retrieved new rpc RegSelSiLUT map is NULL" << endreq;
	errorFlag = true;
      }
    }      
  }
  
  //! Read MDT data from Detector Store
  if ( m_initMDT.value() ) {  
  
    StatusCode sc = readFromSG(m_lutCreatorToolMDT, m_newmdt);
    if (sc.isFailure()){
      (*m_log) <<  MSG::WARNING << "Failed to initialize MDT data" << endreq;
      errorFlag = true;
    }
    else { 
      if ( m_newmdt ) 
	(*m_log) << MSG::INFO << "retrieved new mdt RegSelSiLUT map " << m_newmdt->getName() << endreq;
      else {
	(*m_log) << MSG::ERROR << "retrieved new mdt RegSelSiLUT map is NULL" << endreq;   
	errorFlag = true;
      }
    }
  }
  
  
  //! Read TGC data from Detector Store
  
  if ( m_initTGC.value() ) {  
   
    StatusCode sc = readFromSG(m_lutCreatorToolTGC, m_newtgc);
    if (sc.isFailure()){
      (*m_log) <<  MSG::WARNING << "Failed to initialize TGC data" << endreq;
      errorFlag = true;
    }
    else { 
      if ( m_newtgc ) 
	(*m_log) << MSG::INFO << "retrieved new tgc RegSelSiLUT map " << m_newtgc->getName() << endreq;
      else {  
	(*m_log) << MSG::ERROR << "retrieved new tgc RegSelSiLUT is NULL" << endreq;
	errorFlag = true;
      }
    }    
  }      


  //! Read CSC data from Detector Store
  
  if ( m_initCSC.value() ) {  
   
    StatusCode sc = readFromSG(m_lutCreatorToolCSC, m_newcsc);
    if (sc.isFailure()){
      (*m_log) <<  MSG::WARNING << "Failed to initialize CSC data" << endreq;
      errorFlag = true;
    }
    else { 
      if ( m_newcsc ) 
	(*m_log) << MSG::INFO << "retrieved new csc RegSelSiLUT map " << m_newcsc->getName() << endreq;
      else {  
	(*m_log) << MSG::ERROR << "retrieved new csc RegSelSiLUT is NULL" << endreq;
	errorFlag = true;
      }
    }    
  }      
  
  m_enabledDetectors.push_back("Muon");

  m_errorFlag |= errorFlag; 

  return errorFlag; 
}






bool RegSelSvc::handleCalo() {

  // StatusCode RegSelSvc::handleCalo( IOVSVC_CALLBACK_ARGS ) {
  
  StatusCode sc;
  //  bool errorFlag=m_errorFlag; // true indicates an error occured somewhere
  bool errorFlag=false;  // true indicates an error occured somewhere
  const RegionSelectorLUT* detRSlut;
  
  bool readCaloFromSG = true;
  bool readTCaloFromSG = true;
  // first use of tool puts all maps into SG, so subsequent map retrieval can be done directly.
  sc = readFromSG(m_lutCreatorToolLAR, detRSlut, "EM");
  if (sc.isFailure()){
    (*m_log) << MSG::WARNING << "Failed to initialize LAr data" << endreq;
    errorFlag = true;
  } else {
    if (addLUT(LAR,detRSlut).isFailure()){
      (*m_log) << MSG::WARNING << "Failed to mount LAr data" << endreq;
      errorFlag = true;
    }
  }
  
  sc = readFromSG(TTEM, detRSlut);
  if (sc.isFailure()){
    (*m_log) <<  MSG::WARNING << "Failed to initialize TTEM data" << endreq;
    readCaloFromSG = false;
  } else {
    if (addLUT(TTEM,detRSlut).isFailure()){
      (*m_log) << MSG::WARNING << "Failed to mount TTEM data" << endreq;
      errorFlag = true;
    }
  }
  sc = readFromSG(TTHEC, detRSlut);
  if (sc.isFailure()){
    (*m_log) <<  MSG::WARNING << "Failed to initialize TTHEC data" << endreq;
    readCaloFromSG = false;
  } else {
    if (addLUT(TTHEC,detRSlut).isFailure()){
      (*m_log) << MSG::WARNING << "Failed to mount TTHEC data" << endreq;
      errorFlag = true;
    }
  }
  sc = readFromSG(FCALEM, detRSlut);
  if (sc.isFailure()){
    (*m_log) <<  MSG::WARNING << "Failed to initialize FCALEM data" << endreq;
    readCaloFromSG = false;
  } else {
    if (addLUT(FCALEM,detRSlut).isFailure()){
      (*m_log) << MSG::WARNING << "Failed to mount FCALEM data" << endreq;
      errorFlag = true;
    }
  }
  sc = readFromSG(FCALHAD, detRSlut);
  if (sc.isFailure()){
    (*m_log) <<  MSG::WARNING << "Failed to initialize FCALHAD data" << endreq;
    readCaloFromSG = false;
  } else {
    if (addLUT(FCALHAD,detRSlut).isFailure()){
      (*m_log) << MSG::WARNING << "Failed to mount FCALHAD data" << endreq;
      errorFlag = true;
    }
  }
  
  if ( !readCaloFromSG ) { // LAr not read from SG => from files
    (*m_log) <<  MSG::WARNING << "Will try the files for LAr" << endreq;
    sc = initCalo();
    if (sc.isFailure()){
      (*m_log) <<  MSG::WARNING << "Failed to initialize Calo data" << endreq;
      errorFlag = true;
    }
  } 
  
  sc = readFromSG(m_lutCreatorToolTile, detRSlut, "TILE");
  if (sc.isFailure()){
    (*m_log) << MSG::WARNING << "Failed to initialize Tile data" << endreq;
    errorFlag = true;
    readTCaloFromSG = false;
  } else {
    if (addLUT(TILE,detRSlut).isFailure()){
      (*m_log) << MSG::WARNING << "Failed to mount Tile data" << endreq;
      errorFlag = true;
    }
    else{
      (*m_log) <<  MSG::DEBUG << "Tile Read from SG" << endreq;
    }
  }
  
  if(!readTCaloFromSG) { // Tile Still needs file
    (*m_log) <<  MSG::DEBUG << "Will try files for Tile" << endreq;
    sc = initTileCalo();
    if (sc.isFailure()){
      (*m_log) <<  MSG::WARNING << "Failed to initialize Tile Calo data" << endreq;
      errorFlag = true;
    }
  }
  m_enabledDetectors.push_back("Calorimeter");
  
  
  if (m_dumpTable) {
    (*m_log) << MSG::INFO << " >>>> Writing out all loaded tables." 
	     << endreq;
    dumpLoadedTables(m_enabledDetectors);
  }
    
  m_errorFlag |= errorFlag;

  // flag any earlier errors
  // if (errorFlag) return StatusCode::FAILURE;
  // else           return StatusCode::SUCCESS;
  // return sc;

  return errorFlag; 
  
}
//    pthread_mutex_lock(&regselsvcmutex);
//    ...
//    pthread_mutex_unlock(&regselsvcmutex);


StatusCode RegSelSvc::finalize() {
  (*m_log) << MSG::INFO << "Finalizing " << name() << endmsg;
  delete m_log;
  return Service::finalize();
}


// quick hack to protect input, don't want to modify 
// any headers so keep them local

bool checkinput(double etaMin, double etaMax, 
		double phiMin, double phiMax) { 
  
  // could do phiMin!=phiMin to protect against nan, 
  // but this does a range check as well 
  if ( !(phiMin>=-3*M_PI && phiMax<=3*M_PI) ) return false;
  if ( etaMin!=etaMin || etaMax!=etaMax )  return false;
  return true;
}

bool checkinput(double zMin,   double zMax, 
		double etaMin, double etaMax, 
		double phiMin, double phiMax) { 
  if ( zMin!=zMin || zMax!=zMax ) return false;
  return checkinput(etaMin, etaMax, phiMin, phiMax);  
}




// new RegionSelector interface for the Innner Detector 

void RegSelSvc::getRoIData(DETID detectorID,
			   const IRoiDescriptor& roi, 
			   std::vector<const RegSelModule*>& modules) 
{

  if(m_msgOutputLevel <= MSG::DEBUG && modules.size()!=0 ){
    if(m_msgOutputLevel <= MSG::WARNING ){
      (*m_log) << MSG::WARNING << " input module vector not empty for RegSelSvc call for " << detectorID << endreq; 
    }
  } 
  modules.clear();

  //  std::cout << "RegSelSvc::getRoIData()" << detectorID << std::endl; 


  RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );

  switch(detectorID){
  case PIXEL: { // Pixel
    if ( m_newpixel) m_newpixel->getRoIData( _roi, modules );
    break;    
  }
  case SCT:  { // Semiconductor Tracker
    if ( m_newsct ) m_newsct->getRoIData( _roi, modules );    
    break;
  }
  case TRT:  { // TRT
    if ( m_newtrt ) m_newtrt->getRoIData( _roi, modules );    
    break;
  }
  case RPC: { // RPC (obviously)
    if ( m_newrpc ) m_newrpc->getRoIData(_roi, modules); 
    break;
  }
  case MDT: { // MDT (obviously)
    if ( m_newmdt ) m_newmdt->getRoIData(_roi, modules); 
    break;
  }
  case TGC: { // TGC (obviously)
    if ( m_newtgc ) m_newtgc->getRoIData(_roi, modules); 
    break;
  }
  case CSC: { // CSC (obviously)
    if ( m_newcsc ) m_newcsc->getRoIData(_roi, modules); 
    break;
  }
  default:
    break;
  }  

  if(m_msgOutputLevel <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << "getRoIData for detector " << detectorID 
	     << " got " << modules.size() << " modules" << endreq;
  }
  
}







void RegSelSvc::DetHashIDList(DETID detectorID,
			      const IRoiDescriptor& roi,
			      std::vector<IdentifierHash>& IDList) 
{
  //  std::cout << "RegSelSvc::DetHashIDList() new map " << detectorID << std::endl; 

  if ( roi.composite() ) { 
    for ( unsigned iroi=roi.size() ; iroi-- ;  ) {
      DetHashIDList( detectorID, *(roi.at(iroi)), IDList );
      // std::cout << "DetHashIDList:: SuperRoi memeber " << iroi << " IDList.size() " << IDList.size() << *roi.at(iroi) << std::endl;
    }  
    if ( roi.size()>1 ) RegSelSiLUT::removeDuplicates( IDList );
    return;
  }

  if ( roi.isFullscan() ) return DetHashIDList( detectorID, IDList );

  if(m_msgOutputLevel <= MSG::DEBUG && IDList.size()!=0 ){
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " input IDList vector not empty for RegSelSvc call for detectorID " << detectorID << endreq; 
  }


  /// for calorimeter tables

  double etaMin = roi.etaMinus();
  double etaMax = roi.etaPlus();

  double phiMin = roi.phiMinus();
  double phiMax = roi.phiPlus();


  switch(detectorID){
  case PIXEL: { // Pixel    
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newpixel ) m_newpixel->getHashList(_roi, IDList); 
    break;
  }
  case SCT: { // Semiconductor Tracker (and pixel)
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newsct ) m_newsct->getHashList(_roi, IDList); 
    break;
  }
  case TRT: { // TRT (obviously)
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newtrt ) m_newtrt->getHashList(_roi, IDList); 
    break;
  }
  case MDT: { // etc
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newmdt ) m_newmdt->getHashList(_roi, IDList); 
    break;
  }
  case RPC: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newrpc ) m_newrpc->getHashList(_roi, IDList); 
    break;
  }
  case TGC: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newtgc ) m_newtgc->getHashList(_roi, IDList); 
    break;
  }
  case CSC: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newcsc ) m_newcsc->getHashList(_roi, IDList); 
    break;
  }
  case LAR: {  // Liquid Argon Calorimeter
    m_larData.regionSelector(etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TTEM: { // Liquid Argon Trigger Tower EM Calorimeter
    m_ttemData.regionSelector(etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TTHEC: { // Liquid Argon Trigger Tower HEC Calorimeter
    m_tthecData.regionSelector(etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case FCALEM: { // Liquid Argon Trigger Tower HEC Calorimeter
    m_ttfcalemData.regionSelector(etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case FCALHAD: { // Liquid Argon Trigger Tower HEC Calorimeter
    m_ttfcalhadData.regionSelector(etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TILE: { // Tile Calorimeters
    verifyInputs(TILE, etaMin, etaMax, phiMin, phiMax);
    m_tileData.regionSelector(TILE,etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  default:
    break;
  }

  if(m_msgOutputLevel <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << "DetHashIDList for detector " << detectorID 
	     << " got " << IDList.size() << " hash IDs" << endreq;
  }
  
}


void RegSelSvc::DetHashIDList(DETID detectorID, long layer,
			      const IRoiDescriptor& roi,
			      std::vector<IdentifierHash>& IDList) 
{
  //  std::cout << "RegSelSvc::DetHashIDList() new map " << detectorID << std::endl; 

  if ( roi.composite() ) { 
    for ( unsigned iroi=roi.size() ; iroi-- ;  ) {
      DetHashIDList( detectorID, layer, *(roi.at(iroi)), IDList );
    }  
    if ( roi.size()>1 ) RegSelSiLUT::removeDuplicates( IDList );
    return;
  }

  if ( roi.isFullscan() ) return DetHashIDList( detectorID, layer, IDList );

  if(m_msgOutputLevel <= MSG::DEBUG && IDList.size()!=0 ){
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " input IDList vector not empty for RegSelSvc call for detectorID " << detectorID << endreq; 
  }


  /// for calorimeter tables

  double etaMin = roi.etaMinus();
  double etaMax = roi.etaPlus();

  double phiMin = roi.phiMinus();
  double phiMax = roi.phiPlus();

  long sampling = layer; 

  switch(detectorID){
  case PIXEL: { // Pixel    
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newpixel ) m_newpixel->getHashList(_roi, layer, IDList); 
    break;
  }
  case SCT: { // Semiconductor Tracker 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newsct ) m_newsct->getHashList(_roi, layer, IDList); 
    break;
  }
  case TRT: { // TRT (obviously)
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newtrt ) m_newtrt->getHashList(_roi, layer, IDList); 
    break;
  }
  case MDT: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newmdt ) m_newmdt->getHashList(_roi, layer, IDList); 
    break;
  }
  case RPC: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newrpc ) m_newrpc->getHashList(_roi, layer, IDList); 
    break;
  }
  case TGC: {
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newtgc ) m_newtgc->getHashList(_roi, layer, IDList); 
    break;
  }
  case CSC: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newcsc ) m_newcsc->getHashList(_roi, layer, IDList); 
    break;
  }
  case LAR: { // Liquid Argon Calorimeter
    m_larData.regionSelector(sampling, etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TTEM: { // Liquid Argon Trigger Tower EM Calorimeter
    m_ttemData.regionSelector(sampling, etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TTHEC: { // Liquid Argon Trigger Tower HEC Calorimeter
    m_tthecData.regionSelector(sampling, etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case FCALEM: { // Liquid Argon Trigger Tower FCALEM Calorimeter
    m_ttfcalemData.regionSelector(sampling, etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case FCALHAD: { // Liquid Argon Trigger Tower FCALHAD Calorimeter
    m_ttfcalhadData.regionSelector(sampling, etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TILE: // Tile Calorimeters
    break;
  default:
    break;
  }

  if(m_msgOutputLevel <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << "DetHashIDList for detector " << detectorID 
	<< " got " << IDList.size() << " hash IDs" << endreq;
  }

}












// detector full scan

void RegSelSvc::DetHashIDList(DETID detectorID, 
			      std::vector<IdentifierHash>& IDList ) 
{ 
  //  std::cout << "RegSelSvc::DetHashIDList() new map " << detectorID << std::endl;   
  
  if(m_msgOutputLevel <= MSG::DEBUG && IDList.size()!=0 ){
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " input IDList vector not empty for RegSelSvc call for detectorID " << detectorID << endreq; 
  }
  IDList.clear();

  switch(detectorID){
  case PIXEL: { // Pixel    
    if ( m_newpixel ) m_newpixel->getHashList(IDList); 
    break;
  }
  case SCT: { // Semiconductor Tracker (and pixel)
    if ( m_newsct ) m_newsct->getHashList(IDList); 
    break;
  }
  case TRT: { // TRT (obviously)
    if ( m_newtrt ) m_newtrt->getHashList(IDList); 
    break;
  }
  case MDT: { 
    if ( m_newmdt ) m_newmdt->getHashList(IDList); 
    break;
  }
  case RPC: { 
    if ( m_newrpc ) m_newrpc->getHashList(IDList); 
    break;
  }
  case TGC: {
    if ( m_newtgc ) m_newtgc->getHashList(IDList); 
    break;
  }
  case CSC: { 
    if ( m_newcsc ) m_newcsc->getHashList(IDList); 
    break;
  }
  case LAR: {  // Liquid Argon Calorimeter
    /// NB: the tables themselves should have explicit, optimised full scan methods
    ///     that don't need to check the eta/phi limits of the modules!! 
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_larData.regionSelector(etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TTEM: { // Liquid Argon Trigger Tower EM Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttemData.regionSelector(etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TTHEC: { // Liquid Argon Trigger Tower HEC Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_tthecData.regionSelector(etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case FCALEM: { // Liquid Argon Trigger Tower HEC Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttfcalemData.regionSelector(etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case FCALHAD: { // Liquid Argon Trigger Tower HEC Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttfcalhadData.regionSelector(etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TILE: { // Tile Calorimeters
    double etaMin=-4.9; double etaMax=4.9; double phiMin=0; double phiMax=2*M_PI; 
    verifyInputs(TILE, etaMin, etaMax, phiMin, phiMax);
    m_tileData.regionSelector(TILE,etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  default:
    break;
  }
} 








void RegSelSvc::DetHashIDList(DETID detectorID, long layer,
			      std::vector<IdentifierHash>& IDList) 
{
  //  std::cout << "RegSelSvc::DetHashIDList() new map " << detectorID << std::endl; 

  if(m_msgOutputLevel <= MSG::DEBUG && IDList.size()!=0 ){
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " input IDList vector not empty for RegSelSvc call for detectorID " << detectorID << endreq; 
  }
  IDList.clear();

  long sampling = layer; 

  switch(detectorID){
  case PIXEL: { // Pixel    
    if ( m_newpixel ) m_newpixel->getHashList( layer, IDList); 
    break;
  }
  case SCT: { // Semiconductor Tracker 
    if ( m_newsct ) m_newsct->getHashList( layer, IDList); 
    break;
  }
  case TRT: { // TRT (obviously)
    if ( m_newtrt ) m_newtrt->getHashList( layer, IDList); 
    break;
  }
  case MDT: { 
    if ( m_newmdt ) m_newmdt->getHashList( layer, IDList); 
    break;
  }
  case RPC: { 
    if ( m_newrpc ) m_newrpc->getHashList( layer, IDList); 
    break;
  }
  case TGC: {
    if ( m_newtgc ) m_newtgc->getHashList( layer, IDList); 
    break;
  }
  case CSC: { 
    if ( m_newcsc ) m_newcsc->getHashList( layer, IDList); 
    break;
  }
  case LAR: { // Liquid Argon Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_larData.regionSelector(sampling, etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TTEM: { // Liquid Argon Trigger Tower EM Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttemData.regionSelector(sampling, etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case TTHEC: { // Liquid Argon Trigger Tower HEC Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_tthecData.regionSelector(sampling, etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case FCALEM: { // Liquid Argon Trigger Tower FCALEM Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttfcalemData.regionSelector(sampling, etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  case FCALHAD: { // Liquid Argon Trigger Tower FCALHAD Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttfcalhadData.regionSelector(sampling, etaMin, etaMax, phiMin, phiMax, IDList);
    break;
  }
  default:
    break;
  }

  if(m_msgOutputLevel <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << "DetHashIDList for detector " << detectorID 
	<< " got " << IDList.size() << " hash IDs" << endreq;
  }

}















void RegSelSvc::DetROBIDListUint(DETID detectorID,
				 const IRoiDescriptor& roi,
				 std::vector<uint32_t>& outputROBIDList) 
{

  if ( roi.composite() ) { 
    setDuplicateRemoval( false );
    for ( unsigned iroi=roi.size() ; iroi-- ;  ) {
      DetROBIDListUint( detectorID, *(roi.at(iroi)), outputROBIDList );
      // std::cout << "DetROBIDListUint:: SuperRoi memeber " << iroi << " outputROBIDList.size() " << outputROBIDList.size() << std::endl;
    }  
    RegSelSiLUT::removeDuplicates( outputROBIDList );
    setDuplicateRemoval( true );
    return;
  }

  if ( roi.isFullscan() ) return DetROBIDListUint( detectorID, outputROBIDList );

  //  std::cout << "RegSelSvc::DetROBIDListUint() new map " << detectorID << std::endl; 

  if(m_msgOutputLevel <= MSG::DEBUG && outputROBIDList.size()!=0 ){
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " input outputROBIDList vector not empty for RegSelSvc call for detectorID " << detectorID << endreq; 
  }
  //  outputROBIDList.clear();




  /// for calorimeter tables

  double etaMin = roi.etaMinus();
  double etaMax = roi.etaPlus();

  double phiMin = roi.phiMinus();
  double phiMax = roi.phiPlus();


  switch (detectorID) {
  case PIXEL: { // Pixel
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newpixel ) m_newpixel->getRobList(_roi, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case SCT: {  // SCT
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newsct ) m_newsct->getRobList(_roi, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case TRT: {  // TRT
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newtrt ) m_newtrt->getRobList(_roi, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case MDT: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newmdt ) m_newmdt->getRobList(_roi, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case RPC: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newrpc ) m_newrpc->getRobList(_roi, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case TGC: {
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newtgc ) m_newtgc->getRobList(_roi, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case CSC: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newcsc ) m_newcsc->getRobList(_roi, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case LAR: {  // Liquid Argon Calorimeter
    m_larData.regionSelectorRobIdUint( etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case TTEM: {  // Liquid Argon Trigger Tower EM Calorimeter
    m_ttemData.regionSelectorRobIdUint( etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case TTHEC:  {  // Liquid Argon Trigger Tower HEC Calorimeter
    m_tthecData.regionSelectorRobIdUint( etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case FCALEM: {  // Liquid Argon Trigger Tower FCALEM Calorimeter
    m_ttfcalemData.regionSelectorRobIdUint( etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case FCALHAD: { // Liquid Argon Trigger Tower FCALHAD Calorimeter
    m_ttfcalhadData.regionSelectorRobIdUint( etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case TILE: { /// TILE Cal
    verifyInputs(TILE, etaMin, etaMax, phiMin, phiMax);
    m_tileData.regionSelectorRobIdUint(TILE, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
  }
  default:
    break;
  }
  
  if(m_msgOutputLevel <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << "DetROBIDListUint for detector " << detectorID 
	<< " got " << outputROBIDList.size() << " ROB IDs" << endreq;
  }

}





void RegSelSvc::DetROBIDListUint(DETID detectorID, long layer,
				 const IRoiDescriptor& roi, 
				 std::vector<uint32_t>& outputROBIDList) 
{
  //  std::cout << "RegSelSvc::DetROBIDListUint() new map " << detectorID << std::endl; 

  if ( roi.composite() ) { 
    setDuplicateRemoval(false);
    for ( unsigned iroi=roi.size() ; iroi-- ;  ) {
      DetROBIDListUint( detectorID, layer, *(roi.at(iroi)), outputROBIDList );
    }  
    RegSelSiLUT::removeDuplicates( outputROBIDList );
    setDuplicateRemoval(true); 
    return;
  }
  

  if ( roi.isFullscan() ) return DetROBIDListUint( detectorID, layer, outputROBIDList );

  if(m_msgOutputLevel <= MSG::DEBUG && outputROBIDList.size()!=0 ){
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " input outputROBIDList vector not empty for RegSelSvc call for detectorID " << detectorID << endreq; 
  }

  /// for calorimeter tables

  double etaMin = roi.etaMinus();
  double etaMax = roi.etaPlus();

  double phiMin = roi.phiMinus();
  double phiMax = roi.phiPlus();

  long sampling = layer; 

  switch (detectorID) {
  case PIXEL: { // Pixel
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newpixel ) m_newpixel->getRobList(_roi, layer, outputROBIDList, m_duplicateRemoval );
    break;
  }
  case SCT: {  // SCT
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newsct ) m_newsct->getRobList(_roi, layer, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case TRT: { // TRT
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newtrt ) m_newtrt->getRobList(_roi, layer, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case MDT: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newmdt ) m_newmdt->getRobList(_roi, layer, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case RPC: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newrpc ) m_newrpc->getRobList(_roi, layer, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case TGC: {
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newtgc ) m_newtgc->getRobList(_roi, layer, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case CSC: { 
    RegSelRoI _roi( roi.zedMinus(), roi.zedPlus(), roi.phiMinus(), roi.phiPlus(), roi.etaMinus(), roi.etaPlus() );
    if ( m_newcsc ) m_newcsc->getRobList(_roi, layer, outputROBIDList, m_duplicateRemoval ); 
    break;
  }
  case LAR: { // Liquid Argon Calorimeter
    m_larData.regionSelectorRobIdUint(sampling, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case TTEM: { // Liquid Argon Trigger Tower EM Calorimeter
    m_ttemData.regionSelectorRobIdUint(sampling, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case TTHEC: { // Liquid Argon Trigger Tower HEC Calorimeter
    m_tthecData.regionSelectorRobIdUint(sampling, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case FCALEM: { // Liquid Argon Trigger Tower FCALEM Calorimeter
    m_ttfcalemData.regionSelectorRobIdUint(sampling, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case FCALHAD: { // Liquid Argon Trigger Tower FCALHAD Calorimeter
    m_ttfcalhadData.regionSelectorRobIdUint(sampling, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  default:
    break;
  }
  
  if(m_msgOutputLevel <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << "DetROBIDListUint for detector " << detectorID 
	<< " got " << outputROBIDList.size() << " ROB IDs" << endreq;
  }

}





// inner detector full scan method

void RegSelSvc::DetROBIDListUint(DETID detectorID, 
				 std::vector<uint32_t>&    outputROBIDList ) 
{ 
  if(m_msgOutputLevel <= MSG::DEBUG && outputROBIDList.size()!=0 ){
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " input outputROBIDList vector not empty for RegSelSvc call for detectorID " << detectorID << endreq; 
  }
  outputROBIDList.clear();

  switch (detectorID) {
  case PIXEL: { // Pixel
    if ( m_newpixel ) m_newpixel->getRobList(outputROBIDList); 
    break;
  }
  case SCT: {  // SCT
    if ( m_newsct ) m_newsct->getRobList(outputROBIDList); 
    break;
  }
  case TRT: {  // TRT
    if ( m_newtrt ) m_newtrt->getRobList(outputROBIDList); 
    break;
  }
  case MDT: { 
    if ( m_newmdt ) m_newmdt->getRobList(outputROBIDList);
    break;
  }
  case RPC: { 
    if ( m_newrpc ) m_newrpc->getRobList(outputROBIDList);
    break;
  }
  case TGC: {
    if ( m_newtgc ) m_newtgc->getRobList(outputROBIDList);
    break;
  }
  case CSC: { 
    if ( m_newcsc ) m_newcsc->getRobList(outputROBIDList);
    break;
  }
  case LAR: {  // Liquid Argon Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_larData.regionSelectorRobIdUint( etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case TTEM: {  // Liquid Argon Trigger Tower EM Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttemData.regionSelectorRobIdUint( etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case TTHEC:  {  // Liquid Argon Trigger Tower HEC Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_tthecData.regionSelectorRobIdUint( etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case FCALEM: {  // Liquid Argon Trigger Tower FCALEM Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttfcalemData.regionSelectorRobIdUint( etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case FCALHAD: { // Liquid Argon Trigger Tower FCALHAD Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttfcalhadData.regionSelectorRobIdUint( etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case TILE: { /// TILE Cal
    double etaMin=-4.9; double etaMax=4.9; double phiMin=0;double phiMax=2*M_PI; 
    verifyInputs(TILE, etaMin, etaMax, phiMin, phiMax);
    m_tileData.regionSelectorRobIdUint(TILE, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
  }
  default:
    break;
  }
} 








void RegSelSvc::DetROBIDListUint(DETID detectorID, long layer,
				 std::vector<uint32_t>& outputROBIDList) 
{
  //  std::cout << "RegSelSvc::DetROBIDListUint() new map " << detectorID << std::endl; 

  if(m_msgOutputLevel <= MSG::DEBUG && outputROBIDList.size()!=0 ){
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " input outputROBIDList vector not empty for RegSelSvc call for detectorID " << detectorID << endreq; 
  }
  outputROBIDList.clear();

  long sampling = layer;

  switch (detectorID) {
  case PIXEL: { // Pixel
    if ( m_newpixel ) m_newpixel->getRobList( layer, outputROBIDList ); 
    break;
  }
  case SCT: {  // SCT
    if ( m_newsct ) m_newsct->getRobList( layer, outputROBIDList ); 
    break;
  }
  case TRT: { // TRT
    if ( m_newtrt ) m_newtrt->getRobList( layer, outputROBIDList ); 
    break;
  }
  case MDT: { 
    if ( m_newmdt ) m_newmdt->getRobList( layer, outputROBIDList );
    break;
  }
  case RPC: { 
    if ( m_newrpc ) m_newrpc->getRobList( layer, outputROBIDList );
    break;
  }
  case TGC: {
    if ( m_newtgc ) m_newtgc->getRobList( layer, outputROBIDList );
    break;
  }
  case CSC: { 
    if ( m_newcsc ) m_newcsc->getRobList( layer, outputROBIDList );
    break;
  }
  case LAR: { // Liquid Argon Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_larData.regionSelectorRobIdUint(sampling, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case TTEM: { // Liquid Argon Trigger Tower EM Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttemData.regionSelectorRobIdUint(sampling, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case TTHEC: { // Liquid Argon Trigger Tower HEC Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_tthecData.regionSelectorRobIdUint(sampling, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case FCALEM: { // Liquid Argon Trigger Tower FCALEM Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttfcalemData.regionSelectorRobIdUint(sampling, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  case FCALHAD: { // Liquid Argon Trigger Tower FCALHAD Calorimeter
    double etaMin=-4.8; double etaMax=4.8; double phiMin=-M_PI;double phiMax=M_PI; 
    m_ttfcalhadData.regionSelectorRobIdUint(sampling, etaMin, etaMax, phiMin, phiMax, outputROBIDList);
    break;
  }
  default:
    break;
  }
  
  if(m_msgOutputLevel <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << "DetROBIDListUint for detector " << detectorID 
	<< " got " << outputROBIDList.size() << " ROB IDs" << endreq;
  }

}










//
// Some internal helper methods
//

StatusCode RegSelSvc::readFromSG( ToolHandle<IRegionLUT_Creator> p_lutCreatorTool, const RegionSelectorLUT*& detRSlut, const std::string lutName){
  // Use generic tool to create LUT - instance set via ToolHandle property
  if (!p_lutCreatorTool) {
    (*m_log) << MSG::INFO << "LUT creator tool not configured " << p_lutCreatorTool << endreq;
  } else {
    if ( p_lutCreatorTool.retrieve().isFailure() ) {
      (*m_log) << MSG::FATAL << "Failed to retrieve tool " << p_lutCreatorTool << endreq;
      return StatusCode::FAILURE;
    } else {
      (*m_log) << MSG::INFO << "Retrieved tool " << p_lutCreatorTool << endreq;
      // compute LUT - lutName is empty by default but for LAr tool, need to specify something like EM
      detRSlut = p_lutCreatorTool->getLUT(lutName);
      if (!detRSlut) {
        (*m_log) << MSG::ERROR << "lookup table missing" << endreq;
	return StatusCode::FAILURE;
      } else {
	//        (*m_log) << MSG::VERBOSE << "Test lookup table" << endreq;
        printTable(detRSlut);
      }
    }
  }
  return StatusCode::SUCCESS;
}




StatusCode RegSelSvc::readFromSG( ToolHandle<IRegionIDLUT_Creator> p_lutCreatorTool,  RegSelSiLUT*& detRSlut){
  // Use generic tool to create LUT - instance set via ToolHandle property
  // NB: this is *very* misleading, the test !p_lutCreatorTool *isn't* just a test, 
  //     it actually tries to create the tool if it doesn't already exist, and is 
  //     false if it fails to create it 
  if (!p_lutCreatorTool) {
    (*m_log) << MSG::INFO << "LUT creator tool not configured " << p_lutCreatorTool << endreq;
  } else {
    if ( p_lutCreatorTool.retrieve().isFailure() ) {
      (*m_log) << MSG::FATAL << "Failed to retrieve tool " << p_lutCreatorTool << endreq;
      return StatusCode::FAILURE;
    } else {
      (*m_log) << MSG::INFO << "Retrieved tool " << p_lutCreatorTool << endreq;
      detRSlut = p_lutCreatorTool->getLUT();
      if (detRSlut) {
	(*m_log) << MSG::INFO << "retrieved new RegSelSiLUT map " << detRSlut->getName() << endreq;
      }
      else {
        (*m_log) << MSG::ERROR << "new lookup table missing" << endreq;
	return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}




StatusCode RegSelSvc::readFromSG( ToolHandle<IRegionFTKLUT_Creator> p_lutCreatorTool,  RegSelEtaPhiLUT*& detRSlut){
  // Use generic tool to create LUT - instance set via ToolHandle property
  // NB: this is *very* misleading, the test !p_lutCreatorTool *isn't* just a test, 
  //     it actually tries to create the tool if it doesn't already exist, and is 
  //     false if it fails to create it 
  if (!p_lutCreatorTool) {
    (*m_log) << MSG::INFO << "LUT creator tool not configured " << p_lutCreatorTool << endreq;
  } else {
    if ( p_lutCreatorTool.retrieve().isFailure() ) {
      (*m_log) << MSG::FATAL << "Failed to retrieve tool " << p_lutCreatorTool << endreq;
      return StatusCode::FAILURE;
    } else {
      (*m_log) << MSG::INFO << "Retrieved tool " << p_lutCreatorTool << endreq;
      detRSlut = p_lutCreatorTool->getLUT();
      if (detRSlut) {
	(*m_log) << MSG::INFO << "retrieved new RegSeletaPhiLUT map " << endreq;
      }
      else {
        (*m_log) << MSG::ERROR << "new lookup table missing" << endreq;
	return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}



//// const detRSlut is copied so pointer set but not returned.
StatusCode RegSelSvc::readFromSG( DETID type, const RegionSelectorLUT*& detRSlut ){
  std::string detKey;
  StatusCode sc = StatusCode::SUCCESS;

  //! Read DETID type RegionSelectorLUT from Detector Store
  detKey = returnKeyString(type);
  sc = m_detStore->contains< RegionSelectorLUT >(detKey);
  //const DataHandle<RegionSelectorLUT> *detRSlutDH;
  if (sc == StatusCode::SUCCESS ) {
    sc = m_detStore->retrieve(detRSlut, detKey);
    if( sc.isFailure() || detRSlut==0 ) {
      (*m_log) << MSG::FATAL << "Could not retrieve RegionSelectorLUT " << detKey << " from detStore." << endreq;
      return sc;
    } else {
      (*m_log) << MSG::DEBUG << "RegionSelectorLUT " << detKey << " successfully retrieved from Detector Store" << endreq;
    }
  } else {
    (*m_log) << MSG::FATAL << " RegionSelectorLUT " << detKey << " doesn't exists " << endreq;
  }
  return sc;
}


//! Mount Region Selector maps
StatusCode RegSelSvc::addLUT(DETID type, const RegionSelectorLUT* detRSlut){
  StatusCode sc;
  switch(type){
  case LAR:
    m_larData.addLut( (detRSlut) );
    openDataStatus(sc,LAR, m_larData);
    break;
  case TTEM:
    m_ttemData.addLut( (detRSlut) );
    openDataStatus(sc,TTEM, m_ttemData);
    break;
  case TTHEC:
    m_tthecData.addLut( (detRSlut) );
    openDataStatus(sc,TTHEC, m_tthecData);
    break;
  case FCALEM:
    m_ttfcalemData.addLut( (detRSlut) );
    openDataStatus(sc,FCALEM, m_ttfcalemData);
    break;
  case FCALHAD:
    m_ttfcalhadData.addLut( (detRSlut) );
    openDataStatus(sc,FCALHAD, m_ttfcalhadData);
    break;
  case TILE: 
    m_tileData.addLut( (*detRSlut) );
    openDataStatus(sc,TILE, m_tileData);
    break;
  default:
    break;
  }
  return sc;
}

std::string RegSelSvc::returnKeyString(DETID type){

  std::string rsKey;

  switch(type){
  case PIXEL: rsKey = "PixelRegionSelectorLUT"; break;
  case SCT: rsKey = "SctRegionSelectorLUT"; break;
  case TRT: rsKey = "TrtRegionSelectorLUT"; break;
  case LAR: rsKey = "LArRegionSelectorLUT_EM"; break;
  case TTEM: rsKey = "LArRegionSelectorLUT_EM"; break;
  case TTHEC: rsKey = "LArRegionSelectorLUT_HEC"; break;
  case FCALEM: rsKey = "LArRegionSelectorLUT_FCALem"; break;
  case FCALHAD: rsKey = "LArRegionSelectorLUT_FCALhad"; break;
  case TILE: rsKey = "TileRegionSelectorLUT"; break;
  case MDT: rsKey = "MdtRegionSelectorLUT"; break;
  case RPC: rsKey = "RpcRegionSelectorLUT"; break;
  case TGC: rsKey = "TgcRegionSelectorLUT"; break;
  case CSC: rsKey = "CscRegionSelectorLUT"; break;
  default: break;  
  }

  return rsKey;
}



StatusCode RegSelSvc::initCalo(void){

  StatusCode sc;
  int i;

  //! Initialization (reading the data input files)
  char filename[50];
  m_larData.initvar();
  m_ttemData.initvar();
  for(i=0; i < 4; i++){
    snprintf(filename,50,"TrigTowerMapEM_%i.txt",i);
    sc = m_larData.read(filename);
    sc = m_ttemData.read(filename);
    if(sc == StatusCode::FAILURE){
      (*m_log) << MSG::FATAL << " >>>>> Could't read " << filename << " data file" << endreq;
      return sc;
    }  
  }
  openDataStatus(sc, TTEM , m_ttemData);
  
  m_tthecData.initvar();
  for(i=0; i < 4; i++){
    snprintf(filename,50,"TrigTowerMapHEC_%i.txt",i);
    sc = m_larData.read(filename);
    sc = m_tthecData.read(filename);
    if(sc == StatusCode::FAILURE){
      (*m_log) << MSG::FATAL << " >>>>> Could't read " << filename << " data file" << endreq;
      return sc;
    }  
  }
  openDataStatus(sc, LAR , m_larData);
  openDataStatus(sc, TTHEC , m_tthecData);
  sc = initTileCalo();
  return sc;
}



StatusCode RegSelSvc::initTileCalo(void){

  StatusCode sc;

  //! Initialization (reading the data input files)
  char filename[50];

  strcpy(filename,"TileMapIdAndHash.txt");
  sc = m_tileData.read(filename,TILE);
  openDataStatus(sc,TILE, m_tileData);

  return sc;
}


StatusCode RegSelSvc::initMuon(void){
  /// NB: muon detectors are set up completely 
  /// automatically now
  return StatusCode::SUCCESS;
}

void RegSelSvc::verifyInputs(DETID Type,
				double &etaminIn, double &etamaxIn,
				double &phiminIn, double &phimaxIn){

  StatusCode sc;

  switch(Type){
  case LAR:
    m_larData.verifyInputs(etaminIn,etamaxIn,phiminIn,phimaxIn); 
    if(sc == StatusCode::FAILURE) { } 
    break;
  case TTEM:
    m_ttemData.verifyInputs(etaminIn,etamaxIn,phiminIn,phimaxIn); 
    if(sc == StatusCode::FAILURE) { } 
    break;
  case TTHEC:
    m_tthecData.verifyInputs(etaminIn,etamaxIn,phiminIn,phimaxIn); 
    if(sc == StatusCode::FAILURE) { } 
    break;
  case FCALEM:
    m_ttfcalemData.verifyInputs(etaminIn,etamaxIn,phiminIn,phimaxIn); 
    if(sc == StatusCode::FAILURE) { } 
    break;
  case FCALHAD:
    m_ttfcalhadData.verifyInputs(etaminIn,etamaxIn,phiminIn,phimaxIn); 
    if(sc == StatusCode::FAILURE) { }
    break;
  case TILE:
    sc = m_tileData.verifyInputs(etaminIn,etamaxIn,phiminIn,phimaxIn); 
    if(sc == StatusCode::FAILURE) { 
      if(m_msgOutputLevel <= MSG::DEBUG ) 
	(*m_log) << MSG::DEBUG << " >>>>> TILE: Out of Eta Range (" << m_tileData.etaminValue() 
		 << ", " << m_tileData.etamaxValue() << ")" << endreq;
    } 
    break;
  default:
    break;
  }

}

void RegSelSvc::GetEtaPhi(DETID detectorID, 
			  IdentifierHash hashId, 
			  double *etaMin, double *etaMax,
			  double *phiMin, double *phiMax ) {


  if (m_msgOutputLevel <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << ">>>   RegSelSvc::getEtaPhi() " << endreq;
  }
	    
  switch(detectorID){
  case LAR: // Liquid Argon Calorimeter
    m_larData.getEtaPhi(hashId, etaMin, etaMax, phiMin, phiMax);
    break;
  case TTEM: // Liquid Argon Trigger Tower EM Calorimeter
    m_ttemData.getEtaPhi(hashId, etaMin, etaMax, phiMin, phiMax);
    break;
  case TTHEC: // Liquid Argon Trigger Tower HEC Calorimeter
    m_tthecData.getEtaPhi(hashId, etaMin, etaMax, phiMin, phiMax);
    break;
  case FCALEM: // Liquid Argon Trigger Tower FCALEM Calorimeter
    m_ttfcalemData.getEtaPhi(hashId, etaMin, etaMax, phiMin, phiMax);
    break;
  case FCALHAD: // Liquid Argon Trigger Tower FCALHAD Calorimeter
    m_ttfcalhadData.getEtaPhi(hashId, etaMin, etaMax, phiMin, phiMax);
    break;
  case TILE: // Tile Calorimeters
    m_tileData.getEtaPhi(hashId, etaMin, etaMax, phiMin, phiMax);
    break;
#if 0
  case MDT: // MDT Calorimeter
    m_mdtData.getEtaPhi(hashId, etaMin, etaMax, phiMin, phiMax);
    break;
  case RPC: // RPC Calorimeters
    m_rpcData.getEtaPhi(hashId, etaMin, etaMax, phiMin, phiMax);
    break;
  case TGC: // TGC Calorimeters
    m_tgcData.getEtaPhi(hashId, etaMin, etaMax, phiMin, phiMax);
    break;
  case CSC: // CSC Calorimeters
    m_cscData.getEtaPhi(hashId, etaMin, etaMax, phiMin, phiMax);
    break;
#endif
  default:
    break;
  }

  if (m_msgOutputLevel <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG
	<< ">>> HashId " << hashId 
	<< " -> EtaMin " << etaMin << "| EtaMax " << etaMax
	<< "| PhiMin " << phiMin  << "| PhiMax " << phiMax
	<< endreq;
  }

}

void RegSelSvc::openDataStatus(StatusCode &sc, DETID type,
			       RegSelectorMap &subDetData){
  char strtmp[20];
  bool flag = false;

  sc = StatusCode::SUCCESS;

  switch(type){
  case PIXEL: strcpy(strtmp,"PIXEL"); break;
  case SCT: strcpy(strtmp,"SCT"); break;
  case TRT: strcpy(strtmp,"TRT"); break;
  case LAR: strcpy(strtmp,"LAR"); flag = true; break;
  case TTEM: strcpy(strtmp,"TT EM"); flag = true; break;
  case TTHEC: strcpy(strtmp,"TT HEC"); flag = true; break;
  case FCALEM: strcpy(strtmp,"FCALEM"); flag = true; break;
  case FCALHAD: strcpy(strtmp,"FCALHAD"); flag = true; break;
  case TILE: strcpy(strtmp,"TILE"); break;
  case MDT: strcpy(strtmp,"MDT"); break;
  case RPC: strcpy(strtmp,"RPC"); break;
  case TGC: strcpy(strtmp,"TGC"); break;
  case CSC: strcpy(strtmp,"CSC"); break;
  default: break;
  }

  if(sc == StatusCode::SUCCESS){
    subDetData.mountDataStruct();
  }
  else{
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " >>>>> Could't read " << strtmp << " data file"
	       << endreq;
    sc = StatusCode::FAILURE;
  }
  
  if(flag)
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " >>>>> OpenDataStatus no RegSelectorMap for " << strtmp << " detector" << endreq;
}

void RegSelSvc::openDataStatus(StatusCode &sc, DETID type,
				RegSelectorHashMap &subDetHashData){
  char strtmp[20];
  bool flag = false;

  sc = StatusCode::SUCCESS;

  switch(type){
  case PIXEL: strcpy(strtmp,"PIXEL"); break;
  case SCT: strcpy(strtmp,"SCT"); break;
  case TRT: strcpy(strtmp,"TRT"); break;
  case LAR: strcpy(strtmp,"LAR"); flag = true; break;
  case TTEM: strcpy(strtmp,"TT EM"); flag = true; break;
  case TTHEC: strcpy(strtmp,"TT HEC"); flag = true; break;
  case FCALEM: strcpy(strtmp,"FCALEM"); flag = true; break;
  case FCALHAD: strcpy(strtmp,"FCALHAD"); flag = true; break;
  case TILE: strcpy(strtmp,"TILE"); break;
  case MDT: strcpy(strtmp,"MDT"); break;
  case RPC: strcpy(strtmp,"RPC"); break;
  case TGC: strcpy(strtmp,"TGC"); break;
  case CSC: strcpy(strtmp,"CSC"); break;
  default: break;
  }

  if(sc == StatusCode::SUCCESS){
    if(flag)    subDetHashData.mountDataStruct();
  }
  else{
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " >>>>> Could't read " << strtmp << " data file"
	       << endreq;
    sc = StatusCode::FAILURE;
  }

  if(flag == false)
    if(m_msgOutputLevel <= MSG::WARNING ) 
      (*m_log) << MSG::WARNING << " >>>>> OpenDataStatus no RegSelectorHashMap for " << strtmp << " detector" << endreq;
  
}



void RegSelSvc::boundaryValues(DETID detectorID,
			       double &etaminValue, double &etamaxValue,
			       double &phiminValue, double &phimaxValue){

  switch(detectorID){
  case LAR: // Liquid Argon Calorimeter
    phiminValue = m_larData.phiminValue(); phimaxValue = m_larData.phimaxValue();
    etaminValue = m_larData.etaminValue(); etamaxValue = m_larData.etamaxValue();
    break;
  case TTEM: // Liquid Argon EM Calorimeter
    phiminValue = m_ttemData.phiminValue(); phimaxValue = m_ttemData.phimaxValue();
    etaminValue = m_ttemData.etaminValue(); etamaxValue = m_ttemData.etamaxValue();
    break;
  case TTHEC: // Liquid Argon HEC Calorimeter
    phiminValue = m_tthecData.phiminValue(); phimaxValue = m_tthecData.phimaxValue();
    etaminValue = m_tthecData.etaminValue(); etamaxValue = m_tthecData.etamaxValue();
    break;
  case FCALEM: // Liquid Argon FCALEM Calorimeter
    phiminValue = m_ttfcalemData.phiminValue(); phimaxValue = m_ttfcalemData.phimaxValue();
    etaminValue = m_ttfcalemData.etaminValue(); etamaxValue = m_ttfcalemData.etamaxValue();
    break;
  case FCALHAD: // Liquid Argon FCALHAD Calorimeter
    phiminValue = m_ttfcalhadData.phiminValue(); phimaxValue = m_ttfcalhadData.phimaxValue();
    etaminValue = m_ttfcalhadData.etaminValue(); etamaxValue = m_ttfcalhadData.etamaxValue();
    break;
  case TILE: // Tile Calorimeters
    phiminValue = m_tileData.phiminValue(); phimaxValue = m_tileData.phimaxValue();
    etaminValue = m_tileData.etaminValue(); etamaxValue = m_tileData.etamaxValue();
    break; 
  default:
    break;
  }

}

// loop over all the enabled detector groups (Indet, Calorimeter, Muon),
// call dumpLoadedTables for each one in turn.
void RegSelSvc::dumpLoadedTables(std::vector<std::string>& enabledDetectors){
  std::vector<std::string>::const_iterator d;
  for (d=enabledDetectors.begin(); d!=enabledDetectors.end(); ++d){
    dumpLoadedTables(*d);
  }

}

void RegSelSvc::dumpLoadedTables(const std::string& detTypeStr){

  std::vector<std::string> filenames;
  std::vector<std::string> detName;
  unsigned int i;
  
  getFilenames(detTypeStr, filenames, detName);
  for( i = 0; i < filenames.size(); i++){
    std::ofstream fileout;
    fileout.open(filenames[i].c_str());
    (*m_log) << MSG::DEBUG << " >>>>> Writing table " << filenames[i] 
        << endreq;
    if(detTypeStr == "Muon")
      fileout << "hashId sampling layer phimin phimax etamin etamax robId" << std::endl;
    else
      fileout << "hashId barrel/endcap# layer/disk# phimin phimax etamin etamax robId" << std::endl;
    
    writeFile(fileout, detName[i], detTypeStr);
    fileout.close();
  }

}

void RegSelSvc::getFilenames(const std::string& detTypeStr, std::vector<std::string>& filename, std::vector<std::string>& detName){
  
  std::string tmp;
  unsigned int i;
  
  std::string::size_type pos = m_roiFileName.value().find (".",0);
  getDetname(detTypeStr, detName);
  if( pos != 0 ){
    for( i = 0; i < detName.size(); i++){
      tmp = m_roiFileName.value();
      tmp.insert (pos,detName[i]);
      filename.push_back(tmp);
    }
  }
  else {
    for( i = 0; i < detName.size(); i++){
      tmp = m_roiFileName.value() + detName[i] + ".txt";
      filename.push_back(tmp);
    }
  }
}


void RegSelSvc::getDetname(const std::string& detTypeStr, std::vector<std::string>& detName){
  
  if(detTypeStr == "Inner"){
    detName.push_back("PIXEL");
    detName.push_back("SCT");
    detName.push_back("TRT");
  }
  else if(detTypeStr == "Calorimeter"){
    detName.push_back("LAR");
    detName.push_back("TTEM");
    detName.push_back("TTHEC");
    detName.push_back("FCALEM");
    detName.push_back("FCALHAD");
    detName.push_back("Tile");
  }
  else if(detTypeStr == "Muon"){
    detName.push_back("MDT");
    detName.push_back("RPC");
    detName.push_back("TGC");
  }
}

void RegSelSvc::writeFile(std::ofstream& fileout, const std::string& detName, const std::string& detTypeStr){
  
  RegSelectorMap rsData;
  RegSelectorHashMap rsHashData;
  std::vector<IdentifierHash> hashId;
  std::vector<int>  layer, sampling;
  std::vector<uint32_t> robId1;
  std::vector<std::vector<uint32_t> > robId2;
  std::vector<double> etamin, etamax;
  std::vector<double> phimin, phimax;
  
  if( detTypeStr == "Calorimeter"){
    switchDetector(detName, rsHashData);
    hashId = rsHashData.hashIdOut();
    sampling = rsHashData.sampleOut();
    layer = rsHashData.layerOut();
    phimin = rsHashData.phiMinOut();
    phimax = rsHashData.phiMaxOut();
    etamin = rsHashData.etaMinOut();
    etamax = rsHashData.etaMaxOut();
    robId2 = rsHashData.robIdOut();
  }
  else{
    switchDetector(detName, rsData);
    hashId = rsData.hashIdOut();
    sampling = rsData.barORendOut();
    layer = rsData.layORdskOut();
    phimin = rsData.phiMinOut();
    phimax = rsData.phiMaxOut();
    etamin = rsData.etaMinOut();
    etamax = rsData.etaMaxOut();
    robId1 = rsData.robIdOut();
  }

  for( unsigned int i = 0; i < hashId.size(); i++){
    fileout << hashId[i] << " "
	    << sampling[i] << " "
	    << layer[i] << " "
	    << phimin[i] << " "
	    << phimax[i] << " "
	    << etamin[i] << " "
	    << etamax[i] << " ";
    if( detTypeStr == "Calorimeter"){
      //      for ( std::vector<std::vector<uint32_t> >::iterator robIdIter  = robId2[i].begin();
      //	    robIdIter != robId2[i].end(); ++robIdIter;){
      //	fileout << robIdIter << ",";
      //}
    } 
    else {
      fileout << robId1[i];
    }
    fileout << std::endl;
  }

}

void RegSelSvc::switchDetector(const std::string& detName,RegSelectorMap& rsData){

  if(detName == "TILE")  rsData = m_tileData;
}

void RegSelSvc::switchDetector(const std::string& detName, RegSelectorHashMap& rsHashData){
  if(detName == "LAR")
    rsHashData = m_larData;
  else if(detName == "TTEM")
    rsHashData = m_ttemData;
  else if(detName == "TTHEC")
    rsHashData = m_tthecData;
  else if(detName == "FCALEM")
    rsHashData = m_ttfcalemData;
  else if(detName == "FCALHAD")
    rsHashData = m_ttfcalhadData;
}

void RegSelSvc::printTable(const RegionSelectorLUT * lut){
  if(m_msgOutputLevel <= MSG::VERBOSE ) {
    for (unsigned int idHash = 0; idHash < lut->maxHash(); idHash++) {
      (*m_log) << MSG::VERBOSE << idHash << " " << lut->phiMin(idHash) << endreq;
    }
  }
}

void RegSelSvc::DisableSiRobList(const std::vector<uint32_t>& RobIdList) { 
  // if the list of robs to disable is not empty... 
  if ( RobIdList.size()>0 ) { 
    // check the look up tables actually already exist before
    // trying to disable the robs from them
    if ( m_newpixel ) m_newpixel->disableRobList(RobIdList);
    if ( m_newsct )   m_newsct->disableRobList(RobIdList);
    if ( m_newtrt )   m_newtrt->disableRobList(RobIdList);
  } 
}

void RegSelSvc::DisablePixelHashList(const std::vector<unsigned int>& HashList) { 
  // if the list of hashids to disable is not empty... 
  if ( HashList.size()>0 ) { 
    // check the look up tables actually already exist before
    // trying to disable the modules from them
    
    std::vector<IdentifierHash> hashlist;
    for ( std::vector<unsigned int>::const_iterator hptr(HashList.begin()) ; 
	  hptr!=HashList.end() ; hptr ++ ) hashlist.push_back(*hptr);

    if ( m_newpixel ) m_newpixel->disableModuleList(hashlist);
  } 
}

void RegSelSvc::DisableSCTHashList(const std::vector<unsigned int>& HashList) { 
  // if the list of hashids to disable is not empty... 
  if ( HashList.size()>0 ) { 
    // check the look up tables actually already exist before
    // trying to disable the modules from them
    std::vector<IdentifierHash> hashlist;
    for ( std::vector<unsigned int>::const_iterator hptr(HashList.begin()) ; 
	  hptr!=HashList.end() ; hptr ++ ) hashlist.push_back(*hptr);
    if ( m_newsct )   m_newsct->disableModuleList(hashlist);
  } 
}


void RegSelSvc::DisableTRTHashList(const std::vector<unsigned int>& HashList) { 
  // if the list of hashids to disable is not empty... 
  if ( HashList.size()>0 ) { 
    // check the look up tables actually already exist before
    // trying to disable the modules from them
    std::vector<IdentifierHash> hashlist;
    for ( std::vector<unsigned int>::const_iterator hptr(HashList.begin()) ; 
	  hptr!=HashList.end() ; hptr ++ ) hashlist.push_back(*hptr);
    if ( m_newtrt )   m_newtrt->disableModuleList(hashlist);
  } 
}


void RegSelSvc::RestoreSiRobList() {
  // check the look up tables actually already exist before
  // trying to disable the robs from them
  if ( m_newpixel ) m_newpixel->enableRobs();
  if ( m_newsct )   m_newsct->enableRobs();
  if ( m_newtrt )   m_newtrt->enableRobs();
}



StatusCode RegSelSvc::reinitialize() { 
  if ( !reinitialiseInternalFromOKS() )   return StatusCode::FAILURE;
  if ( reinitialiseInternal() )           return StatusCode::SUCCESS;
  else                                    return StatusCode::FAILURE;
}


// routine which can be called at any time to get the list of rob 
// ids from oks and enable the modules corresponding to the 
// specified robs
bool RegSelSvc::reinitialiseInternalFromOKS() { 

  // don't disable anything if not initialising from oks
  if ( m_readSiROBListFromOKS.value()==false ) return true;

  if ( m_initPixel.value() )  {   
    // disable everything
    m_newpixel->disableRobs();
    
    // enable the robs from OKS
    m_newpixel->enableRobList(m_enabledROBs.value());
  }

  if ( m_initSCT.value() ) { 
    m_newsct->disableRobs();
    
    // enable robs from list for pixel and sct
    m_newsct->enableRobList(m_enabledROBs.value());
  }
  
  if ( m_initTRT.value() )  {   
    // disable everything
    m_newtrt->disableRobs();
    
    // enable the robs from OKS
    m_newtrt->enableRobList(m_enabledROBs.value());
  }

  return true;
}





bool RegSelSvc::reinitialiseInternal() { 

  // NB: the conditions summary service names, and probably the services 
  //     themselves should be setable from a job option, but why make
  //     life even more complicated with configurability that may never
  //     be used. If it needs it, it can easily be added  

  // now handle all the disabling for the pixel ... 
  if ( m_initPixel.value() )  { 

    // first disable modules from the conditions summary services
    //    if ( m_disableFromConditions ) disableIDFromConditions(m_newpixel, "PixelConditionsSummarySvc");
    if ( m_disableFromConditions && m_disablePixelFromConditions ) disableIDFromConditions(m_newpixel, m_PixConditionsSvc);

    // now *disable* the modules from robs the user has flagged
    if ( m_deleteRobList.size() ) m_newpixel->disableRobList(m_deleteRobList);

    // and disable any other modules from the module list
    // have to convert the std::vector<unsigned int> into std::vector<IdentifierHash>  
    std::vector<IdentifierHash> hashlist;
    for ( std::vector<unsigned int>::const_iterator hptr(m_deletePixelHashList.begin()) ; 
	  hptr!=m_deletePixelHashList.end() ; hptr ++ ) hashlist.push_back(*hptr);
    
    if ( m_deletePixelHashList.size() ) m_newpixel->disableModuleList(hashlist);    
  }
  
  // and now handle all the disabling for the sct ... 
  if ( m_initSCT.value() ) { 
    
    // first disable modules from the conditions summary services
    //    if ( m_disableFromConditions ) disableIDFromConditions(m_newsct, "SCT_ConditionsSummarySvc");
    if ( m_disableFromConditions  && m_disableSCTFromConditions ) disableIDFromConditions(m_newsct, m_SCTConditionsSvc);

    // now *disable* the modules from robs the user has flagged
    if ( m_deleteRobList.size() ) m_newsct->disableRobList(m_deleteRobList);
    
    // and disable any other modules from the module list
    // have to convert the std::vector<unsigned int> into std::vector<IdentifierHash>  
    std::vector<IdentifierHash> hashlist;
    for ( std::vector<unsigned int>::const_iterator hptr(m_deleteSCTHashList.begin()) ; 
	  hptr!=m_deleteSCTHashList.end() ; hptr ++ ) hashlist.push_back(*hptr);
    
    if ( m_deleteSCTHashList.size() ) m_newsct->disableModuleList(hashlist);
  }

  // and now handle all the disabling for the trt ... 
  if ( m_initTRT.value() ) { 

    // first disable modules from the conditions summary services
    //    if ( m_disableFromConditions ) disableIDFromConditions(m_newtrt, "TRT_ConditionsSummarySvc");
    if ( m_disableFromConditions  && m_disableTRTFromConditions ) disableIDFromConditions(m_newtrt, m_TRTConditionsSvc);
    
    // now *disable* the modules from robs the user has flagged
    if ( m_deleteRobList.size() ) m_newtrt->disableRobList(m_deleteRobList);

    // and disable any other modules from the module list
    // have to convert the std::vector<unsigned int> into std::vector<IdentifierHash>  
    std::vector<IdentifierHash> hashlist;
    for ( std::vector<unsigned int>::const_iterator hptr(m_deleteTRTHashList.begin()) ; 
	  hptr!=m_deleteTRTHashList.end() ; hptr ++ ) hashlist.push_back(*hptr);
    
    if ( m_deleteTRTHashList.size() ) m_newtrt->disableModuleList(hashlist);
  }


  return true;
}



bool RegSelSvc::GetEnabledROBsFromOKS() 
{
  bool robOKSconfigFound = false;

  IJobOptionsSvc* p_jobOptionSvc;
  if (service("JobOptionSvc", p_jobOptionSvc).isFailure()) {
    (*m_log) << MSG::ERROR << "Could not find JobOptionSvc" << endreq;
    return robOKSconfigFound;
  } 
  
  // get the data flow config property  
  const std::vector<const Property*>* dataFlowProps = p_jobOptionSvc->getProperties("DataFlowConfig");
  
  // go through looking for the enabled robs
  std::vector<const Property*>::const_iterator propp(dataFlowProps->begin());
  for ( ; propp!=dataFlowProps->end() ; propp++) {
    // the enabled ROB list is found
    if ( (*propp)->name() == "DF_Enabled_ROB_IDs" ) {
      if (m_enabledROBs.assign(**propp)) {
	robOKSconfigFound = true;
	(*m_log) << MSG::INFO << " ---> Read from OKS                       = " 
	    << m_enabledROBs.value().size() << " enabled ROB IDs." << endreq;

	// is m_enabledROBs.value() a std::vector<uint32_t> ????
	
	//	std::vector<uint32_t>& r = m_enabledROBs.value();

      } else {
	if(m_msgOutputLevel <= MSG::WARNING ) 
	  (*m_log) << MSG::WARNING << " Could not set Property 'enabledROBs' from OKS." << endreq;
      }
    }
  }

  p_jobOptionSvc->release();

  return robOKSconfigFound;
}
 
void RegSelSvc::DisableMissingROBs(const std::vector<uint32_t>& vec){

    m_larData.DisableMissingROBs( vec );
    m_ttemData.DisableMissingROBs( vec );
    m_tthecData.DisableMissingROBs( vec );
    m_ttfcalemData.DisableMissingROBs( vec );
    m_ttfcalhadData.DisableMissingROBs( vec );

}





