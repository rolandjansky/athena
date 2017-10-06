/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files.
#include "TrigServices/HltROBDataProviderSvc.h"
#include "TrigMonitorBase/TrigLockedHist.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IAlgorithm.h"
#include "hltinterface/DataCollector.h"
#include "eformat/Version.h"

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

namespace HltROBDataProviderConstants {
  // reserve a number of ROB monitor collections
  static const int Number_of_Rob_Monitor_Structs = 10;
  // number of ROBs in an event, used to reserve space in an array
  static const int Max_Number_Of_ROBs = 2000;
}

// Constructor.
HltROBDataProviderSvc::HltROBDataProviderSvc(const std::string& name, ISvcLocator* svcloc)
:ROBDataProviderSvc(name,svcloc),
 m_storeGateSvc( "StoreGateSvc", name ),
 m_algContextSvc(0),
 m_onlineRunning(false),
 m_removeEmptyROB(false),
 m_isEventComplete(false),
 m_callerName("UNKNOWN"),
 m_histProp_requestedROBsPerCall(Gaudi::Histo1DDef("RequestedROBsPerCall" ,0,300,50)),
 m_histProp_receivedROBsPerCall(Gaudi::Histo1DDef("ReceivedROBsPerCall" ,0,300,50)),
 m_histProp_timeROBretrieval(Gaudi::Histo1DDef("TimeForROBretrieval" ,0.,500.,50)),
 m_hist_requestedROBsPerCall(0),
 m_hist_receivedROBsPerCall(0),
 m_hist_timeROBretrieval(0),
 m_hist_genericStatusForROB(0),
 m_hist_specificStatusForROB(0)
{
  declareProperty("ignoreROB", m_ignoreROB,"List of ROBs to ignore for retrieval");
  declareProperty("enabledROBs", m_enabledROBs,"List of enabled detector ROBs");
  declareProperty("LArMetROBs", m_enabledLArMetROBs,"List of enabled LAr MET ROBs");
  declareProperty("TileMetROBs", m_enabledTileMetROBs,"List of enabled Tile MET ROBs");
  declareProperty("readROBfromOKS", m_readROBfromOKS=true,"Read enabled ROBs from OKS");
  declareProperty("doMonitoring", m_doMonitoring=false,"Enable histograms");
  declareProperty("doDetailedROBMonitoring", m_doDetailedROBMonitoring=false,"Produce ROB cost data");
  declareProperty("ROBDataMonitorCollectionSGName", m_ROBDataMonitorCollection_SG_Name="ROBDataMonitorCollection","Name of cost monitoring collection in SG");
  declareProperty("HistRequestedROBsPerCall", m_histProp_requestedROBsPerCall,"Number of ROBs requested");
  declareProperty("HistReceivedROBsPerCall", m_histProp_receivedROBsPerCall,"Number of ROBs received");
  declareProperty("HistTimeROBretrieval", m_histProp_timeROBretrieval,"Timing for ROB retrieval");
  declareProperty("ModuleIDGenericLArMetROB", m_genericLArMetModuleID=0xffff,"Generic module id for LAr MET ROB retrieval");  
  declareProperty("ModuleIDGenericTileMetROB", m_genericTileMetModuleID=0xffff,"Generic module id for Tile MET ROB retrieval");
  declareProperty("SeparateMETandDetROBRetrieval", m_separateMETandDetROBRetrieval=true,"Separate retrieval of MET and detector ROBs");

  // fill map with generic status codes
  m_map_GenericStatus[eformat::UNCLASSIFIED]      = "UNCLASSIFIED"; 
  m_map_GenericStatus[eformat::BCID_CHECK_FAIL]   = "BCID_CHECK_FAIL"; 
  m_map_GenericStatus[eformat::LVL1ID_CHECK_FAIL] = "LVL1ID_CHECK_FAIL"; 
  m_map_GenericStatus[eformat::TIMEOUT]           = "TIMEOUT"; 
  m_map_GenericStatus[eformat::DATA_CORRUPTION]   = "DATA_CORRUPTION"; 
  m_map_GenericStatus[eformat::INTERNAL_OVERFLOW] = "INTERNAL_OVERFLOW"; 
  m_map_GenericStatus[eformat::DUMMY_FRAGMENT]    = "DUMMY_FRAGMENT"; 

  // fill vector with specific status codes
  m_vec_SpecificStatus.reserve(16);
  m_vec_SpecificStatus.push_back("TRIGGER_TYPE_SYNC_ERROR/DATAFLOW_DUMMY"); 
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
  m_vec_SpecificStatus.push_back("ROBIN_DISCARD_MODE"); 
}

// Destructor.
HltROBDataProviderSvc::~HltROBDataProviderSvc()
{
}

// Initialization 
StatusCode HltROBDataProviderSvc::initialize()
{
  StatusCode sc = ROBDataProviderSvc::initialize();

  ATH_MSG_INFO(" ---> HltROBDataProviderSvc = " << name() << " initialize "
               << " - package version " << PACKAGE_VERSION);

  // get Property filterEmptyROB from base class
  if ( !sc.isSuccess() ) {
    ATH_MSG_ERROR(" ROBDataProviderSvc::initialize() failed.");
    return sc;
  } else {
    BooleanProperty filterEmptyROB;
    filterEmptyROB.setName("filterEmptyROB");
    if (filterEmptyROB.assign(getProperty("filterEmptyROB"))) {
      m_removeEmptyROB = filterEmptyROB.value() ;
      ATH_MSG_INFO(" ---> getProperty('filterEmptyROB')       = " << filterEmptyROB);
    } else {
      ATH_MSG_WARNING(" ROBDataProviderSvc::getProperty('filterEmptyROB') failed.");
    }
  }

  // get the list of enabled ROBs from OKS
  bool robOKSconfigFound = false;
  bool robLArMetOKSconfigFound = false;
  bool robTileMetOKSconfigFound = false;

  if ( m_readROBfromOKS.value() ) {
    ServiceHandle<IJobOptionsSvc> p_jobOptionsSvc("JobOptionsSvc", name());
    if ((p_jobOptionsSvc.retrieve()).isFailure()) {
      ATH_MSG_ERROR("Could not find JobOptionsSvc");
    } else {
      const std::vector<const Property*>* dataFlowProps = p_jobOptionsSvc->getProperties("DataFlowConfig");
      if(!dataFlowProps)
        ATH_MSG_ERROR("Could not find DataFlowConfig properties");
      else
      {
        for ( const Property* cur : *dataFlowProps ) {
          // the enabled ROB list is found
          if ( cur->name() == "DF_Enabled_ROB_IDs" ) {
            if (m_enabledROBs.assign(*cur)) {
              robOKSconfigFound = true;
              ATH_MSG_INFO(" ---> Read from OKS                       = " << m_enabledROBs.value().size() << " enabled ROB IDs.");
            } else {
              ATH_MSG_WARNING(" Could not set Property 'enabledROBs' from OKS.");
            }
          }

          // the LAr MET ROB list is found
          if ( cur->name() == "DF_LAr_MET_ROB_IDs" ) {
            if (m_enabledLArMetROBs.assign(*cur)) {
              robLArMetOKSconfigFound = true;
              ATH_MSG_INFO(" ---> Read from OKS                       = " << m_enabledLArMetROBs.value().size() << " LAr MET ROB IDs.");
            } else {
              ATH_MSG_WARNING(" Could not set Property 'LArMetROBs' from OKS.");
            }
          }

          // the Tile MET ROB list is found
          if ( cur->name() == "DF_Tile_MET_ROB_IDs" ) {
            if (m_enabledTileMetROBs.assign(*cur)) {
              robTileMetOKSconfigFound = true;
              ATH_MSG_INFO(" ---> Read from OKS                       = " << m_enabledTileMetROBs.value().size() << " Tile MET ROB IDs.");
            } else {
              ATH_MSG_WARNING(" Could not set Property 'TileMetROBs' from OKS.");
            }
          }
        }
      }
      p_jobOptionsSvc.release().ignore();
    }
  }

  ATH_MSG_INFO(" ---> HltROBDataProviderSvc               = " << name() << " special properties <---");
  ATH_MSG_INFO(" ---> Filter out empty ROB fragments      = " << m_removeEmptyROB);
  ATH_MSG_INFO(" ---> Fill monitoring histograms          = " << m_doMonitoring);
  ATH_MSG_INFO("        Hist:RequestedROBsPerCall         = " << m_histProp_requestedROBsPerCall);
  ATH_MSG_INFO("        Hist:ReceivedROBsPerCall          = " << m_histProp_receivedROBsPerCall);
  ATH_MSG_INFO("        Hist:TimeROBretrieval             = " << m_histProp_timeROBretrieval);
  ATH_MSG_INFO(" ---> Do detailed ROB monitoring          = " << m_doDetailedROBMonitoring);
  ATH_MSG_INFO(" ---> SG name for ROB monitoring collect. = " << m_ROBDataMonitorCollection_SG_Name);
  ATH_MSG_INFO(" ---> Read list of enabled ROBs from OKS  = " << m_readROBfromOKS);
  if (m_enabledROBs.value().size() == 0) {
    ATH_MSG_INFO(" ---> The list of enabled ROBs has size   = 0. No check will be performed ");
  } else {
    if (m_readROBfromOKS.value() && robOKSconfigFound) {
      ATH_MSG_INFO(" ---> The list of enabled ROBs has size   = " << m_enabledROBs.value().size() 
                   << ". It was read from the partition database." );
    } else {
      ATH_MSG_INFO(" ---> The list of enabled ROBs has size   = " << m_enabledROBs.value().size() 
                   << ". It was read from job options." );
    }
  }

  ATH_MSG_INFO(" ---> Generic Module ID for LAr MET ROB   = " << m_genericLArMetModuleID);
  if (m_enabledLArMetROBs.value().size() == 0) {
    ATH_MSG_INFO(" ---> The list of LAr MET ROBs has size   = 0. No LAr MET ROB access will be done.");
  } else {
    if (m_readROBfromOKS.value() && robLArMetOKSconfigFound) {
      ATH_MSG_INFO(" ---> The list of LAr MET ROBs has size   = " << m_enabledLArMetROBs.value().size() 
                   << ". It was read from the partition database." );
    } else {
      ATH_MSG_INFO(" ---> The list of LAr MET ROBs has size   = " << m_enabledLArMetROBs.value().size() 
                   << ". It was read from job options." );
    }
  }

  ATH_MSG_INFO(" ---> Generic Module ID for Tile MET ROB  = " << m_genericTileMetModuleID);
  if (m_enabledTileMetROBs.value().size() == 0) {
    ATH_MSG_INFO(" ---> The list of Tile MET ROBs has size  = 0. No Tile MET ROB access will be done.");
  } else {
    if (m_readROBfromOKS.value() && robTileMetOKSconfigFound) {
      ATH_MSG_INFO(" ---> The list of Tile MET ROBs has size  = " << m_enabledTileMetROBs.value().size() 
                   << ". It was read from the partition database." );
    } else {
      ATH_MSG_INFO(" ---> The list of Tile MET ROBs has size  = " << m_enabledTileMetROBs.value().size() 
                   << ". It was read from job options." );
    }
  }

  ATH_MSG_INFO(" ---> Separate MET and Det ROB Retrieval  = " << m_separateMETandDetROBRetrieval);

  if (m_ignoreROB.value().size() == 0) {
    ATH_MSG_INFO(" ---> The list of ROBs to ignore has size = 0. No check will be performed ");
  } else {
    ATH_MSG_INFO(" ---> The list of ROBs to ignore has size = " << m_ignoreROB.value().size());
  }

  for (unsigned int i=0; i<m_ignoreROB.value().size(); i++) {
    ATH_MSG_INFO(" ---> do not retrieve ROB[" << i << "]: hex(id)=0x"
                 << MSG::hex << m_ignoreROB.value()[i]<<MSG::dec
                 << " dec(id)="<< m_ignoreROB.value()[i]);
  }

  // register incident handler for begin run
  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
  ATH_CHECK(incidentSvc.retrieve());

  long int pri=100;
  incidentSvc->addListener(this,"BeginRun",pri);
  incidentSvc.release().ignore();

  // Setup the StoreGateSvc
  ATH_CHECK(m_storeGateSvc.retrieve());

  return sc;
}

// Initialization 
StatusCode HltROBDataProviderSvc::finalize()
{
  StatusCode sc = ROBDataProviderSvc::finalize();
  if ( !sc.isSuccess() ) {
    ATH_MSG_ERROR(" ROBDataProviderSvc::finalize() failed.");
  }

  // release the AlgContextSvc if used
  if ( m_algContextSvc ) m_algContextSvc->release();

  return sc;
}

/// Query interface
StatusCode HltROBDataProviderSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IROBDataProviderSvc::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (IROBDataProviderSvc*)this;
  } else if ( ITrigROBDataProviderSvc::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (ITrigROBDataProviderSvc*)this;
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
void HltROBDataProviderSvc::addROBData(const std::vector<uint32_t>& robIds, const std::string callerName)
{ 
  //-------------------
  // remove empty calls
  //-------------------
  if (robIds.size() == 0) return;

  //--------------------
  // set the caller name
  //--------------------
  if (callerName != "UNKNOWN") m_callerName = callerName;

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

  //---------------------------------------------
  // preload only requests with more than one ROB
  //---------------------------------------------
  if (robIdsUnique.size() <= 1) return;

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
          p_robMonCollection->reserve( HltROBDataProviderConstants::Number_of_Rob_Monitor_Structs ) ;
          if ( (m_storeGateSvc->record(p_robMonCollection, m_ROBDataMonitorCollection_SG_Name.value(), true)).isFailure() ) {
            ATH_MSG_WARNING(" Registering ROB Monitoring collection in StoreGate failed.");
            delete p_robMonCollection;
            p_robMonCollection = 0;
          }
        }
      } else {
        if ( m_storeGateSvc->retrieve(p_robMonCollection).isFailure() ) {
          ATH_MSG_WARNING(" Retrieval of ROB Monitoring collection from StoreGate failed.");
          p_robMonCollection = 0;
        }
      }
    }

    // create a new ROBDataMonitorStruct and fill it
    robmonitor::ROBDataMonitorStruct* p_robMonStruct(0);
    if ( p_robMonCollection ) {
      // caller name
      std::string caller_name("UNKNOWN");
      if (callerName != "UNKNOWN") {
        caller_name = callerName;
      } else if ((callerName == "UNKNOWN") && (m_callerName != "UNKNOWN")) {
        caller_name = m_callerName;
      } else {
        IAlgorithm* alg(0);
        if ( m_algContextSvc ) {
          alg = m_algContextSvc->currentAlg();
          caller_name = (alg ? alg->name() : "<NONE>");
        }
      }

      // initialize new ROBDataMonitorStruct
      p_robMonStruct = new robmonitor::ROBDataMonitorStruct(m_currentLvl1ID, robIdsUnique, caller_name);
    }

    // for online running the requested ROBs should be not found in cache
    // ------------------------------------------------------------------

    // find missing ROB ids which should be retrieved  
    ATH_MSG_DEBUG(" ---> addROBData: Number of ROB Ids requested : " << robIdsUnique.size());

    //--------------------------
    // update internal ROB cache
    //--------------------------
    addROBDataToCache(robIdsUnique, p_robMonStruct);

    // add the ROB monitoring structure to the collection
    if ( p_robMonCollection && p_robMonStruct ) p_robMonCollection->push_back( p_robMonStruct );
  } // end online running   
  return;
}

/** - in online add the LVL1/HLT result
    - rebuild the map
    - set flag for online running
 */
void HltROBDataProviderSvc::setNextEvent(const std::vector<ROBF>& result)
{ 
  // clear the ROB map
  m_online_robmap.clear(); 
  m_currentLvl1ID = 0;

  // set the online flag
  m_onlineRunning = true ;

  // set the complete event flag
  m_isEventComplete = false;

  if ( result.size() == 0 ) {
    ATH_MSG_ERROR(" ---> setNextEvent online for "<< name() 
                  <<" failed: Size of received vector of ROB fragments = " << result.size());
    return;
  }

  // set the LVL1 id
  m_currentLvl1ID = result[0].rod_lvl1_id();

  // add fragments to map
  std::vector<ROBF>::const_iterator it_robf     = result.begin();
  std::vector<ROBF>::const_iterator it_robf_end = result.end();
  for(; it_robf!=it_robf_end; ++it_robf) {
    uint32_t id = it_robf->source_id() ;
    // check current L1 ID against CTP fragment when possible
    if ( (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_CTP) &&
         (it_robf->rod_lvl1_id() != m_currentLvl1ID) ) {
      ATH_MSG_ERROR(" ---> Lvl1 ID mismatch for CTP fragment with SourceId = 0x" << MSG::hex << id << MSG::dec
                    << " and L1 Id = " << it_robf->rod_lvl1_id()
                    << " to currently used L1 Id = " << m_currentLvl1ID
                    << " -> Use CTP version from now on.");
      m_currentLvl1ID = it_robf->rod_lvl1_id() ;
    }
    // remove empty ROB fragments or ones with bad status, if requested
    if ((it_robf->rod_ndata() == 0) && (m_removeEmptyROB)) { 
      ATH_MSG_DEBUG(" ---> Empty ROB Id = 0x" << MSG::hex << id << MSG::dec
                    << " removed for L1 Id = " << m_currentLvl1ID);
    } else if ( ROBDataProviderSvc::filterRobWithStatus(&*it_robf) ) {
      if (msgLvl(MSG::DEBUG) && (it_robf->nstatus() > 0)) {
        const uint32_t* it_status;
        it_robf->status(it_status);
        eformat::helper::Status tmpstatus( (*it_status) ) ;
        ATH_MSG_DEBUG(" ---> ROB Id = 0x" << MSG::hex << id
                      << std::setfill( '0' )
                      << " with Generic Status Code = 0x" << std::setw(4) << tmpstatus.generic()
                      << " and Specific Status Code = 0x" << std::setw(4) << tmpstatus.specific()
                      << MSG::dec
                      << " removed for L1 Id = " << m_currentLvl1ID);
      }
    } else {
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

  if(msgLvl(MSG::DEBUG)) {
    msg() << MSG::DEBUG << " ---> setNextEvent online for "<< name() << endmsg; 
    msg() << MSG::DEBUG << "      online running    = " << m_onlineRunning << endmsg;
    msg() << MSG::DEBUG << "      current LVL1 id   = " << m_currentLvl1ID << endmsg;
    msg() << MSG::DEBUG << "      # LVL1 ROBs       = " << result.size() << endmsg;
    msg() << MSG::DEBUG << "      size of ROB cache = " << m_online_robmap.size() << endmsg;
    msg() << MSG::DEBUG << dumpROBcache() << endmsg; 
  }
  return; 
}

/** - add a new Raw event
    - rebuild the map
    - set flag for offline running
 */
void HltROBDataProviderSvc::setNextEvent(const RawEvent* re)
{ 
  // set the offline flag
  m_onlineRunning = false ;
  
  // set the event complete flag
  m_isEventComplete = true ;

  ROBDataProviderSvc::setNextEvent(re);
  return ;
}

/** return ROBData for ROBID
 */ 
void HltROBDataProviderSvc::getROBData(const std::vector<uint32_t>& robIds, std::vector<const ROBF*>& robFragments, std::string callerName)
{
  //--------------------
  // set the caller name
  //--------------------
  if (callerName != "UNKNOWN") m_callerName = callerName;

  //-------------------
  //--- offline running
  //-------------------
  if (!m_onlineRunning) {
    ROBDataProviderSvc::getROBData(robIds,robFragments);
    //------------------
    //--- online running
    //------------------
  } else {
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

    // detailed ROB monitoring
    //------------------------
    // Create a ROB monitoring collection and register it to StoreGate
    ROBDataMonitorCollection* p_robMonCollection(0); 
    if ((m_doDetailedROBMonitoring.value()) && (robIdsUnique.size() != 0)) {
      if ( !(m_storeGateSvc->transientContains<ROBDataMonitorCollection>(m_ROBDataMonitorCollection_SG_Name.value())) ) {
	p_robMonCollection = new ROBDataMonitorCollection;
	if ( p_robMonCollection ) {
	  p_robMonCollection->reserve( HltROBDataProviderConstants::Number_of_Rob_Monitor_Structs ) ;
	  if ( (m_storeGateSvc->record(p_robMonCollection, m_ROBDataMonitorCollection_SG_Name.value(), true)).isFailure() ) {
	    ATH_MSG_WARNING(" getROBData: Registering ROB Monitoring collection in StoreGate failed.");
	    delete p_robMonCollection;
	    p_robMonCollection = 0;
	  }
	}
      } else {
	if ( m_storeGateSvc->retrieve(p_robMonCollection).isFailure() ) {
	  ATH_MSG_WARNING(" getROBData: Retrieval of ROB Monitoring collection from StoreGate failed.");
	  p_robMonCollection = 0;
	}
      }
    }

    // create a new ROBDataMonitorStruct and fill it
    robmonitor::ROBDataMonitorStruct* p_robMonStruct(0);
    if ( p_robMonCollection ) {
      // caller name
      std::string caller_name("UNKNOWN");
      if (callerName != "UNKNOWN") {
	caller_name = callerName;
      } else if ((callerName == "UNKNOWN") && (m_callerName != "UNKNOWN")) {
	caller_name = m_callerName;
      } else {
	IAlgorithm* alg(0);
	if ( m_algContextSvc ) {
	  alg = m_algContextSvc->currentAlg();
	  caller_name = (alg ? alg->name() : "<NONE>");
	}
      }

      // initialize new ROBDataMonitorStruct
      
      p_robMonStruct = new robmonitor::ROBDataMonitorStruct(m_currentLvl1ID, robIdsUnique, caller_name);
    }

    //--------------------------
    // update internal ROB cache
    //--------------------------
    addROBDataToCache(robIdsUnique, p_robMonStruct);

    // add the ROB monitoring structure to the collection
    if ( p_robMonCollection && p_robMonStruct ) p_robMonCollection->push_back( p_robMonStruct );


    //------------------------------------------------------------------
    // Return requested ROBs from internal cache
    //------------------------------------------------------------------
    for(std::vector<uint32_t>::const_iterator it = robIdsUnique.begin(); it != robIdsUnique.end(); ++it){
      uint32_t id = (*it); 
      ONLINE_ROBMAP::iterator map_it = m_online_robmap.find(id) ; 
      if(map_it != m_online_robmap.end()) {      
        robFragments.push_back( &((*map_it).second) );
      } else {
        ATH_MSG_DEBUG(" ---> getROBData: Failed to find ROB for id 0x"
                      << MSG::hex << id << MSG::dec);
        ATH_MSG_VERBOSE(dumpROBcache());
      }
    }
  }

  return ; 
}

/// Retrieve the whole event.
const RawEvent* HltROBDataProviderSvc::getEvent(){
  if (m_onlineRunning) return 0;
  return ROBDataProviderSvc::getEvent(); 
}

/// Return vector with all ROBFragments stored in the cache 
void HltROBDataProviderSvc::getAllROBData(std::vector<const ROBF*>& robFragments)
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
std::string HltROBDataProviderSvc::dumpROBcache() const {

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
int HltROBDataProviderSvc::collectCompleteEventData(const std::string callerName) {

  // return if call was already issued
  if (m_isEventComplete) return 0;

  //--------------------
  // set the caller name
  //--------------------
  if (callerName != "UNKNOWN") m_callerName = callerName;

  typedef std::vector<hltinterface::DCM_ROBInfo> ROBInfoVec;
  ROBInfoVec vRobInfos ;
  if (m_enabledROBs.value().size() != 0) {
    vRobInfos.reserve( m_enabledROBs.value().size() ) ;
  } else {
    vRobInfos.reserve( HltROBDataProviderConstants::Max_Number_Of_ROBs ) ;
  }

  struct timeval time_start;
  struct timeval time_stop;
  if ( m_doMonitoring || m_doDetailedROBMonitoring.value() ) gettimeofday(&time_start, 0);

  // Get ROB Fragments for complete event with DataCollector
  hltinterface::DataCollector::instance()->collect(vRobInfos, m_currentLvl1ID);

  if ( m_doMonitoring || m_doDetailedROBMonitoring.value() ) gettimeofday(&time_stop, 0);

  if (msgLvl(MSG::DEBUG)) {
    std::ostringstream ost;
    unsigned int rob_counter = 1;
    for (const auto& rob : vRobInfos) {
      ost << "       # = "<< std::setw(5) << rob_counter++ << " ROB id = 0x" 
          << std::hex << rob.robFragment.source_id() << std::dec << "\n" ; 
    }
    ATH_MSG_DEBUG(" ---> collectCompleteEventData: The following ROB Ids were received from DataCollector : \n"
                  << "      Lvl1 id                                             = " << m_currentLvl1ID << "\n"
                  << "      Number of actually received ROB Ids                 = " << vRobInfos.size() << "\n"
                  << ost.str());
  }
    
    
  // detailed ROB monitoring
  //------------------------
  // Create a ROB monitoring collection and register it to StoreGate
  ROBDataMonitorCollection* p_robMonCollection(0); 
  if ( m_doDetailedROBMonitoring.value() ) {
    if ( !(m_storeGateSvc->transientContains<ROBDataMonitorCollection>(m_ROBDataMonitorCollection_SG_Name.value())) ) {
      p_robMonCollection = new ROBDataMonitorCollection;
      if ( p_robMonCollection ) {
        p_robMonCollection->reserve( HltROBDataProviderConstants::Number_of_Rob_Monitor_Structs ) ;
        if ( (m_storeGateSvc->record(p_robMonCollection, m_ROBDataMonitorCollection_SG_Name.value(), true)).isFailure() ) {
          ATH_MSG_WARNING(" Registering ROB Monitoring collection in StoreGate failed.");
          delete p_robMonCollection;
          p_robMonCollection = 0;
        }
      }
    } else {
      if ( m_storeGateSvc->retrieve(p_robMonCollection).isFailure() ) {
        ATH_MSG_WARNING(" Retrieval of ROB Monitoring collection from StoreGate failed.");
        p_robMonCollection = 0;
      }
    }
  }
  
  // create a new ROBDataMonitorStruct and fill it
  robmonitor::ROBDataMonitorStruct* p_robMonStruct(0);
  if ( p_robMonCollection ) {
    // caller name
    std::string caller_name("UNKNOWN");
    if (callerName != "UNKNOWN") {
      caller_name = callerName;
    } else if ((callerName == "UNKNOWN") && (m_callerName != "UNKNOWN")) {
      caller_name = m_callerName;
    } else {
      IAlgorithm* alg(0);
      if ( m_algContextSvc ) {
        alg = m_algContextSvc->currentAlg();
        caller_name = (alg ? alg->name() : "<NONE>");
      }
    }

    // get ROB Ids
    std::vector<uint32_t> robIds;
    robIds.reserve(vRobInfos.size());
    for(ROBInfoVec::const_iterator it=vRobInfos.begin(); it!=vRobInfos.end(); ++it) {
      robIds.push_back( it->robFragment.source_id() ) ;
    }

    // initialize new ROBDataMonitorStruct
    p_robMonStruct = new robmonitor::ROBDataMonitorStruct(m_currentLvl1ID, robIds, caller_name);
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
      m_hist_receivedROBsPerCall->Fill(vRobInfos.size());
      m_hist_receivedROBsPerCall->LabelsDeflate("X");
    }

    //* detailed monitoring
    if ( p_robMonStruct ) {
      p_robMonStruct->start_time_of_ROB_request    = time_start;
      p_robMonStruct->end_time_of_ROB_request      = time_stop;
    }
  }

  // add ROBs to cache
  updateROBDataCache(vRobInfos,p_robMonStruct);

  // add the ROB monitoring structure to the collection
  if ( p_robMonCollection && p_robMonStruct ) p_robMonCollection->push_back( p_robMonStruct );

  // update event complete flag
  m_isEventComplete = true;

  return vRobInfos.size();
} // end int collectCompleteEventData(...)

/// set the name of the program which uses the ROBDataProviderSvc
void HltROBDataProviderSvc::setCallerName(const std::string callerName) {
  m_callerName = callerName;
}

// handler for BeginRun
void HltROBDataProviderSvc::handle(const Incident& incident) {
  if (incident.type()!="BeginRun") return;
  ATH_MSG_DEBUG("In BeginRun incident.");

  // if detailed ROB monitoring is requested, check if the AlgContextSvc is running, 
  // if yes use it to obtain the calling algorithm name
  if ( m_doDetailedROBMonitoring.value() ) {
    if ( service("AlgContextSvc", m_algContextSvc, /*createIf=*/ false).isFailure() ) {
      ATH_MSG_ERROR("Error retrieving AlgContextSvc."  
                    << "Calling algorithm name not available in detailed ROB monitoring");
      m_algContextSvc=0;
    }
  }

  // define histograms if monitoring is requested
  if ( !m_doMonitoring.value() ) return;

  // find histogramming service
  ServiceHandle<ITHistSvc> rootHistSvc("THistSvc", name());
  if ((rootHistSvc.retrieve()).isFailure()) {
    ATH_MSG_ERROR("Unable to locate THistSvc");
    rootHistSvc.release().ignore();
    return;
  }

  // *-- booking path
  std::string path = std::string("/EXPERT/")+getGaudiThreadGenericName(name())+"/";

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
      ATH_MSG_WARNING("Can not register monitoring histogram: " << m_hist_requestedROBsPerCall->GetName());
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
      ATH_MSG_WARNING("Can not register monitoring histogram: " << m_hist_receivedROBsPerCall->GetName());
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
      ATH_MSG_WARNING("Can not register monitoring histogram: " << m_hist_timeROBretrieval->GetName());
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
        it_sub != eformat::helper::SubDetectorDictionary.end(); ++it_sub ) {
      m_hist_genericStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, (it_sub->second).c_str() );
      n_tmp_bin++;
    }

    n_tmp_bin = 1;
    for (std::map<eformat::GenericStatus, std::string>::const_iterator it = m_map_GenericStatus.begin();it != m_map_GenericStatus.end();++it) {
      m_hist_genericStatusForROB->GetYaxis()->SetBinLabel( n_tmp_bin, (*it).second.c_str() );
      n_tmp_bin++;
    }
    if( rootHistSvc->regHist(path + m_hist_genericStatusForROB->GetName(), m_hist_genericStatusForROB).isFailure() ) {
      ATH_MSG_WARNING("Can not register monitoring histogram: " << m_hist_genericStatusForROB->GetName());
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
        it_sub != eformat::helper::SubDetectorDictionary.end(); ++it_sub ) {
      m_hist_specificStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, (it_sub->second).c_str() );
      n_tmp_bin++;
    }

    n_tmp_bin = 1;
    for (std::vector<std::string>::const_iterator it = m_vec_SpecificStatus.begin();it != m_vec_SpecificStatus.end();++it) {
      m_hist_specificStatusForROB->GetYaxis()->SetBinLabel( n_tmp_bin, (*it).c_str() );
      n_tmp_bin++;
    }
    if( rootHistSvc->regHist(path + m_hist_specificStatusForROB->GetName(), m_hist_specificStatusForROB).isFailure() ) {
      ATH_MSG_WARNING("Can not register monitoring histogram: " << m_hist_specificStatusForROB->GetName());
    }
  }

  // release histogramming service
  rootHistSvc.release().ignore();
} // end handler for BeginRun

// helper function to retrieve ROB fragments over the network and to add them to the cache 
void HltROBDataProviderSvc::addROBDataToCache(std::vector<uint32_t>& robIdsForRetrieval,
					      robmonitor::ROBDataMonitorStruct* p_robMonStruct) {

  struct timeval time_start;
  struct timeval time_stop;

  ATH_MSG_DEBUG(" ---> addROBDataToCache: Number of ROB Ids requested for retrieval : " 
                << robIdsForRetrieval.size() << ", Lvl1 id = " << m_currentLvl1ID);

  // return if no ROBs are requested
  if (robIdsForRetrieval.size() == 0) return;

  
  std::vector<uint32_t> vRobIds, vMETRobIds;
  vRobIds.reserve( robIdsForRetrieval.size() );
  vMETRobIds.reserve( robIdsForRetrieval.size() );

  // Check requested ROBs
  for (std::vector<uint32_t>::const_iterator rob_it=robIdsForRetrieval.begin(); rob_it!=robIdsForRetrieval.end(); ++rob_it) {
    uint32_t id =  (*rob_it);
    
    // check first if ROB is already in cache
    ONLINE_ROBMAP::iterator map_it = m_online_robmap.find(*rob_it) ;
    if(map_it != m_online_robmap.end()) {
      ATH_MSG_DEBUG(" ---> addROBDataToCache: Found   ROB Id : 0x" << MSG::hex << (*map_it).second.source_id() 
                    << MSG::dec <<" in cache ");
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

    // check if ROB is actually enabled for readout
    // do not perform this check for MET ROBs
    if (m_enabledROBs.value().size() != 0) { 
      if ( (eformat::helper::SourceIdentifier(*rob_it).subdetector_id() != eformat::TDAQ_LAR_MET) &&
	   (eformat::helper::SourceIdentifier(*rob_it).subdetector_id() != eformat::TDAQ_TILE_MET) ){
	std::vector<uint32_t>::const_iterator rob_enabled_it =
	  std::find(m_enabledROBs.value().begin(), m_enabledROBs.value().end(),(*rob_it));
	if(rob_enabled_it == m_enabledROBs.value().end()) {
	  ATH_MSG_DEBUG(" ---> addROBDataToCache: ROB Id : 0x" << MSG::hex << (*rob_it) << MSG::dec
			<< " will be not retrieved, since it is not on the list of enabled ROBs.");
	  if ( p_robMonStruct ) {
	    (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::DISABLED;
	  }
	  continue;
	}
      }
    }

    if (m_ignoreROB.value().size() != 0) {
      std::vector<uint32_t>::const_iterator rob_ignore_it =
	std::find(m_ignoreROB.value().begin(), m_ignoreROB.value().end(),id);
      if(rob_ignore_it != m_ignoreROB.value().end()) {
	ATH_MSG_DEBUG(" ---> addROBDataToCache: ROB Id : 0x" << MSG::hex << id << MSG::dec
		      << " will be not retrieved, since it is on the veto list.");
	if ( p_robMonStruct ) {
	  (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::IGNORED;
	}
	continue;
      }
    }

    // separate MET and detector ROBs if requested
    if ( (m_separateMETandDetROBRetrieval.value()) &&
      ( (eformat::helper::SourceIdentifier(*rob_it).subdetector_id() == eformat::TDAQ_LAR_MET) || 
	(eformat::helper::SourceIdentifier(*rob_it).subdetector_id() == eformat::TDAQ_TILE_MET) ) )  {
        vMETRobIds.push_back( *rob_it ) ;
    } else {
      vRobIds.push_back( *rob_it );
    }
  }
 


  typedef std::vector<hltinterface::DCM_ROBInfo> ROBInfoVec;
  ROBInfoVec vRobInfos ;

  // Get ROB Fragments with DataCollector
  if ( m_doMonitoring || p_robMonStruct ) gettimeofday(&time_start, 0);
  if ( vRobIds.size() != 0 ) {
    vRobInfos.reserve( vRobIds.size() ) ;
    hltinterface::DataCollector::instance()->collect(vRobInfos, m_currentLvl1ID, vRobIds);
  }

  // Do a separate data collect call for MET ROBs if required
  if ( (m_separateMETandDetROBRetrieval.value()) && (vMETRobIds.size() != 0) ) {
    ROBInfoVec vMETRobInfos;
    vMETRobInfos.reserve( vMETRobIds.size() ) ;
    // retrieve MET ROBs
    hltinterface::DataCollector::instance()->collect(vMETRobInfos, m_currentLvl1ID, vMETRobIds);

    // add MET ROBs to Det ROBs
    vRobInfos.insert( vRobInfos.end(), vMETRobInfos.begin(), vMETRobInfos.end() );
  }

  if(msgLvl(MSG::DEBUG) && ((vRobIds.size()!=0) || (vMETRobIds.size()!=0))) {
    std::ostringstream ost;
    unsigned int rob_counter = 1;
    for (const auto& rob : vRobInfos) {
      ost << "       # = "<< std::setw(5) << rob_counter++ << " ROB id = 0x" << std::hex 
          << rob.robFragment.source_id() << std::dec << "\n" ; 
    }
    ATH_MSG_DEBUG(" ---> addROBDataToCache: The following ROB Ids were received from DataCollector : \n"
                  << "      Lvl1 id                                             = " << m_currentLvl1ID << "\n"
                  << "      Number of detector ROB Ids requested for retrieval  = " << vRobIds.size() << "\n"
                  << "      Number of MET ROB Ids requested for retrieval       = " << vMETRobIds.size() << "\n"
                  << "      Number of actually received ROB Ids                 = " << vRobInfos.size() << "\n"
                  << ost.str());
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
    m_hist_receivedROBsPerCall->Fill(vRobInfos.size());
    m_hist_receivedROBsPerCall->LabelsDeflate("X");
  }

  // add ROBs to cache
  updateROBDataCache(vRobInfos,p_robMonStruct);

  return;
} // end void addROBDataToCache(...)


// helper function to put retrieved ROB fragments into the local cache and update the monitoring records 
void HltROBDataProviderSvc::updateROBDataCache(std::vector<hltinterface::DCM_ROBInfo>& vRobInfo,
					      robmonitor::ROBDataMonitorStruct* p_robMonStruct) {

  ATH_MSG_DEBUG(" ---> updateROBDataCache: Number of ROB Info records for cache update : " 
                << vRobInfo.size() << ", Lvl1 id = " << m_currentLvl1ID);

  // return if no ROB Info records are available
  if (vRobInfo.size() == 0) return;
  
  
  // add ROBs to cache
  typedef std::vector<hltinterface::DCM_ROBInfo> ROBInfoVec;
  for(ROBInfoVec::const_iterator it=vRobInfo.begin(); it!=vRobInfo.end(); ++it) {
    uint32_t id = it->robFragment.source_id() ;

    // check first if ROB is already in cache (for full event requests)
    ONLINE_ROBMAP::iterator map_it = m_online_robmap.find(id) ;
    if(map_it != m_online_robmap.end()) continue;
    
    if ((it->robFragment.rod_ndata() == 0) && (m_removeEmptyROB)) {
      ATH_MSG_DEBUG(" ---> addROBDataToCache: Empty ROB Id = 0x" << MSG::hex << id << MSG::dec
                    << " removed for L1 Id = " << m_currentLvl1ID);
      if ( p_robMonStruct ) {
      	
          (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::IGNORED;
        }
    } else if ( ROBDataProviderSvc::filterRobWithStatus(&it->robFragment)) {
      if (msgLvl(MSG::DEBUG) && (it->robFragment.nstatus() > 0)) {
        const uint32_t* it_status;
        it->robFragment.status(it_status);
        eformat::helper::Status tmpstatus( (*it_status) ) ;
        ATH_MSG_DEBUG(" ---> addROBDataToCache: ROB Id = 0x" << MSG::hex << id
                      << std::setfill( '0' )
                      << " with Generic Status Code = 0x" << std::setw(4) << tmpstatus.generic()
                      << " and Specific Status Code = 0x" << std::setw(4) << tmpstatus.specific()
                      << MSG::dec
                      << " removed for L1 Id = " << m_currentLvl1ID);
      }
      if ( p_robMonStruct ) {
      	 (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::IGNORED;
        }
    } else {
      m_online_robmap[id]= (it->robFragment);

      //* detailed monitoring
      if ( p_robMonStruct ) {
	(p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::RETRIEVED;
	(p_robMonStruct->requested_ROBs)[id].rob_size    = it->robFragment.fragment_size_word();
	if ( it->robFragment.nstatus() != 0 ) {
	  const uint32_t* it_status;
	  it->robFragment.status(it_status);
	  for (uint32_t k=0; k < it->robFragment.nstatus(); k++) {
	    (p_robMonStruct->requested_ROBs)[id].rob_status_words.push_back( *(it_status+k) );
	  }
	}
      } // end detailed monitoring
    }

    //* fill monitoring histogram for ROB generic status
    if ( ( m_hist_genericStatusForROB ) && ( it->robFragment.nstatus() != 0 ) ) {
      const uint32_t* it_status;
      it->robFragment.status(it_status);
      if ((*it_status) != 0) {
        scoped_lock_histogram lock;
        m_hist_genericStatusForROB->Fill(eformat::helper::SourceIdentifier(it->robFragment.source_id()).human_detector().c_str(),
            m_map_GenericStatus[eformat::helper::Status(*it_status).generic()].c_str(),1.);
      }
    }

    //* fill monitoring histogram for ROB specific status
    if ( ( m_hist_specificStatusForROB ) && ( it->robFragment.nstatus() != 0 ) ) {
      const uint32_t* it_status;
      it->robFragment.status(it_status);
      if ((*it_status) != 0) {
        scoped_lock_histogram lock;
        std::bitset<16> specificBits(eformat::helper::Status(*it_status).specific());
        for (unsigned int index=0; index < 16; ++index) {
          if (specificBits[index]) m_hist_specificStatusForROB->Fill(eformat::helper::SourceIdentifier(it->robFragment.source_id()).human_detector().c_str(),
              m_vec_SpecificStatus[index].c_str(),1.);
        }
      }
    }
  }   // end loop over ROBInfo records
  return;
} // end void updateROBDataCache(...)
