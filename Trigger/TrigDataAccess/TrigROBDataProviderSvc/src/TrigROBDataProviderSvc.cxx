/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//===================================================================
//  Implementation of TrigROBDataProviderSvc
//
//      For valid ROB Source Ids, Sub Det Ids and ROB Status elements see the event format
//      document ATL-D-ES-0019 (EDMS)
//             
//===================================================================

// Include files.
#include "TrigROBDataProviderSvc.h"
#include "TrigMonitorBase/TrigLockedHist.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IAlgorithm.h"

#include "CxxUtils/excepts.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert> 
#include <bitset> 
#include <TH1F.h>
#include <TH2F.h>

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) 
#   define CAN_REBIN(hist)  hist->SetCanExtend(TH1::kAllAxes)
#else
#   define CAN_REBIN(hist)  hist->SetBit(TH1::kCanRebin)
#endif

// Constructor.
TrigROBDataProviderSvc::TrigROBDataProviderSvc(const std::string& name, ISvcLocator* svcloc)
  :ROBDataProviderSvc(name,svcloc),
   m_callerName("UNKNOWN"),
   m_algContextSvc(0),   
   m_storeGateSvc( "StoreGateSvc", name ),
   m_onlineRunning(false),
   m_removeEmptyROB(false),
   m_maskL2EFModuleID(false),  
   m_isEventComplete(false),
   m_msg(0),
   m_RobRequestInfo(0),
   m_histProp_requestedROBsPerCall(Gaudi::Histo1DDef("RequestedROBsPerCall" ,0,300,50)),
   m_histProp_receivedROBsPerCall(Gaudi::Histo1DDef("ReceivedROBsPerCall" ,0,300,50)),
   m_histProp_timeROBretrieval(Gaudi::Histo1DDef("TimeForROBretrieval" ,0.,500.,50)),
   m_histProp_retrievedROBsPerAlgo(Gaudi::Histo1DDef("RetrievedROBsPerAlgo" ,0.,1.,1)),
   m_histProp_ROSRequest(Gaudi::Histo1DDef("ROSRequests" ,0.,1.,1)),
   m_hist_requestedROBsPerCall(0),
   m_hist_receivedROBsPerCall(0),
   m_hist_retrievedROBsPerAlgo(0),
   m_hist_timeROBretrieval(0),
   m_hist_genericStatusForROB(0),
   m_hist_specificStatusForROB(0),
   m_hist_ROSRequest(0)
{
  declareProperty("ignoreROB", m_ignoreROB,"List of ROBs to ignore for retrieval");
  declareProperty("enabledROBs", m_enabledROBs,"List of enabled detector ROBs");
  declareProperty("LArMetROBs", m_enabledLArMetROBs,"List of enabled LAr MET ROBs");
  declareProperty("TileMetROBs", m_enabledTileMetROBs,"List of enabled Tile MET ROBs");
  declareProperty("RobRosMapping", m_rob_ros_map,"Mapping of ROBs to ROSes");
  declareProperty("readROBfromOKS", m_readROBfromOKS=false,"Read enabled ROBs from OKS");
  declareProperty("doMonitoring", m_doMonitoring=false,"Enable histograms");
  declareProperty("doDetailedROBMonitoring", m_doDetailedROBMonitoring=false,"Produce ROB cost data");
  declareProperty("ROBDataMonitorCollectionSGName", m_ROBDataMonitorCollection_SG_Name="ROBDataMonitorCollection","Name of cost monitoring collection in SG");
  declareProperty("HistRequestedROBsPerCall", m_histProp_requestedROBsPerCall,"Number of ROBs requested");
  declareProperty("HistReceivedROBsPerCall", m_histProp_receivedROBsPerCall,"Number of ROBs received");
  declareProperty("HistTimeROBretrieval", m_histProp_timeROBretrieval,"Timing for ROB retrieval");
  declareProperty("HistRetrievedROBsPerAlgo", m_histProp_retrievedROBsPerAlgo,"Number of ROB retrieval per Algo");
  declareProperty("ModuleIDGenericLArMetROB", m_genericLArMetModuleID=0xffff,"Generic module id for LAr MET ROB retrieval");  
  declareProperty("ModuleIDGenericTileMetROB", m_genericTileMetModuleID=0xffff,"Generic module id for Tile MET ROB retrieval");
  declareProperty("SeparateMETandDetROBRetrieval", m_separateMETandDetROBRetrieval=true,"Separate retrieval of MET and detector ROBs");
  declareProperty("UseROSmappingForROBRetrieval", m_useROSmappingForROBRetrieval=true,"Use a ROB-ROS mapping for ROB prefetching");

  declareProperty("SimulateOnlineDataAccess", m_simulateOnlineDataAccess=true,"Simulate online ROB data access");
  declareProperty("PrintStackTraceGetROBData", m_printStackTraceGetROBData=false,"Print a stck trace for failed ROB requests");

  // fill map with generic status codes
  m_map_GenericStatus[eformat::UNCLASSIFIED]      = "UNCLASSIFIED"; 
  m_map_GenericStatus[eformat::BCID_CHECK_FAIL]   = "BCID_CHECK_FAIL"; 
  m_map_GenericStatus[eformat::LVL1ID_CHECK_FAIL] = "LVL1ID_CHECK_FAIL"; 
  m_map_GenericStatus[eformat::TIMEOUT]           = "TIMEOUT"; 
  m_map_GenericStatus[eformat::DATA_CORRUPTION]   = "DATA_CORRUPTION"; 
  m_map_GenericStatus[eformat::INTERNAL_OVERFLOW] = "INTERNAL_OVERFLOW"; 

  // fill vector with specific status codes
  m_vec_SpecificStatus.reserve(16);
  m_vec_SpecificStatus.push_back("TRIGGER_TYPE_SYNC_ERROR"); 
  m_vec_SpecificStatus.push_back("FRAGMENT_SIZE_ERROR"); 
  m_vec_SpecificStatus.push_back("DATABLOCK_ERROR"); 
  m_vec_SpecificStatus.push_back("CTRL_WORD_ERROR"); 
  m_vec_SpecificStatus.push_back("MISSING_BOF"); 
  m_vec_SpecificStatus.push_back("MISSING_EOF"); 
  m_vec_SpecificStatus.push_back("INVALID_HEADER_MARKER"); 
  m_vec_SpecificStatus.push_back("FORMAT_ERROR"); 
  m_vec_SpecificStatus.push_back("DUPLICATE_EVENT"); 
  m_vec_SpecificStatus.push_back("SEQUENCE_ERROR"); 
  m_vec_SpecificStatus.push_back("TRANSMISSION_ERROR"); 
  m_vec_SpecificStatus.push_back("TRUNCATION"); 
  m_vec_SpecificStatus.push_back("SHORT_FRAGMENT"); 
  m_vec_SpecificStatus.push_back("FRAGMENT_LOST"); 
  m_vec_SpecificStatus.push_back("FRAGMENT_PENDING"); 
  m_vec_SpecificStatus.push_back("ROL_DISABLED"); 
} 

// Destructor.
TrigROBDataProviderSvc::~TrigROBDataProviderSvc()
{
}

// Initialization 
StatusCode TrigROBDataProviderSvc::initialize()
{
  StatusCode sc = ROBDataProviderSvc::initialize();

  // set message stream
  m_msg  = new MsgStream( msgSvc(), name() );
  
  m_callerName = "UNKNOWN";

  IntegerProperty baseOutputLevel;
  baseOutputLevel.assign(ROBDataProviderSvc::getProperty("OutputLevel")) ;
  if (m_msg) m_msg->setLevel( baseOutputLevel.value() );

  logStream() << MSG::INFO << " ---> TrigROBDataProviderSvc = " << name() << " initialize "
      << " - package version " << PACKAGE_VERSION << endmsg ;

  // get Property filterEmptyROB from base class
  if ( !sc.isSuccess() ) {
    logStream() << MSG::ERROR << " ROBDataProviderSvc::initialize() failed." << endmsg;
    return sc;
  } else {
    BooleanProperty filterEmptyROB;
    filterEmptyROB.setName("filterEmptyROB");
    if (filterEmptyROB.assign(getProperty("filterEmptyROB"))) {
      m_removeEmptyROB = filterEmptyROB.value() ;
      logStream() << MSG::INFO << " ---> getProperty('filterEmptyROB')       = " << filterEmptyROB << endmsg;
    } else {
      logStream() << MSG::WARNING << " ROBDataProviderSvc::getProperty('filterEmptyROB') failed." << endmsg;
    }
  }

  // intialize L1 ROBIds
  m_l1_ROB_ids.reserve(49);   

  m_l1_ROB_ids.push_back(0x910081);    // TDAQ_CALO_TOPO_PROC ROBs
  m_l1_ROB_ids.push_back(0x910091);
 
  // ROBs from ROIB
  m_l1_ROB_ids.push_back(0x770001);  // CTP
  m_l1_ROB_ids.push_back(0x760001);  // muCTPi
  m_l1_ROB_ids.push_back(0x76000b);  // L1Muctpi_on_RDO

  m_l1_ROB_ids.push_back(0x7300a8);  // Calo cluster RoI
  m_l1_ROB_ids.push_back(0x7300a9);  // Calo cluster RoI
  m_l1_ROB_ids.push_back(0x7300aa);  // Calo cluster RoI
  m_l1_ROB_ids.push_back(0x7300ab);  // Calo cluster RoI
  m_l1_ROB_ids.push_back(0x7500ac);  // Calo Jet/Energy RoI
  m_l1_ROB_ids.push_back(0x7500ad);  // Calo Jet/Energy RoI

  // ROBs from CPMSim/CPCMX -24
  m_l1_ROB_ids.push_back(0x720008);  
  m_l1_ROB_ids.push_back(0x720018);  
  m_l1_ROB_ids.push_back(0x720028);  
  m_l1_ROB_ids.push_back(0x720038);  
  m_l1_ROB_ids.push_back(0x720009);  
  m_l1_ROB_ids.push_back(0x720019);  
  m_l1_ROB_ids.push_back(0x720029);  
  m_l1_ROB_ids.push_back(0x720039);  
  m_l1_ROB_ids.push_back(0x72000a);  
  m_l1_ROB_ids.push_back(0x72001a);  
  m_l1_ROB_ids.push_back(0x72002a);  
  m_l1_ROB_ids.push_back(0x72003a);  
  m_l1_ROB_ids.push_back(0x72000b);  
  m_l1_ROB_ids.push_back(0x72001b);  
  m_l1_ROB_ids.push_back(0x72002b);  
  m_l1_ROB_ids.push_back(0x72003b);  
  m_l1_ROB_ids.push_back(0x730088);  
  m_l1_ROB_ids.push_back(0x730098);  
  m_l1_ROB_ids.push_back(0x730089);  
  m_l1_ROB_ids.push_back(0x730099);  
  m_l1_ROB_ids.push_back(0x73008a);  
  m_l1_ROB_ids.push_back(0x73009a);  
  m_l1_ROB_ids.push_back(0x73008b);  
  m_l1_ROB_ids.push_back(0x73009b);  

  // ROBs from JEMJetSim/JEMEnergySim -12
  m_l1_ROB_ids.push_back(0x74000c);  
  m_l1_ROB_ids.push_back(0x74001c);  
  m_l1_ROB_ids.push_back(0x74002c);  
  m_l1_ROB_ids.push_back(0x74003c);  
  m_l1_ROB_ids.push_back(0x74000d);  
  m_l1_ROB_ids.push_back(0x74001d);  
  m_l1_ROB_ids.push_back(0x74002d);  
  m_l1_ROB_ids.push_back(0x74003d);  
  m_l1_ROB_ids.push_back(0x75008c);  
  m_l1_ROB_ids.push_back(0x75009c);  
  m_l1_ROB_ids.push_back(0x75008d);  
  m_l1_ROB_ids.push_back(0x75009d);  

 // ROBs from DAQ
  m_l1_ROB_ids.push_back(0x770000);  // CTP    DAQ
  m_l1_ROB_ids.push_back(0x760000);  // muCTPi DAQ
  // preload also L2 result, when available
  m_l1_ROB_ids.push_back(0x7b0000);  // L2 HLT result

 
  // get the list of enabled ROBs from OKS
  bool robOKSconfigFound = false;
  bool robLArMetOKSconfigFound = false;
  bool robTileMetOKSconfigFound = false;
  bool robRosOKSconfigFound = false;

  if ( m_readROBfromOKS.value() ) {
    ServiceHandle<IJobOptionsSvc> p_jobOptionsSvc("JobOptionsSvc", name());
    if ((p_jobOptionsSvc.retrieve()).isFailure()) {
      logStream() << MSG::ERROR << "Could not find JobOptionsSvc" << endmsg;
    } else {
      const std::vector<const Property*>* dataFlowProps = p_jobOptionsSvc->getProperties("DataFlowConfig");
      if (dataFlowProps) {
	for ( std::vector<const Property*>::const_iterator cur = dataFlowProps->begin();
	      cur != dataFlowProps->end(); cur++) {
	  // the enabled ROB list is found
	  if ( (*cur)->name() == "DF_Enabled_ROB_IDs" ) {
	    if (m_enabledROBs.assign(**cur)) {
	      robOKSconfigFound = true;
	      logStream() << MSG::INFO << " ---> Read from OKS                       = " << m_enabledROBs.value().size() << " enabled ROB IDs." << endmsg;
	    } else {
	      logStream() << MSG::WARNING << " Could not set Property 'enabledROBs' from OKS." << endmsg;
	    }
	  }

	  // the LAr MET ROB list is found
	  if ( (*cur)->name() == "DF_LAr_MET_ROB_IDs" ) {
	    if (m_enabledLArMetROBs.assign(**cur)) {
	      robLArMetOKSconfigFound = true;
	      logStream() << MSG::INFO << " ---> Read from OKS                       = " << m_enabledLArMetROBs.value().size() << " LAr MET ROB IDs." << endmsg;
	    } else {
	      logStream() << MSG::WARNING << " Could not set Property 'LArMetROBs' from OKS." << endmsg;
	    }
	  }

	  // the Tile MET ROB list is found
	  if ( (*cur)->name() == "DF_Tile_MET_ROB_IDs" ) {
	    if (m_enabledTileMetROBs.assign(**cur)) {
	      robTileMetOKSconfigFound = true;
	      logStream() << MSG::INFO << " ---> Read from OKS                       = " << m_enabledTileMetROBs.value().size() << " Tile MET ROB IDs." << endmsg;
	    } else {
	      logStream() << MSG::WARNING << " Could not set Property 'TileMetROBs' from OKS." << endmsg;
	    }
	  }

	  // the ROB-ROS mapping is found
	  if ( (*cur)->name() == "DF_ROB_ROS_Mapping" ) {
	    if (m_rob_ros_map.assign(**cur)) {
	      robRosOKSconfigFound = true;
	      logStream() << MSG::INFO << " ---> Read from OKS                       = " << m_rob_ros_map.value().size() << " ROB/ROS mapping values." << endmsg;
	    } else {
	      logStream() << MSG::WARNING << " Could not set Property 'RobRosMapping' from OKS." << endmsg;
	    }
	  }
	}
      } else {
	logStream() << MSG::WARNING << " No 'DataFlowConfig' properties available in JobOptionsSvc " << endmsg;
      }
      p_jobOptionsSvc.release().ignore();
    }
  }

  logStream() << MSG::INFO << " ---> TrigROBDataProviderSvc              = " << name() << " special properties <---" << endmsg;
  logStream() << MSG::INFO << " ---> Filter out empty ROB fragments      = " << m_removeEmptyROB << endmsg;
  logStream() << MSG::INFO << " ---> Fill monitoring histograms          = " << m_doMonitoring << endmsg;
  logStream() << MSG::INFO << "        Hist:RequestedROBsPerCall         = " << m_histProp_requestedROBsPerCall << endmsg; 
  logStream() << MSG::INFO << "        Hist:ReceivedROBsPerCall          = " << m_histProp_receivedROBsPerCall << endmsg; 
  logStream() << MSG::INFO << "        Hist:TimeROBretrieval             = " << m_histProp_timeROBretrieval << endmsg; 
  logStream() << MSG::INFO << "        Hist:RetrievedROBsPerAlgo         = " << m_histProp_retrievedROBsPerAlgo << endmsg; 
  logStream() << MSG::INFO << "        Hist:ROSRequest                   = " << m_histProp_ROSRequest << endmsg; 
  logStream() << MSG::INFO << " ---> Do detailed ROB monitoring          = " << m_doDetailedROBMonitoring << endmsg;
  logStream() << MSG::INFO << " ---> SG name for ROB monitoring collect. = " << m_ROBDataMonitorCollection_SG_Name << endmsg;
  logStream() << MSG::INFO << " ---> Read list of enabled ROBs from OKS  = " << m_readROBfromOKS << endmsg;
  if (m_enabledROBs.value().size() == 0) {
    logStream() << MSG::INFO << " ---> The list of enabled ROBs has size   = 0. No check will be performed " << endmsg;
  } else {
    if (m_readROBfromOKS.value() && robOKSconfigFound) {
      logStream() << MSG::INFO << " ---> The list of enabled ROBs has size   = " << m_enabledROBs.value().size() 
		  << ". It was read from the partition database."  << endmsg;
    } else {
      logStream() << MSG::INFO << " ---> The list of enabled ROBs has size   = " << m_enabledROBs.value().size() 
		  << ". It was read from job options."  << endmsg;
    }
  }

  logStream() << MSG::INFO << " ---> Generic Module ID for LAr MET ROB   = " << m_genericLArMetModuleID << endmsg;
  if (m_enabledLArMetROBs.value().size() == 0) {
    logStream() << MSG::INFO << " ---> The list of LAr MET ROBs has size   = 0. No LAr MET ROB access will be done." << endmsg;
  } else {
    if (m_readROBfromOKS.value() && robLArMetOKSconfigFound) {
      logStream() << MSG::INFO << " ---> The list of LAr MET ROBs has size   = " << m_enabledLArMetROBs.value().size() 
		  << ". It was read from the partition database."  << endmsg;
    } else {
      logStream() << MSG::INFO << " ---> The list of LAr MET ROBs has size   = " << m_enabledLArMetROBs.value().size() 
		  << ". It was read from job options."  << endmsg;
    }
  }

  logStream() << MSG::INFO << " ---> Generic Module ID for Tile MET ROB  = " << m_genericTileMetModuleID << endmsg;
  if (m_enabledTileMetROBs.value().size() == 0) {
    logStream() << MSG::INFO << " ---> The list of Tile MET ROBs has size  = 0. No Tile MET ROB access will be done." << endmsg;
  } else {
    if (m_readROBfromOKS.value() && robTileMetOKSconfigFound) {
      logStream() << MSG::INFO << " ---> The list of Tile MET ROBs has size  = " << m_enabledTileMetROBs.value().size() 
		  << ". It was read from the partition database."  << endmsg;
    } else {
      logStream() << MSG::INFO << " ---> The list of Tile MET ROBs has size  = " << m_enabledTileMetROBs.value().size() 
		  << ". It was read from job options."  << endmsg;
    }
  }

  logStream() << MSG::INFO << " ---> Separate MET and Det ROB Retrieval  = " << m_separateMETandDetROBRetrieval << endmsg;

  if (m_rob_ros_map.value().size() == 0) {
    logStream() << MSG::INFO << " ---> The mapping of ROBs to ROSes has size  = 0. No ROB/ROS mapping will be used for ROB prefetching." << endmsg;
    m_useROSmappingForROBRetrieval = false; // reset the job property
  } else {
    if (m_readROBfromOKS.value() && robRosOKSconfigFound) {
      logStream() << MSG::INFO << " ---> The ROB/ROS mapping has size  = " << m_rob_ros_map.value().size() 
		  << ". It was read from the partition database."  << endmsg;
    } else {
      logStream() << MSG::INFO << " ---> The ROB/ROS mapping has size  = " << m_rob_ros_map.value().size() 
		  << ". It was read from job options."  << endmsg;
    }
  }

  logStream() << MSG::INFO << " ---> Use ROB/ROS mapping for ROB prefetching/retrieval = " << m_useROSmappingForROBRetrieval  << endmsg;

  if (m_ignoreROB.value().size() == 0) {
    logStream() << MSG::INFO << " ---> The list of ROBs to ignore has size = 0. No check will be performed " << endmsg;
  } else {
    logStream() << MSG::INFO << " ---> The list of ROBs to ignore has size = " << m_ignoreROB.value().size() << endmsg;
  }

  for (unsigned int i=0; i<m_ignoreROB.value().size(); i++) {
    logStream() << MSG::INFO << " ---> do not retrieve ROB[" << i << "]: hex(id)=0x"
		<< MSG::hex << m_ignoreROB.value()[i]<<MSG::dec 
		<< " dec(id)="<< m_ignoreROB.value()[i] << endmsg;
  }

  logStream() << MSG::INFO << " ---> Simulate online ROB data access (Network access)  = " << m_simulateOnlineDataAccess << endmsg;
  if (m_simulateOnlineDataAccess.value() && !m_useROSmappingForROBRetrieval.value()) {
    logStream() << MSG::ERROR << " +-----------------------------------------------------------------------------------------------+ " << endmsg;
    logStream() << MSG::ERROR << " | Simulate online data access is switched on but no ROB-ROS mapping is provided.                | " << endmsg;
    logStream() << MSG::ERROR << " | Online simulation is not meainigful in this case.                                             | " << endmsg;
    logStream() << MSG::ERROR << " | Set 'UseROSmappingForROBRetrieval' to True and provide a ROB-ROS mapping with 'RobRosMapping' | " << endmsg;
    logStream() << MSG::ERROR << " | A simple ROB-ROS mapping can be generated from the Bytestream input file with the scipt       | " << endmsg;
    logStream() << MSG::ERROR << " | 'generate-rob-ros-map-from-data.py' in the TrigROBDataProviderSvc package.                    | " << endmsg;
    logStream() << MSG::ERROR << " +-----------------------------------------------------------------------------------------------+ " << endmsg;
    return StatusCode::FAILURE;
  }

  logStream() << MSG::INFO << " ---> Print Stack Trace for failed ROB access  (DEBUG)  = " << m_printStackTraceGetROBData << endmsg;

  // register incident handler for begin run
  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
  if ((incidentSvc.retrieve()).isFailure()) {
    logStream() << MSG::ERROR << "Unable to locate IncidentSvc" << endmsg;
    incidentSvc.release().ignore();
    return StatusCode::FAILURE;
  }
  long int pri=100;
  incidentSvc->addListener(this,"BeginRun",pri);
  incidentSvc.release().ignore();

  // Setup the StoreGateSvc
  ATH_CHECK( m_storeGateSvc.retrieve() );

  return sc;
}

// Initialization 
StatusCode TrigROBDataProviderSvc::finalize()
{
  StatusCode sc = ROBDataProviderSvc::finalize();
  logStream() << MSG::DEBUG << "finalize()" << endmsg; 
  if ( !sc.isSuccess() ) {
    logStream() << MSG::ERROR << " ROBDataProviderSvc::finalize() failed." << endmsg;
  }

  // delete message stream
  if ( m_msg ) delete m_msg;

  // release the AlgContextSvc if used
  if ( m_algContextSvc ) m_algContextSvc->release();

  return sc;
}

/// Query interface
StatusCode TrigROBDataProviderSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IROBDataProviderSvc::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (IROBDataProviderSvc*)this;
  } else if ( ITrigROBDataProviderSvc::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (ITrigROBDataProviderSvc*)this;
  } else if ( ITrigROBDataProviderSvcPrefetch::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (ITrigROBDataProviderSvcPrefetch*)this;
  } else if ( IIncidentListener::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (IIncidentListener*)this;
  } else {
   return Service::queryInterface(riid, ppvInterface);
  } 
  addRef(); 
  return StatusCode::SUCCESS; 
}

/** - in online add ROB fragments for given ROB ids to the map
      call the DataCollector to retrieve the ROB fragments 

    - in offline only check that given ROB ids are in the map, issue an
      error if not
*/
void TrigROBDataProviderSvc::addROBData(const std::vector<uint32_t>& robIds, const std::string_view callerName) 
{ 
  //-------------------
  // remove empty calls
  //-------------------
  if (robIds.size() == 0) return;

  //--------------------
  // set the caller name
  //--------------------
  setCallerName(callerName);

  //--------------------
  // make unique ROB IDs
  //--------------------
  std::vector<uint32_t>::iterator remove_duplicate;
  std::vector<uint32_t> robIdsUnique(robIds);

  sort(robIdsUnique.begin(), robIdsUnique.end()); 
  remove_duplicate = unique(robIdsUnique.begin(), robIdsUnique.end()); 
  robIdsUnique.erase(remove_duplicate, robIdsUnique.end());

  //------------------------------------------------------------------
  // Replace the generic MET ROB ID with the full list of all MET ROBs
  //------------------------------------------------------------------
  // LAr MET ROBs
  uint32_t generic_LAr_MET_id = eformat::helper::SourceIdentifier(eformat::TDAQ_LAR_MET,m_genericLArMetModuleID.value()).code();
  std::vector<uint32_t>::iterator rob_LAr_Met_it = std::find(robIdsUnique.begin(), robIdsUnique.end(), generic_LAr_MET_id);
  if (rob_LAr_Met_it != robIdsUnique.end()) {
    robIdsUnique.erase(rob_LAr_Met_it);
    if (m_enabledLArMetROBs.value().size() != 0) robIdsUnique.insert(robIdsUnique.end(),m_enabledLArMetROBs.value().begin(),m_enabledLArMetROBs.value().end());
  }

  // Tile MET ROBs
  uint32_t generic_Tile_MET_id = eformat::helper::SourceIdentifier(eformat::TDAQ_TILE_MET,m_genericTileMetModuleID.value()).code();
  std::vector<uint32_t>::iterator rob_Tile_Met_it = std::find(robIdsUnique.begin(), robIdsUnique.end(), generic_Tile_MET_id);
  if (rob_Tile_Met_it != robIdsUnique.end()) {
    robIdsUnique.erase(rob_Tile_Met_it);
    if (m_enabledTileMetROBs.value().size() != 0) robIdsUnique.insert(robIdsUnique.end(),m_enabledTileMetROBs.value().begin(),m_enabledTileMetROBs.value().end());
  }

  //-------------------
  //--- offline running
  //-------------------
  if (!m_onlineRunning) {

    // for offline running all requested ROBs should be found in cache
    // if not issue error
    ROBDataProviderSvc::addROBData(robIdsUnique);
  //------------------
  //--- online running
  //------------------
  } else {
    // detailed ROB monitoring
    //------------------------
    // Create a ROB monitoring collection and register it to StoreGate
    ROBDataMonitorCollection* p_robMonCollection(0); 
    if ( m_doDetailedROBMonitoring.value() ) {
      if ( !(m_storeGateSvc->transientContains<ROBDataMonitorCollection>(m_ROBDataMonitorCollection_SG_Name.value())) ) {
	p_robMonCollection = new ROBDataMonitorCollection;
	if ( p_robMonCollection ) {
	  p_robMonCollection->reserve( 10 ) ;
	  if ( (m_storeGateSvc->record(p_robMonCollection, m_ROBDataMonitorCollection_SG_Name.value(), true)).isFailure() ) { 
	    logStream() << MSG::WARNING << " Registering ROB Monitoring collection in StoreGate failed." << endmsg;
	    delete p_robMonCollection;
	    p_robMonCollection = 0;
	  }
	}
      } else {
	if ( m_storeGateSvc->retrieve(p_robMonCollection).isFailure() ) {
	  logStream() << MSG::WARNING << " Retrieval of ROB Monitoring collection from StoreGate failed." << endmsg;
	  p_robMonCollection = 0;
	}
      }
    }
  
    // create a new ROBDataMonitorStruct and fill it
    robmonitor::ROBDataMonitorStruct* p_robMonStruct(0);
    if ( p_robMonCollection ) { 
      // initialize new ROBDataMonitorStruct
      p_robMonStruct = new robmonitor::ROBDataMonitorStruct(lvl1_id(), robIdsUnique, m_callerName);
    }

    // for online running the requested ROBs should be not found in cache
    // ------------------------------------------------------------------
    // vector with missing ROB ids for DataCollector
    std::vector<uint32_t> vRobIds;
    vRobIds.reserve( robIdsUnique.size() ) ;

    // find missing ROB ids which should be retrieved  
    if(logLevel() <= MSG::DEBUG)
      logStream() << MSG::DEBUG << " ---> addROBData for "<<m_callerName<<": Number of ROB Ids requested : " << robIdsUnique.size() << endmsg;

    for(std::vector<uint32_t>::const_iterator it=robIdsUnique.begin(); it!=robIdsUnique.end(); ++it){
      uint32_t id = (*it);
      // mask off the module ID for L2 and EF result for Run 1 data
      if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) &&
	   (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_LVL2) ) {
	 id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
	 if (!m_maskL2EFModuleID) {
	   ATH_MSG_ERROR("Inconsistent flag for masking L2/EF module IDs");
	   m_maskL2EFModuleID=true;
	 }
      } else if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) && 
		  (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_EVENT_FILTER) &&
		  (m_maskL2EFModuleID) ) {
	id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
      }
      // check if ROB is already in cache
      ONLINE_ROBMAP::iterator map_it = m_online_robmap.find(id) ;
      if(map_it != m_online_robmap.end()) {
	if(logLevel() <= MSG::DEBUG)
	  logStream() << MSG::DEBUG << " ---> addROBData for "<<m_callerName<<": Found   ROB Id : 0x" << MSG::hex << (*map_it).second.source_id() << MSG::dec
		      <<" in cache "<< endmsg;
	//* detailed monitoring
	if ( p_robMonStruct ) {
	  (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::CACHED;
	  (p_robMonStruct->requested_ROBs)[id].rob_size    = ((*map_it).second).fragment_size_word();
	  if ( (*map_it).second.nstatus() != 0 ) {
	    const uint32_t* it_status;
	    (*map_it).second.status(it_status);
	    for (uint32_t k=0; k < (*map_it).second.nstatus(); k++) {
	      (p_robMonStruct->requested_ROBs)[id].rob_status_words.push_back( *(it_status+k) );
	    }
	  }
	}
	continue;
      } 

      // check if ROB should be ignored
      if (m_ignoreROB.value().size() != 0) {
	std::vector<uint32_t>::const_iterator rob_ignore_it =
	  std::find(m_ignoreROB.value().begin(), m_ignoreROB.value().end(),id);
	if(rob_ignore_it != m_ignoreROB.value().end()) {
	  if(logLevel() <= MSG::DEBUG)
	    logStream() << MSG::DEBUG << " ---> addROBData for "<<m_callerName<<": ROB Id : 0x" << MSG::hex << id << MSG::dec
			<< " will be not retrieved, since it is on the veto list."<< endmsg;
	  //* detailed monitoring
	  if ( p_robMonStruct ) {
	    (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::IGNORED;
	  }
	  continue;
	}
      }

      // check if ROB is actually enabled for readout
      // do not perform this check for MET ROBs
      if ( (m_enabledROBs.value().size() != 0) && 
	   (eformat::helper::SourceIdentifier(id).subdetector_id() != eformat::TDAQ_LAR_MET) && 
	   (eformat::helper::SourceIdentifier(id).subdetector_id() != eformat::TDAQ_TILE_MET) ){
	std::vector<uint32_t>::const_iterator rob_enabled_it =
	  std::find(m_enabledROBs.value().begin(), m_enabledROBs.value().end(),id);
	if(rob_enabled_it == m_enabledROBs.value().end()) {
	  if(logLevel() <= MSG::DEBUG)
	    logStream() << MSG::DEBUG << " ---> addROBData for "<<m_callerName<<": ROB Id : 0x" << MSG::hex << id << MSG::dec
			<< " will be not retrieved, since it is not on the list of enabled ROBs."<< endmsg;
	  //* detailed monitoring
	  if ( p_robMonStruct ) {
	    (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::DISABLED;
	  }
	  continue;
	}
      }

      // check that ROB is not already on the list
      std::vector<uint32_t>::const_iterator rob_duplicate_it = std::find(vRobIds.begin(), vRobIds.end(),id);
      if(rob_duplicate_it != vRobIds.end()) {
	if(logLevel() <= MSG::DEBUG)
	  logStream() << MSG::DEBUG << " ---> addROBData for "<<m_callerName<<": ROB Id : 0x" << MSG::hex << id << MSG::dec
		      << " found multiple times on list. Only one copy will be retrieved."<< endmsg;
	continue;
      }

      // in case a ROB-ROS mapping is used, check that the id is present in the ROB-ROS map
      if ( (m_useROSmappingForROBRetrieval.value()) && (m_rob_ros_map.value().find(id) == m_rob_ros_map.value().end()) ) {
	logStream() << MSG::WARNING << " ---> addROBData for "<<m_callerName<<": ROB Id : 0x" << MSG::hex << id << MSG::dec
		    <<" is not found in the ROB-ROS mapping. ROB can not be scheduled for retrieval. Check ROB-ROS mapping."
		    << endmsg;
	continue;
      }

      // the ROB should be retrieved from the ROS
      if (logLevel() <= MSG::VERBOSE)
	logStream() << MSG::VERBOSE << " ---> addROBData for "<<m_callerName<<": Request ROB Id : 0x" << MSG::hex << id << MSG::dec
		    <<" from ROS "<< endmsg;
      vRobIds.push_back( *it ) ;
      if ( (m_useROSmappingForROBRetrieval.value()) && (m_Det_Robs_for_retrieval.find(id)==m_Det_Robs_for_retrieval.end()) ) {
	m_Det_Robs_for_retrieval[ *it ] = m_rob_ros_map.value().find(id)->second ;
      }
    } // end loop over requested input ROBs

    if (vRobIds.size() == 0) {
      if(logLevel() <= MSG::DEBUG)
	logStream() << MSG::DEBUG 
		    << " ---> addROBData for "<<m_callerName<<": either all requested ROBs are found in cache for running mode ONLINE, \n"
		    << "      or input ROB Id list was empty, \n" 
		    << "      or all requested ROBs were not retrieved due to the veto list.\n" 
		    << "      Number of requested ROB Ids = " << robIdsUnique.size() << "\n"
		    << "      Lvl1 id                     = " << lvl1_id() 
		    << endmsg;
      // Set ROB request time also in the case when no DataCollector request is necessary 
      // to allow correlation with RoI request times
      // start and stop times will be equal
      if ( p_robMonStruct ) {
	struct timeval time_start;
	struct timeval time_stop;

	gettimeofday(&time_start, 0);
	gettimeofday(&time_stop, 0);

	p_robMonStruct->start_time_of_ROB_request    = time_start;
	p_robMonStruct->end_time_of_ROB_request      = time_stop;
      }
    } else {
      //
      // Either no ROS/ROB mapping is available or it should not be used
      // add the requested ROBs already here to the ROB cache 
      // ---------------------------------------------------------------
      //
      if (!m_useROSmappingForROBRetrieval.value()) {
	std::vector<uint32_t> retrievedRobIds;
	retrievedRobIds.reserve( vRobIds.size() );
	addROBDataToCache(vRobIds, retrievedRobIds, p_robMonStruct);
      } else { // ROS/ROB mapping will be used, print the ROB ids for prefetching
	//* detailed monitoring
	if ( p_robMonStruct ) {
	  for (std::map<uint32_t,int>::const_iterator map_it=m_Det_Robs_for_retrieval.begin(); map_it!=m_Det_Robs_for_retrieval.end(); ++map_it){
	    uint32_t id =(*map_it).first;	      
	    (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::SCHEDULED;
	    (p_robMonStruct->requested_ROBs)[id].rob_size    = 1;
	  }
	}
	if(logLevel() <= MSG::DEBUG) {
	  std::ostringstream ost;
	  ost << "      Number of scheduled ROB Ids = " << m_Det_Robs_for_retrieval.size() << "\n" ;
	  unsigned int rob_counter = 1;
	  for (std::map<uint32_t,int>::const_iterator map_it=m_Det_Robs_for_retrieval.begin(); map_it!=m_Det_Robs_for_retrieval.end(); ++map_it,++rob_counter){
	    ost << "       # = "<< std::setw(5) << rob_counter << " ROB id = 0x" << std::hex << (*map_it).first << std::dec 
		<< " ROS id = 0x" << std::hex << (*map_it).second << std::dec << "\n"; 	  
	  }

	  logStream() << MSG::DEBUG 
		      << " ---> addROBData for "<<m_callerName<<": A ROB/ROS mapping is available the following ROB Ids are scheduled for retrieval in getROBData: \n"
		      << "      Lvl1 id                     = " << lvl1_id() << "\n"
		      << ost.str()
		      << endmsg;
	}
      } // end if (!m_useROSmappingForROBRetrieval.value())
    }

    // add the ROB monitoring structure to the collection
    if ( p_robMonCollection && p_robMonStruct )       p_robMonCollection->push_back( p_robMonStruct );

  } // end online running   
  return;
}

uint32_t TrigROBDataProviderSvc::lvl1_id() {
  return getEvent()->lvl1_id();
}

/** - in online add the LVL1/LVL2 result
    - rebuild the map
    - set flag for online running
*/

void TrigROBDataProviderSvc::setNextEvent(const std::vector<ROBF>& result) 
{ 
  // clear the ROB map
  m_online_robmap.clear(); 
  //m_currentLvl1ID = 0;

  // clear the maps used for prefetching
  m_Det_Robs_for_retrieval.clear();

  // set the online flag
  m_onlineRunning = true ;

  // set the complete event flag
  m_isEventComplete = false;

  // clear the ROB info object from steering
  if (m_RobRequestInfo) m_RobRequestInfo->clearRequestScheduledRobIDs();

  if ( result.size() == 0 ) {
    logStream()<<MSG::ERROR<< " ---> setNextEvent online for "<< name() 
	       <<" failed: Size of received vector of ROB fragments = " << result.size() 
	       <<endmsg;
    return;
  }

  // set the LVL1 id
  uint32_t currentLvl1ID = result[0].rod_lvl1_id();

  // add fragments to map
  std::vector<ROBF>::const_iterator it_robf     = result.begin();
  std::vector<ROBF>::const_iterator it_robf_end = result.end();

  for(; it_robf!=it_robf_end; ++it_robf) {
    uint32_t id = it_robf->source_id() ;
    // check current L1 ID against CTP fragment when possible
    if ( (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_CTP) &&
	 ( it_robf->rod_lvl1_id() != currentLvl1ID ) ) {
      logStream() << MSG::ERROR << " ---> Lvl1 ID mismatch for CTP fragment with SourceId = 0x" << MSG::hex << id << MSG::dec
		  << " and L1 Id = " << it_robf->rod_lvl1_id()  
		  << " to currently used L1 Id = " << currentLvl1ID 
		  << " -> Use CTP version from now on."<< endmsg;
      //      currentLvl1ID = it_robf->rod_lvl1_id(); // TB is this ERROR real or some past history
    }
    // remove empty ROB fragments or ones with bad status, if requested
    if ((it_robf->rod_ndata() == 0) && (m_removeEmptyROB)) { 
      if(logLevel() <= MSG::DEBUG) { 
	logStream() << MSG::DEBUG << " ---> Empty ROB Id = 0x" << MSG::hex << id << MSG::dec
		    << " removed for L1 Id = " << currentLvl1ID << endmsg;
      }  
    } else if ( ROBDataProviderSvc::filterRobWithStatus(&*it_robf) ) {
      if ((logLevel() <= MSG::DEBUG) && (it_robf->nstatus() > 0)) {
	const uint32_t* it_status;
	it_robf->status(it_status);
	eformat::helper::Status tmpstatus( (*it_status) ) ;
	logStream() << MSG::DEBUG << " ---> ROB Id = 0x" << MSG::hex << id
		    << std::setfill( '0' )
		    << " with Generic Status Code = 0x" << std::setw(4) << tmpstatus.generic()
		    << " and Specific Status Code = 0x" << std::setw(4) << tmpstatus.specific()
		    << MSG::dec
		    << " removed for L1 Id = " << currentLvl1ID << endmsg;
      }
    } else {
      // mask off the module ID for L2 and EF result for Run 1 data
      if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) &&
	   (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_LVL2) ) {
	 id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
	 if (!m_maskL2EFModuleID) {
	   ATH_MSG_ERROR("Inconsistent flag for masking L2/EF module IDs");
	   m_maskL2EFModuleID=true;
	 }
      } else if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) && 
		  (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_EVENT_FILTER) &&
		  (m_maskL2EFModuleID) ) {
	id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
      }
      m_online_robmap[id]= (*it_robf) ;
    }

    //* fill monitoring histogram for ROB generic status
    if ( ( m_hist_genericStatusForROB ) && ( it_robf->nstatus() != 0 ) ) {
      const uint32_t* it_status;
      it_robf->status(it_status);
      if ((*it_status) != 0) {
	scoped_lock_histogram lock;
	m_hist_genericStatusForROB->Fill(eformat::helper::SourceIdentifier(it_robf->source_id()).human_detector().c_str(),
					 m_map_GenericStatus[eformat::helper::Status(*it_status).generic()].c_str(),1.);
      }
    }

    //* fill monitoring histogram for ROB specific status
    if ( ( m_hist_specificStatusForROB ) && ( it_robf->nstatus() != 0 ) ) {
      const uint32_t* it_status;
      it_robf->status(it_status);
      if ((*it_status) != 0) {
	scoped_lock_histogram lock;
	std::bitset<16> specificBits(eformat::helper::Status(*it_status).specific());
	for (unsigned int index=0; index < 16; ++index) { 
	  if (specificBits[index]) m_hist_specificStatusForROB->Fill(eformat::helper::SourceIdentifier(it_robf->source_id()).human_detector().c_str(),
								     m_vec_SpecificStatus[index].c_str(),1.);
	}
      }
    }
  }
  
  if(logLevel() <= MSG::DEBUG) {
    logStream()<<MSG::DEBUG<< " ---> setNextEvent online for "<< name()<<endmsg; 
    logStream()<<MSG::DEBUG<< "      online running    = " << m_onlineRunning <<endmsg;
    logStream()<<MSG::DEBUG<< "      current LVL1 id   = " << currentLvl1ID <<endmsg;
    logStream()<<MSG::DEBUG<< "      # LVL1 ROBs       = " << result.size() <<endmsg;
    logStream()<<MSG::DEBUG<< "      size of ROB cache = " << m_online_robmap.size() <<endmsg;
    logStream()<<MSG::DEBUG<< dumpROBcache() <<endmsg; 
  }
  return; 
}

/** - add a new Raw event
    - rebuild the map
    - set flag for offline running
*/
void TrigROBDataProviderSvc::setNextEvent(const RawEvent* re) 
{ 
  // set the offline flag
  m_onlineRunning = false ;

 // set the event complete flag
  m_isEventComplete = true ;

  ROBDataProviderSvc::setNextEvent(re);
  // set flag for masking L2/EF module ID, this is only necessary for the separate L2 and EF systems from Run 1
  m_maskL2EFModuleID = ROBDataProviderSvc::m_maskL2EFModuleID; 
 
  if (m_simulateOnlineDataAccess.value()) {
    // now get L1 result
    std::vector<const ROBF*> l1_ROB_fragments;
    ROBDataProviderSvc::getROBData(m_l1_ROB_ids, l1_ROB_fragments);

    std::vector<ROBF> l1_result;
    l1_result.reserve(l1_ROB_fragments.size());

    for(std::vector<const ROBF*>::const_iterator it=l1_ROB_fragments.begin(); it != l1_ROB_fragments.end(); ++it) {
      l1_result.push_back(**it);
      if (eformat::helper::SourceIdentifier( (*it)->source_id() ).subdetector_id() == eformat::TDAQ_LVL2) m_maskL2EFModuleID = true;
    }
    setNextEvent(l1_result);
  }

  return ;
}

/** return ROBData for ROBID
 */ 
void TrigROBDataProviderSvc::getROBData(const std::vector<uint32_t>& robIds, std::vector<const ROBF*>& robFragments, std::string_view callerName) 
{

  //--------------------
  // set the caller name
  //--------------------
  setCallerName(callerName);

  //-------------------
  //--- offline running
  //-------------------
  if (!m_onlineRunning) {
    ROBDataProviderSvc::getROBData(robIds,robFragments);
  //------------------
  //--- online running
  //------------------
  } else {
    // ---------------------------------------------------------------------------------------------
    // make sure that all ROB prefetching information is taken into account before the ROB retrieval
    // ---------------------------------------------------------------------------------------------
    if (m_RobRequestInfo) {
      // add ROBs in the info object to the prefetching list
      addROBData(m_RobRequestInfo->requestScheduledRobIDs()); 
      m_RobRequestInfo->clearRequestScheduledRobIDs();
    }

    //--------------------
    // make unique ROB IDs
    //--------------------
    std::vector<uint32_t>::iterator remove_duplicate;
    std::vector<uint32_t> robIdsUnique(robIds);

    sort(robIdsUnique.begin(), robIdsUnique.end()); 
    remove_duplicate = unique(robIdsUnique.begin(), robIdsUnique.end()); 
    robIdsUnique.erase(remove_duplicate, robIdsUnique.end());

    //------------------------------------------------------------------
    // Replace the generic MET ROB ID with the full list of all MET ROBs
    //------------------------------------------------------------------
    // LAr MET ROBs
    uint32_t generic_LAr_MET_id = eformat::helper::SourceIdentifier(eformat::TDAQ_LAR_MET,m_genericLArMetModuleID.value()).code();
    std::vector<uint32_t>::iterator rob_LAr_Met_it = std::find(robIdsUnique.begin(), robIdsUnique.end(), generic_LAr_MET_id);
    if (rob_LAr_Met_it != robIdsUnique.end()) {
      robIdsUnique.erase(rob_LAr_Met_it);
      if (m_enabledLArMetROBs.value().size() != 0) robIdsUnique.insert(robIdsUnique.end(),m_enabledLArMetROBs.value().begin(),m_enabledLArMetROBs.value().end());
    }

    // Tile MET ROBs
    uint32_t generic_Tile_MET_id = eformat::helper::SourceIdentifier(eformat::TDAQ_TILE_MET,m_genericTileMetModuleID.value()).code();
    std::vector<uint32_t>::iterator rob_Tile_Met_it = std::find(robIdsUnique.begin(), robIdsUnique.end(), generic_Tile_MET_id);
    if (rob_Tile_Met_it != robIdsUnique.end()) {
      robIdsUnique.erase(rob_Tile_Met_it);
      if (m_enabledTileMetROBs.value().size() != 0) robIdsUnique.insert(robIdsUnique.end(),m_enabledTileMetROBs.value().begin(),m_enabledTileMetROBs.value().end());
    }

    //------------------------------------------------------------------
    // If a ROB/ROS mapping is used and a requested ROB is found on the
    // list of ROB Ids which are scheduled for retrieval then update
    // first the internal ROB cache with all scheduled ROBs in the same
    // ROS 
    //------------------------------------------------------------------
    if ( (m_useROSmappingForROBRetrieval.value()) && (m_Det_Robs_for_retrieval.size()!=0) ) {
      std::vector<uint32_t> robIdsForRetrieval, retrievedRobIds ;

      // create map ROS->vector<ROB Ids>
      std::map<int, std::vector<uint32_t> > ros_rob_map_for_retrieval;
      for (std::map<uint32_t,int>::const_iterator rob_ros_it=m_Det_Robs_for_retrieval.begin();rob_ros_it!=m_Det_Robs_for_retrieval.end();++rob_ros_it) {
	ros_rob_map_for_retrieval[ rob_ros_it->second ].push_back( rob_ros_it->first );
      }

      // find ROBs which are requested and which are on the list of scheduled ROBs
      // in case one ROB is found on the list all other scheduled ROBs on this list which are in the same ROS are retrieved
      for (std::vector<uint32_t>::const_iterator requested_rob_it=robIdsUnique.begin(); requested_rob_it!=robIdsUnique.end();++requested_rob_it) {
	std::map<uint32_t,int>::const_iterator rob_ros_it=m_Det_Robs_for_retrieval.find(*requested_rob_it);
	if ((rob_ros_it!=m_Det_Robs_for_retrieval.end()) &&  (ros_rob_map_for_retrieval[ rob_ros_it->second ].size()!=0)) {
	  // put all ROBs in the same ROS on the retrieval list
	  robIdsForRetrieval.insert(robIdsForRetrieval.end(),ros_rob_map_for_retrieval[ rob_ros_it->second ].begin(),ros_rob_map_for_retrieval[ rob_ros_it->second ].end());
	  // empty the retrieval list for this ROS, that it is not used multiple times
	  ros_rob_map_for_retrieval[ rob_ros_it->second ].clear();
	}
      }

      // detailed ROB monitoring
      //------------------------
      // Create a ROB monitoring collection and register it to StoreGate
      ROBDataMonitorCollection* p_robMonCollection(0); 
      if ((m_doDetailedROBMonitoring.value()) && (robIdsForRetrieval.size() != 0)) {
	if ( !(m_storeGateSvc->transientContains<ROBDataMonitorCollection>(m_ROBDataMonitorCollection_SG_Name.value())) ) {
	  p_robMonCollection = new ROBDataMonitorCollection;
	  if ( p_robMonCollection ) {
	    p_robMonCollection->reserve( 10 ) ;
	    if ( (m_storeGateSvc->record(p_robMonCollection, m_ROBDataMonitorCollection_SG_Name.value(), true)).isFailure() ) { 
	      logStream() << MSG::WARNING << " getROBData: Registering ROB Monitoring collection in StoreGate failed." << endmsg;
	      delete p_robMonCollection;
	      p_robMonCollection = 0;
	    }
	  }
	} else {
	  if ( m_storeGateSvc->retrieve(p_robMonCollection).isFailure() ) {
	    logStream() << MSG::WARNING << " getROBData: Retrieval of ROB Monitoring collection from StoreGate failed." << endmsg;
	    p_robMonCollection = 0;
	  }
	}
      }
  
      // create a new ROBDataMonitorStruct and fill it
      robmonitor::ROBDataMonitorStruct* p_robMonStruct(0);
      if ( p_robMonCollection ) {
	// initialize new ROBDataMonitorStruct
	p_robMonStruct = new robmonitor::ROBDataMonitorStruct(lvl1_id(), robIdsForRetrieval,  m_callerName);
      }

      // update internal ROB cache 
      retrievedRobIds.reserve( robIdsForRetrieval.size() );
      addROBDataToCache(robIdsForRetrieval, retrievedRobIds, p_robMonStruct);

      // remove successfully retrieved ROBs from the list of scheduled ROBs
      for (std::vector<uint32_t>::const_iterator rob_it = retrievedRobIds.begin(); rob_it!=retrievedRobIds.end(); ++rob_it) {
	m_Det_Robs_for_retrieval.erase( *rob_it );
      }

      // add the ROB monitoring structure to the collection
      if ( p_robMonCollection && p_robMonStruct )	p_robMonCollection->push_back( p_robMonStruct );
      //	std::cout <<(*p_robMonStruct)<<std::endl;

    } // end if (m_useROSmappingForROBRetrieval.value())

    //------------------------------------------------------------------
    // Return requested ROBs from internal cache
    //------------------------------------------------------------------
    for(std::vector<uint32_t>::const_iterator it = robIdsUnique.begin(); it != robIdsUnique.end(); ++it){
      uint32_t id = (*it); 

      // mask off the module ID for L2 and EF result for Run 1 data
      if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) &&
	   (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_LVL2) ) {
	 id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
	 if (!m_maskL2EFModuleID) {
	   ATH_MSG_ERROR("Inconsistent flag for masking L2/EF module IDs");
	   m_maskL2EFModuleID=true;
	 }
      } else if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) && 
		  (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_EVENT_FILTER) &&
		  (m_maskL2EFModuleID) ) {
	id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
      } 

      // exclude check on L1 ROBs
      if (std::find(m_l1_ROB_ids.begin(), m_l1_ROB_ids.end(), id)!=m_l1_ROB_ids.end()) {
	continue;
      }

      ONLINE_ROBMAP::iterator map_it = m_online_robmap.find(id) ;
      if(map_it != m_online_robmap.end()) {      
	robFragments.push_back( &((*map_it).second) ); 
      } else {

	if(logLevel() <= MSG::WARNING) {
	  logStream()<<MSG::WARNING<<" ---> getROBData: Failed to find ROB for id 0x"
		     <<MSG::hex<< id <<MSG::dec<<endmsg;
	  if (m_Det_Robs_for_retrieval.find(id)!=m_Det_Robs_for_retrieval.end()) {
	    logStream()<<MSG::WARNING<<"      getROBData: The ROB id 0x"
		       <<MSG::hex<< id <<MSG::dec<<" was put on the prefetching list, but never arrived on the processor."<<endmsg;
	  } else {
	    logStream()<<MSG::WARNING<<"      getROBData: The ROB id 0x"
		       <<MSG::hex<< id <<MSG::dec<<" was never put on the prefetching list."<<endmsg;
	  }

	  // caller name of this method
	  std::vector<IAlgorithm*> knownAlgs;
	  if ( m_algContextSvc ) {
	    knownAlgs = m_algContextSvc->algorithms();
	  }
	  logStream()<<MSG::WARNING<<"      getROBData was called by    : " <<  m_callerName << endmsg;
          // Print all known algorithms
	  if(logLevel() <= MSG::DEBUG) {
	    logStream()<<MSG::DEBUG<<"      getROBData known algorithms : \n" ;
	    int i=0;	  
	    for (std::vector<IAlgorithm*>::const_iterator itr=knownAlgs.begin(); itr!=knownAlgs.end(); ++itr) {
	      logStream() << std::setw(2) << i << " : " << (*itr)->name() << "\n";
	      ++i;
	    }
	    logStream()<<endmsg;
	  }

	  // Optionally provide caller backtrace
	  if (m_printStackTraceGetROBData.value()) {
	    static const int TRACE_DEPTH=64;
	    void * array[TRACE_DEPTH];
	    int nSize = backtrace(array, TRACE_DEPTH);
	    char ** symbols = backtrace_symbols(array, nSize);
	    logStream()<<MSG::WARNING<<"      getROBData: Printing stacktrace: Found " << nSize << " backtrace symbols.\n";
	    for (int i = 0; i < nSize; i++) {
	      if (i==1) { logStream() << "-->" << std::setw(2) << i << " : " << symbols[i] << "\n"; }
	      else { logStream() << std::setw(5) << i << " : " << symbols[i] << "\n"; }
	    }
	    logStream() << endmsg;
	    free(symbols);
	  }

          // Print contents of ROB cache
	  if(logLevel() <= MSG::VERBOSE) { logStream()<<MSG::VERBOSE<< dumpROBcache() <<endmsg; }
	} 
      }
    }
  }

  return ; 
}

/// Retrieve the whole event.
const RawEvent* TrigROBDataProviderSvc::getEvent(){
  //  if (m_onlineRunning) return 0;
  return ROBDataProviderSvc::getEvent(); 
}

/// Return vector with all ROBFragments stored in the cache 
void TrigROBDataProviderSvc::getAllROBData(std::vector<const ROBF*>& robFragments) 
{
  //------------------
  //--- online running
  //------------------
  if (m_onlineRunning) {
    for(ONLINE_ROBMAP::iterator it=m_online_robmap.begin(); it!=m_online_robmap.end(); ++it) {
      robFragments.push_back( &((*it).second) ); 
    } 
  }
  return ; 
}

// Dump ROB cache
std::string TrigROBDataProviderSvc::dumpROBcache() const {

  ONLINE_ROBMAP::const_iterator cache_it  = m_online_robmap.begin() ; 
  ONLINE_ROBMAP::const_iterator cache_end = m_online_robmap.end() ;
  int nrob = 0;

  std::ostringstream ost;
  ost << " --- Dump of ROB cache ids --- total size = "
      << m_online_robmap.size() <<"\n";  
  for(; cache_it!=cache_end; ++cache_it){
    ++nrob;
    ost <<" # = "<< std::setw(5) << nrob << " cache id = 0x" << std::hex << (*cache_it).first 
	<< " hex: source id = 0x" << std::hex << (*cache_it).second.source_id() 
	<< std::dec << "  decimal: source id = " << (*cache_it).second.source_id() << "\n";
  }
  std::string ret(ost.str());
  return ret;
}

/// Collect all data for an event from the ROS and put them into the cache
/// Return value: number of ROBs which were retrieved to complete event
int TrigROBDataProviderSvc::collectCompleteEventData(const std::string_view callerName) {

  // return if call was already issued
  if (m_isEventComplete) return 0;

  //--------------------
  // set the caller name
  //--------------------
  setCallerName(callerName);

  std::vector<uint32_t> robIdsForRetrieval, retrievedRobIds;
  robIdsForRetrieval.reserve(2048);

  struct timeval time_start;
  struct timeval time_stop;
  if ( m_doMonitoring || m_doDetailedROBMonitoring.value() ) gettimeofday(&time_start, 0);

  const EventContext context{ Gaudi::Hive::currentContext() };
  const ROBMAP& robmap = m_eventsCache.get(context)->robmap;

  // Get ROB Fragments to complete event from offline ROBDataProviderSvc
  for(ROBDataProviderSvc::ROBMAP::const_iterator offline_rob_map_it=robmap.begin();
      offline_rob_map_it != robmap.end(); ++offline_rob_map_it) {
    uint32_t id = (*offline_rob_map_it).first; 
    ONLINE_ROBMAP::iterator map_it = m_online_robmap.find(id); 
    if (map_it == m_online_robmap.end()) robIdsForRetrieval.push_back(id); 
  }

  if ( m_doMonitoring || m_doDetailedROBMonitoring.value() ) gettimeofday(&time_stop, 0);

  // detailed ROB monitoring
  //------------------------
  // Create a ROB monitoring collection and register it to StoreGate
  ROBDataMonitorCollection* p_robMonCollection(0);
  if ( m_doDetailedROBMonitoring.value() ) {
    if ( !(m_storeGateSvc->transientContains<ROBDataMonitorCollection>(m_ROBDataMonitorCollection_SG_Name.value())) ) {
      p_robMonCollection = new ROBDataMonitorCollection;
      if ( p_robMonCollection ) {
        p_robMonCollection->reserve( 10 ) ;
        if ( (m_storeGateSvc->record(p_robMonCollection, m_ROBDataMonitorCollection_SG_Name.value(), true)).isFailure() ) {
          logStream() << MSG::WARNING << " Registering ROB Monitoring collection in StoreGate failed." << endmsg;
          delete p_robMonCollection;
          p_robMonCollection = 0;
        }
      }
    } else {
      if ( m_storeGateSvc->retrieve(p_robMonCollection).isFailure() ) {
        logStream() << MSG::WARNING << " Retrieval of ROB Monitoring collection from StoreGate failed." << endmsg;
        p_robMonCollection = 0;
      }
    }
  }

  // create a new ROBDataMonitorStruct and fill it
  robmonitor::ROBDataMonitorStruct* p_robMonStruct(0);
  if ( p_robMonCollection ) {
    // initialize new ROBDataMonitorStruct
    p_robMonStruct = new robmonitor::ROBDataMonitorStruct( lvl1_id(), robIdsForRetrieval,  m_callerName);
  }

  if ( m_doMonitoring || p_robMonStruct ) {
    int secs = 0 ;
    if (time_stop.tv_sec >= time_start.tv_sec)
      secs = time_stop.tv_sec - time_start.tv_sec;

    int usecs = time_stop.tv_usec - time_start.tv_usec;
    float mtime = static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;

    //* timing histogram
    if (m_hist_timeROBretrieval) {
      scoped_lock_histogram lock;
      m_hist_timeROBretrieval->Fill(mtime);
      m_hist_timeROBretrieval->LabelsDeflate("X");
    }
    //* number of received ROBs
    if ( m_hist_receivedROBsPerCall ) {
      scoped_lock_histogram lock;
      m_hist_receivedROBsPerCall->Fill(retrievedRobIds.size());
      m_hist_receivedROBsPerCall->LabelsDeflate("X");
    }

    //* detailed monitoring
    if ( p_robMonStruct ) {
      p_robMonStruct->start_time_of_ROB_request    = time_start;
      p_robMonStruct->end_time_of_ROB_request      = time_stop;
    }
  }

  // update internal ROB cache 
  retrievedRobIds.reserve( robIdsForRetrieval.size() );
  addROBDataToCache(robIdsForRetrieval, retrievedRobIds, p_robMonStruct);

  // clear list of scheduled ROBs, all ROBs are available now, no new retrieval
  m_Det_Robs_for_retrieval.clear();

  // add the ROB monitoring structure to the collection
  if ( p_robMonCollection && p_robMonStruct )     p_robMonCollection->push_back( p_robMonStruct );

  if (logLevel() <= MSG::DEBUG) {
    logStream() << MSG::DEBUG
        << " ---> collectCompleteEventData: The following ROB Ids were received from DataCollector : \n"
		<< "      Lvl1 id                                             = " << lvl1_id() << "\n"
        << "      Number of actually received ROB Ids                 = " << retrievedRobIds.size()
        << endmsg;
  }
  if (logLevel() <= MSG::VERBOSE) {
    std::ostringstream ost;
    unsigned int rob_counter = 1;
    for (std::vector<uint32_t>::const_iterator rob_it = retrievedRobIds.begin(); rob_it != retrievedRobIds.end(); ++rob_it,++rob_counter)
      ost << "       # = "<< std::setw(5) << rob_counter << " ROB id = 0x" << std::hex << *rob_it << std::dec << "\n" ;
    logStream() << MSG::VERBOSE << "\n" << ost.str() << endmsg;
  }
  
  // update event complete flag
  m_isEventComplete = true;

  return retrievedRobIds.size();
} // end int collectCompleteEventData(...)

int TrigROBDataProviderSvc::collectCompleteEventData(const EventContext&, 
                                                     const std::string_view callerName)
{
  return collectCompleteEventData (callerName);
}

/// set the name of the program which uses the ROBDataProviderSvc
void TrigROBDataProviderSvc::setCallerName(std::string_view callerName)
{
 // caller name of this method
  std::string caller_name("UNKNOWN");
  if (callerName != caller_name) { 
    caller_name = callerName;
  }else{
    IAlgorithm* alg(0);
    if ( m_algContextSvc ) {
      alg = m_algContextSvc->currentAlg();
      caller_name = (alg ? alg->name() : "<NONE>");
    }
  }
  m_callerName=caller_name; 
  //  logStream() << MSG::DEBUG << "TrigROBDataProviderSvc::setCallerName    : " << m_callerName <<endmsg;
  return;
}


// handler for BeginRun
void TrigROBDataProviderSvc::handle(const Incident& incident) {
  if (incident.type()!="BeginRun") return;
  if(logLevel() <= MSG::DEBUG)
    logStream() <<MSG::DEBUG << "In BeginRun incident." << endmsg;
  
  // if detailed ROB monitoring is requested, check if the AlgContextSvc is running, 
  // if yes use it to obtain the calling algorithm name
  if ( m_doDetailedROBMonitoring.value() ) {
    if ( service("AlgContextSvc", m_algContextSvc, /*createIf=*/ false).isFailure() ) {
      logStream() << MSG::ERROR << "Error retrieving AlgContextSvc."  
		  << "Calling algorithm name not available in detailed ROB monitoring" << endmsg;
      m_algContextSvc=0;
    }
  }
  
  // define histograms if monitoring is requested
  if ( !m_doMonitoring.value() ) return;
      
  // find histogramming service
  ServiceHandle<ITHistSvc> rootHistSvc("THistSvc", name());
  if ((rootHistSvc.retrieve()).isFailure()) {
    logStream() << MSG::ERROR << "Unable to locate THistSvc" << endmsg;
    rootHistSvc.release().ignore();
    return;
  }
      
  // *-- booking path
  std::string path = std::string("/EXPERT/")+name()+"/";

  // *-- number of bins for sub detector plots (55 SubDet max.)
  uint32_t n_bins_partEBSubDet = eformat::helper::SubDetectorDictionary.size();

  // *-- number of requested ROBs per call
  m_hist_requestedROBsPerCall = new TH1F (m_histProp_requestedROBsPerCall.value().title().c_str(),
					  (m_histProp_requestedROBsPerCall.value().title()+";number of ROBs").c_str(), 
					  m_histProp_requestedROBsPerCall.value().bins(), 
					  m_histProp_requestedROBsPerCall.value().lowEdge(), 
					  m_histProp_requestedROBsPerCall.value().highEdge());
  if (m_hist_requestedROBsPerCall) {
    CAN_REBIN(m_hist_requestedROBsPerCall);
    if( rootHistSvc->regHist(path + m_hist_requestedROBsPerCall->GetName(), m_hist_requestedROBsPerCall).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_requestedROBsPerCall->GetName() << endmsg;
    }
  }

  // *-- number of received ROBs per call
  m_hist_receivedROBsPerCall  = new TH1F (m_histProp_receivedROBsPerCall.value().title().c_str(),
					  (m_histProp_receivedROBsPerCall.value().title()+";number of ROBs").c_str(), 
					  m_histProp_receivedROBsPerCall.value().bins(), 
					  m_histProp_receivedROBsPerCall.value().lowEdge(), 
					  m_histProp_receivedROBsPerCall.value().highEdge());
  if (m_hist_receivedROBsPerCall) {
    CAN_REBIN(m_hist_receivedROBsPerCall);
    if( rootHistSvc->regHist(path + m_hist_receivedROBsPerCall->GetName(), m_hist_receivedROBsPerCall).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_receivedROBsPerCall->GetName() << endmsg;
    }
  }

  // *-- timing of ROB retrieval
  m_hist_timeROBretrieval     = new TH1F (m_histProp_timeROBretrieval.value().title().c_str(),
					  (m_histProp_timeROBretrieval.value().title()+";time [ms]").c_str(), 
					  m_histProp_timeROBretrieval.value().bins(), 
					  m_histProp_timeROBretrieval.value().lowEdge(), 
					  m_histProp_timeROBretrieval.value().highEdge());
  if (m_hist_timeROBretrieval) {
    CAN_REBIN(m_hist_timeROBretrieval);
    if( rootHistSvc->regHist(path + m_hist_timeROBretrieval->GetName(), m_hist_timeROBretrieval).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_timeROBretrieval->GetName() << endmsg;
    }
  }

 // *-- number of retrievd ROBs per Algo
  m_hist_retrievedROBsPerAlgo  = new TH1F (m_histProp_retrievedROBsPerAlgo.value().title().c_str(),
					  (m_histProp_retrievedROBsPerAlgo.value().title()+";number of retrieved ROBs").c_str(), 
					  m_histProp_retrievedROBsPerAlgo.value().bins(), 
					  m_histProp_retrievedROBsPerAlgo.value().lowEdge(), 
					  m_histProp_retrievedROBsPerAlgo.value().highEdge());
  if (m_hist_retrievedROBsPerAlgo) {
    CAN_REBIN(m_hist_retrievedROBsPerAlgo);
    if( rootHistSvc->regHist(path + m_hist_retrievedROBsPerAlgo->GetName(), m_hist_retrievedROBsPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_retrievedROBsPerAlgo->GetName() << endmsg;
    }
  }


 // *-- number of ROS requests

  m_hist_ROSRequest  = new TH2F (m_histProp_ROSRequest.value().title().c_str(),
				 (m_histProp_ROSRequest.value().title()+";ROS id").c_str(), 
				 m_histProp_ROSRequest.value().bins(), 
				 m_histProp_ROSRequest.value().lowEdge(), 
				 m_histProp_ROSRequest.value().highEdge(),
				 n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet); 
  if (m_hist_ROSRequest) {
    uint32_t n_tmp_bin = 1;
    for (eformat::helper::EnumClass<eformat::SubDetector>::const_iterator it_sub=eformat::helper::SubDetectorDictionary.begin();
	 it_sub != eformat::helper::SubDetectorDictionary.end(); it_sub++ ) {
      m_hist_ROSRequest->GetYaxis()->SetBinLabel( n_tmp_bin, (it_sub->second).c_str() );
      n_tmp_bin++;
    }

    CAN_REBIN(m_hist_ROSRequest);
    if( rootHistSvc->regHist(path + m_hist_ROSRequest->GetName(), m_hist_ROSRequest).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_ROSRequest->GetName() << endmsg;
    }
  }


  // *-- Generic Status for ROBs per sub detector
  m_hist_genericStatusForROB = new TH2F ("GenericStatusForROBsFromSubDetectors",
					 "GenericStatusForROBsFromSubDetectors;;",
					 n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet,
					 m_map_GenericStatus.size(),0., (float) m_map_GenericStatus.size());
  if (m_hist_genericStatusForROB) {
    uint32_t n_tmp_bin = 1;
    for (eformat::helper::EnumClass<eformat::SubDetector>::const_iterator it_sub=eformat::helper::SubDetectorDictionary.begin();
	 it_sub != eformat::helper::SubDetectorDictionary.end(); it_sub++ ) {
      m_hist_genericStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, (it_sub->second).c_str() );
      n_tmp_bin++;
    }

    n_tmp_bin = 1;
    for (std::map<eformat::GenericStatus, std::string>::const_iterator it = m_map_GenericStatus.begin();it != m_map_GenericStatus.end();++it) {
      m_hist_genericStatusForROB->GetYaxis()->SetBinLabel( n_tmp_bin, (*it).second.c_str() );
      n_tmp_bin++;
    }
    if( rootHistSvc->regHist(path + m_hist_genericStatusForROB->GetName(), m_hist_genericStatusForROB).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_genericStatusForROB->GetName() << endmsg;
    }
  }

  // *-- Specific Status Bits for ROBs per sub detector
  m_hist_specificStatusForROB = new TH2F ("SpecificStatusBitsForROBsFromSubDetectors",
					  "SpecificStatusBitsForROBsFromSubDetectors;;",
					  n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet,
					  m_vec_SpecificStatus.size(),0., (float) m_vec_SpecificStatus.size());
  if (m_hist_specificStatusForROB) {
    uint32_t n_tmp_bin = 1;
    for (eformat::helper::EnumClass<eformat::SubDetector>::const_iterator it_sub=eformat::helper::SubDetectorDictionary.begin();
	 it_sub != eformat::helper::SubDetectorDictionary.end(); it_sub++ ) {
      m_hist_specificStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, (it_sub->second).c_str() );
      n_tmp_bin++;
    }

    n_tmp_bin = 1;
    for (std::vector<std::string>::const_iterator it = m_vec_SpecificStatus.begin();it != m_vec_SpecificStatus.end();++it) {
      m_hist_specificStatusForROB->GetYaxis()->SetBinLabel( n_tmp_bin, (*it).c_str() );
      n_tmp_bin++;
    }
    if( rootHistSvc->regHist(path + m_hist_specificStatusForROB->GetName(), m_hist_specificStatusForROB).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_specificStatusForROB->GetName() << endmsg;
    }
  }

  // release histogramming service
  rootHistSvc.release().ignore();
}

// helper function to retrieve ROB fragments over the network and to add them to the cache 
void TrigROBDataProviderSvc::addROBDataToCache(std::vector<uint32_t>& robIdsForRetrieval, 
					       std::vector<uint32_t>& retrievedRobIds,
					       robmonitor::ROBDataMonitorStruct* p_robMonStruct) {

  struct timeval time_start;
  struct timeval time_stop;

  retrievedRobIds.clear();

  if(logLevel() <= MSG::DEBUG)
    logStream() << MSG::DEBUG << " ---> addROBDataToCache for "<<m_callerName<<": Number of ROB Ids requested for retrieval : " << robIdsForRetrieval.size() 
		<< ", Lvl1 id = " << lvl1_id() << endmsg;

  // return if no ROBs are requested
  if (robIdsForRetrieval.size() == 0) return;

  std::vector<uint32_t> vRobIds, vMETRobIds;
  vRobIds.reserve( robIdsForRetrieval.size() );



  // separate MET and detector ROBs if requested
  if (m_separateMETandDetROBRetrieval.value()) {
    vMETRobIds.reserve( robIdsForRetrieval.size() );
    for (std::vector<uint32_t>::const_iterator rob_it=robIdsForRetrieval.begin(); rob_it!=robIdsForRetrieval.end(); ++rob_it) {
      if ( (eformat::helper::SourceIdentifier(*rob_it).subdetector_id() == eformat::TDAQ_LAR_MET) || 
	   (eformat::helper::SourceIdentifier(*rob_it).subdetector_id() == eformat::TDAQ_TILE_MET) ) {
	vMETRobIds.push_back( *rob_it ) ;
      } else {
	vRobIds.push_back( *rob_it );
      }
    }
  } else {
    vRobIds=robIdsForRetrieval;
  }

  std::vector<const ROBF*> vRobFragments ;
  vRobFragments.reserve( vRobIds.size() ) ;

  // Get ROB Fragments with DataCollector
  if ( m_doMonitoring || p_robMonStruct ) gettimeofday(&time_start, 0);
  ROBDataProviderSvc::getROBData(vRobIds,vRobFragments);

  // Check that received ROB IDs are on the list of requested ROB IDs
  for (std::vector<const ROBF*>::const_iterator it_robf = vRobFragments.begin();
       it_robf != vRobFragments.end(); ++it_robf) {
    uint32_t id = (*it_robf)->source_id() ;
    retrievedRobIds.push_back(id);
    if ( (eformat::helper::SourceIdentifier(id).subdetector_id() != eformat::TDAQ_LVL2) &&
	 (eformat::helper::SourceIdentifier(id).subdetector_id() != eformat::TDAQ_EVENT_FILTER)
	 ) {
      if (std::find(vRobIds.begin(), vRobIds.end(), id) == vRobIds.end()) {
	logStream() << MSG::ERROR << " ---> addROBDataToCache for "<<m_callerName<<": Received ROB Id : 0x" << MSG::hex << id << MSG::dec
		    <<" from ROS is not on the list of requested ROB Ids."<< endmsg;
      }
    }
  }
  
  // Do a separate data collect call for MET ROBs if required
  if ( (m_separateMETandDetROBRetrieval.value()) && (vMETRobIds.size() != 0) ) {
    std::vector<const ROBF*> vMETRobFragments ;
    vMETRobFragments.reserve( vMETRobIds.size() ) ;
    // retrieve MET ROBs
    ROBDataProviderSvc::getROBData(vMETRobIds,vMETRobFragments);

    // Check that received ROB IDs are on the list of requested ROB IDs
    for (std::vector<const ROBF*>::const_iterator it_robf = vMETRobFragments.begin();
	 it_robf != vMETRobFragments.end(); ++it_robf) {
      uint32_t id = (*it_robf)->source_id() ;
      retrievedRobIds.push_back(id);
      if ( (eformat::helper::SourceIdentifier(id).subdetector_id() != eformat::TDAQ_LVL2) &&
	   (eformat::helper::SourceIdentifier(id).subdetector_id() != eformat::TDAQ_EVENT_FILTER)
	   ) {
	if (std::find(vMETRobIds.begin(), vMETRobIds.end(), id) == vMETRobIds.end()) {
	  logStream() << MSG::ERROR << " ---> addROBDataToCache for "<<m_callerName<<": Received MET ROB Id : 0x" << MSG::hex << id << MSG::dec
		      <<" from ROS is not on the list of requested ROB Ids."<< endmsg;
	}
      }
    }

    // add MET ROBs to Det ROBs
    vRobFragments.insert( vRobFragments.end(), vMETRobFragments.begin(), vMETRobFragments.end() );
  }

  if(logLevel() <= MSG::DEBUG) {
    std::ostringstream ost;
    unsigned int rob_counter = 1;
    for (std::vector<uint32_t>::const_iterator rob_it=retrievedRobIds.begin(); rob_it!=retrievedRobIds.end(); ++rob_it,++rob_counter)
      ost << "       # = "<< std::setw(5) << rob_counter << " ROB id = 0x" << std::hex << (*rob_it) << std::dec << "\n" ; 
    logStream() << MSG::DEBUG 
		<< " ---> addROBDataToCache for "<<m_callerName<<": The following ROB Ids were received from DataCollector : \n"
		<< "      Lvl1 id                                             = " << lvl1_id() << "\n"
                << "      Number of detector ROB Ids requested for retrieval  = " << vRobIds.size() << "\n"
                << "      Number of MET ROB Ids requested for retrieval       = " << vMETRobIds.size() << "\n"
                << "      Number of actually received ROB Ids                 = " << retrievedRobIds.size() << "\n"
		<< ost.str()
		<< endmsg;
  }

  if ( m_doMonitoring || p_robMonStruct ) {
    gettimeofday(&time_stop, 0);
    int secs = 0 ;
    if (time_stop.tv_sec >= time_start.tv_sec)
      secs = time_stop.tv_sec - time_start.tv_sec;
      
    int usecs = time_stop.tv_usec - time_start.tv_usec;
    float mtime = static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;
	
    //* timing histogram
    if (m_hist_timeROBretrieval) {
      scoped_lock_histogram lock;
      m_hist_timeROBretrieval->Fill(mtime);
      m_hist_timeROBretrieval->LabelsDeflate("X");
    }

    //* detailed monitoring
    if ( p_robMonStruct ) {
      p_robMonStruct->start_time_of_ROB_request    = time_start;
      p_robMonStruct->end_time_of_ROB_request      = time_stop;
    }
  }

  //* histograms for number of requested/received ROBs
  if ( m_hist_requestedROBsPerCall ) {
    scoped_lock_histogram lock;
    m_hist_requestedROBsPerCall->Fill(robIdsForRetrieval.size());
    m_hist_requestedROBsPerCall->LabelsDeflate("X");
  }
  if ( m_hist_receivedROBsPerCall ) {
    scoped_lock_histogram lock;
    m_hist_receivedROBsPerCall->Fill(vRobFragments.size());
    m_hist_receivedROBsPerCall->LabelsDeflate("X");
  }

    //* fill monitoring histogram for ROB requests
    if ( m_hist_retrievedROBsPerAlgo )  {     
      scoped_lock_histogram lock;
      m_hist_retrievedROBsPerAlgo->Fill(m_callerName.c_str(), vRobFragments.size());
      m_hist_retrievedROBsPerAlgo->LabelsDeflate("X");
    }


  // add ROBs to cache
  std::vector<const ROBF*>::const_iterator it_robf     = vRobFragments.begin();
  std::vector<const ROBF*>::const_iterator it_robf_end = vRobFragments.end();
  std::vector <uint32_t> ROSRequest;
  for(; it_robf!=it_robf_end; ++it_robf) {
    uint32_t id = (*it_robf)->source_id() ;
    if (((*it_robf)->rod_ndata() == 0) && (m_removeEmptyROB)) { 
      if(logLevel() <= MSG::DEBUG) { 
	logStream() << MSG::DEBUG << " ---> addROBDataToCache for "<<m_callerName<<": Empty ROB Id = 0x" << MSG::hex << id << MSG::dec
		    << " removed for L1 Id = " << lvl1_id() << endmsg;
      }
    } else if ( ROBDataProviderSvc::filterRobWithStatus(&*(*it_robf)) ) {
      if ((logLevel() <= MSG::DEBUG) && ((*it_robf)->nstatus() > 0)) {
	const uint32_t* it_status;
	(*it_robf)->status(it_status);
	eformat::helper::Status tmpstatus( (*it_status) ) ;
	logStream() << MSG::DEBUG << " ---> addROBDataToCache for "<<m_callerName<<": ROB Id = 0x" << MSG::hex << id
		    << std::setfill( '0' )
		    << " with Generic Status Code = 0x" << std::setw(4) << tmpstatus.generic()
		    << " and Specific Status Code = 0x" << std::setw(4) << tmpstatus.specific()
		    << MSG::dec
		    << " removed for L1 Id = " <<lvl1_id() << endmsg;
      }
    } else {
      // mask off the module ID for L2 and EF result for Run 1 data
      if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) &&
	   (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_LVL2) ) {
	 id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
	 if (!m_maskL2EFModuleID) {
	   ATH_MSG_ERROR("Inconsistent flag for masking L2/EF module IDs");
	   m_maskL2EFModuleID=true;
	 }
      } else if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) && 
		  (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_EVENT_FILTER) &&
		  (m_maskL2EFModuleID) ) {
	id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
      }
      m_online_robmap[id]= (**it_robf) ;
    }
    
    //* fill monitoring histogram for ROB generic status
    if ( ( m_hist_genericStatusForROB ) && ( (*it_robf)->nstatus() != 0 ) ) {
      const uint32_t* it_status;
      (*it_robf)->status(it_status);
      if ((*it_status) != 0) {
	scoped_lock_histogram lock;
	m_hist_genericStatusForROB->Fill(eformat::helper::SourceIdentifier((*it_robf)->source_id()).human_detector().c_str(),
					 m_map_GenericStatus[eformat::helper::Status(*it_status).generic()].c_str(),1.);
      }
    }
  
 

    //* fill monitoring histogram for ROB specific status
    if ( ( m_hist_specificStatusForROB ) && ( (*it_robf)->nstatus() != 0 ) ) {
      const uint32_t* it_status;
      (*it_robf)->status(it_status);
      if ((*it_status) != 0) {
	scoped_lock_histogram lock;
	std::bitset<16> specificBits(eformat::helper::Status(*it_status).specific());
	for (unsigned int index=0; index < 16; ++index) { 
	  if (specificBits[index]) m_hist_specificStatusForROB->Fill(eformat::helper::SourceIdentifier((*it_robf)->source_id()).human_detector().c_str(),
								     m_vec_SpecificStatus[index].c_str(),1.);
	}
      }
    }
      
    //* detailed monitoring
    if ( p_robMonStruct ) {
      (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::RETRIEVED;
      (p_robMonStruct->requested_ROBs)[id].rob_size    = (*it_robf)->fragment_size_word();
      if ( (*it_robf)->nstatus() != 0 ) {
	const uint32_t* it_status;
	(*it_robf)->status(it_status);
	for (uint32_t k=0; k < (*it_robf)->nstatus(); k++) {
	  (p_robMonStruct->requested_ROBs)[id].rob_status_words.push_back( *(it_status+k) );
	}
      }
    } // end detailed monitoring


    // ROS requests
    std::map<uint32_t,int>::const_iterator it=m_Det_Robs_for_retrieval.find(id);
    if (it != m_Det_Robs_for_retrieval.end()){
      uint32_t ROSid=(*it).second;
      if (find(ROSRequest.begin(), ROSRequest.end(),ROSid) == ROSRequest.end()){// new ROS id
	if(m_hist_ROSRequest){
	  scoped_lock_histogram lock;
	  m_hist_ROSRequest->Fill( m_callerName.c_str(), eformat::helper::SourceIdentifier((*it_robf)->source_id()).human_detector().c_str(),1.);
	  m_hist_ROSRequest->LabelsDeflate("X");
	  logStream() << MSG::INFO << " ---> addROBDataToCache for "<<m_callerName<<": request to ROS 0x"<<std::hex<<ROSid<<std::dec <<" " <<eformat::helper::SourceIdentifier((*it_robf)->source_id()).human_detector().c_str()<< std::endl;
	}
	ROSRequest.push_back(ROSid);
      }
    }
  }   // end loop over retrieved fragments
  return;
} // end void addROBDataToCache(...)
