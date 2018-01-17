/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//===================================================================
//  Implementation of MonROBDataProviderSvc
//             
//===================================================================

// Include files.
#include "TrigDataAccessMonitoring/MonROBDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IAlgorithm.h"

#include "StoreGate/StoreGateSvc.h"

#include <iostream>
#include <string>
#include <iomanip>
#include <cassert> 
#include <bitset>
#include <algorithm> 
#include <TH1F.h>
#include <TH2F.h>

// Constructor.
MonROBDataProviderSvc::MonROBDataProviderSvc(const std::string& name, ISvcLocator* svcloc)
  :ROBDataProviderSvc(name,svcloc),
   m_storeGateSvc( "StoreGateSvc", name ),
   m_algContextSvc(0),  m_msg(0),
   m_histProp_requestedROBsPerCall(Gaudi::Histo1DDef("RequestedROBsPerCall" ,0,50,50)),
   m_histProp_receivedROBsPerCall(Gaudi::Histo1DDef("ReceivedROBsPerCall" ,0,50,50)),
   m_histProp_timeROBretrieval(Gaudi::Histo1DDef("TimeForROBretrieval" ,0.,5.,50)),
   m_hist_requestedROBsPerCall(0),
   m_hist_receivedROBsPerCall(0),
   m_hist_timeROBretrieval(0),
   m_hist_genericStatusForROB(0),
   m_hist_specificStatusForROB(0)
{
  declareProperty("doMonitoring", m_doMonitoring=true,"Enable histograms");
  declareProperty("doDetailedROBMonitoring", m_doDetailedROBMonitoring=true,"Produce ROB cost data");
  declareProperty("ROBDataMonitorCollectionSGName", m_ROBDataMonitorCollection_SG_Name="ROBDataMonitorCollection","Name of cost monitoring collection in SG");
  declareProperty("HistRequestedROBsPerCall", m_histProp_requestedROBsPerCall,"Number of ROBs requested");
  declareProperty("HistReceivedROBsPerCall", m_histProp_receivedROBsPerCall,"Number of ROBs received");
  declareProperty("HistTimeROBretrieval", m_histProp_timeROBretrieval,"Timing for ROB retrieval");
  

  // fill vector with specific status codes
  m_vec_SpecificStatus = {
    "TRIGGER_TYPE_SYNC_ERROR", 
    "FRAGMENT_SIZE_ERROR", 
    "DATABLOCK_ERROR", 
    "CTRL_WORD_ERROR", 
    "MISSING_BOF", 
    "MISSING_EOF", 
    "INVALID_HEADER_MARKER", 
    "FORMAT_ERROR", 
    "DUPLICATE_EVENT", 
    "SEQUENCE_ERROR", 
    "TRANSMISSION_ERROR", 
    "TRUNCATION", 
    "SHORT_FRAGMENT", 
    "FRAGMENT_LOST", 
    "FRAGMENT_PENDING", 
    "ROL_DISABLED", 
  };

  m_map_GenericStatus =  {
	{eformat::UNCLASSIFIED,       "UNCLASSIFIED"}, 
	{eformat::BCID_CHECK_FAIL,    "BCID_CHECK_FAIL"}, 
	{eformat::LVL1ID_CHECK_FAIL,  "LVL1ID_CHECK_FAIL"}, 
	{eformat::TIMEOUT,            "TIMEOUT"}, 
	{eformat::DATA_CORRUPTION,    "DATA_CORRUPTION"}, 
	{eformat::INTERNAL_OVERFLOW,  "INTERNAL_OVERFLOW"}
  };

} 

const std::string& MonROBDataProviderSvc::genericStatusName( eformat::GenericStatus s ) const {
  // fill map with generic status codes
  static std::string unknown = "UNKNOWN";
  std::map<eformat::GenericStatus, std::string>::const_iterator i = m_map_GenericStatus.find( s );
  if ( i == m_map_GenericStatus.end() ) 
    return unknown;
  return i->second;
}

// Destructor.
MonROBDataProviderSvc::~MonROBDataProviderSvc()
{
}

// Initialization 
StatusCode MonROBDataProviderSvc::initialize()
{
  StatusCode sc = ROBDataProviderSvc::initialize();

  // set message stream
  m_msg  = new MsgStream( msgSvc(), name() );

  logStream() << MSG::INFO << " ---> MonROBDataProviderSvc = " << name() << " initialize "
	      << " - package version " << PACKAGE_VERSION << endmsg ;

  if ( sc.isFailure() ) {
    logStream() << MSG::ERROR << " ROBDataProviderSvc::initialize() failed." << endmsg;
    return sc;
  }

  logStream() << MSG::INFO << " ---> MonROBDataProviderSvc              = " << name() << " special properties <---" << endmsg;
  logStream() << MSG::INFO << " ---> Fill monitoring histograms          = " << m_doMonitoring << endmsg;
  logStream() << MSG::INFO << "        Hist:RequestedROBsPerCall         = " << m_histProp_requestedROBsPerCall << endmsg; 
  logStream() << MSG::INFO << "        Hist:ReceivedROBsPerCall          = " << m_histProp_receivedROBsPerCall << endmsg; 
  logStream() << MSG::INFO << "        Hist:TimeROBretrieval             = " << m_histProp_timeROBretrieval << endmsg; 
  logStream() << MSG::INFO << " ---> Do detailed ROB monitoring          = " << m_doDetailedROBMonitoring << endmsg;
  logStream() << MSG::INFO << " ---> SG name for ROB monitoring collect. = " << m_ROBDataMonitorCollection_SG_Name << endmsg;

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
  if( (m_storeGateSvc.retrieve()).isFailure() ) {
    logStream() << MSG::ERROR << "Error retrieving StoreGateSvc " << m_storeGateSvc << endmsg;
    m_storeGateSvc.release().ignore();
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode MonROBDataProviderSvc::finalize()
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
StatusCode MonROBDataProviderSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IROBDataProviderSvc::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (IROBDataProviderSvc*)this;
  } else if ( IIncidentListener::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (IIncidentListener*)this;
  } else {
   return Service::queryInterface(riid, ppvInterface);
  } 
  addRef(); 
  return StatusCode::SUCCESS; 
}

/// addROBData (no effect in offline, all ROBS were already read in from full event)
void MonROBDataProviderSvc::addROBData(const std::vector<uint32_t>& robIds, const std::string callerName) 
{ 
  // for offline running all requested ROBs should be found in cache
  ROBDataProviderSvc::addROBData(robIds,callerName);

  //------------------------
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
    // caller name
    std::string caller_name("UNKNOWN");
    if (callerName != "UNKNOWN") {
      caller_name = callerName;
    } else {
      IAlgorithm* alg(0);
      if ( m_algContextSvc ) {
	alg = m_algContextSvc->currentAlg();
	caller_name = (alg ? alg->name() : "<NONE>");
      }
    }
    // initialize new ROBDataMonitorStruct
    p_robMonStruct = new robmonitor::ROBDataMonitorStruct(getEvent()->lvl1_id(), robIds, caller_name);
  }

  // for offline running the requested ROBs should be found in the cache
  // -------------------------------------------------------------------
  // start timer to simulate ROB retrieval time (--> not meaningful in offline environment)
  struct timeval time_start;
  if ( m_doMonitoring || p_robMonStruct ) gettimeofday(&time_start, 0);

  // find requested ROB ids in cache, count missing ones 
  uint32_t number_ROB_found(0);
  uint32_t number_ROB_not_found(0);

  const EventContext context{ Gaudi::Hive::currentContext() };
  const ROBMAP& robmap = m_eventsCache.get(context)->robmap;

  for(std::vector<uint32_t>::const_iterator it=robIds.begin(); it!=robIds.end(); ++it){
    uint32_t id = (*it);
    // mask off the module ID for L2 and EF result for Run 1 data
    if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) &&
	 (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_LVL2) ) {
      id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
    } else if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) &&
		(eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_EVENT_FILTER) &&
		(ROBDataProviderSvc::m_maskL2EFModuleID) ) {
      id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
    }

    // check if ROB is already in cache
    ROBDataProviderSvc::ROBMAP::const_iterator map_it = robmap.find(id) ;
    if(map_it != robmap.end()) { // ROB found in cache
      number_ROB_found++;

      if(logLevel() <= MSG::DEBUG)
	logStream() << MSG::DEBUG << " ---> Found   ROB Id : 0x" << MSG::hex << (*map_it).second->source_id() << MSG::dec
		    <<" in cache "<< endmsg;
      //* detailed monitoring
      if ( p_robMonStruct ) {
	// check if ROB was already accessed before and set ROB history accordingly
	uint32_t accessed_id = id;
	// mask off the module ID for L2 and EF result for Run 1 data
	if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) &&
	     (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_LVL2) ) {
	  accessed_id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
	} else if ( (eformat::helper::SourceIdentifier(id).module_id() != 0) &&
		    (eformat::helper::SourceIdentifier(id).subdetector_id() == eformat::TDAQ_EVENT_FILTER) &&
		    (ROBDataProviderSvc::m_maskL2EFModuleID) ) {
	  accessed_id = eformat::helper::SourceIdentifier(eformat::helper::SourceIdentifier(id).subdetector_id(),0).code();
	}
	std::vector<uint32_t>::iterator robAlreadyAccessed_it = find(m_robAlreadyAccessed.get(context)->begin(), m_robAlreadyAccessed.get(context)->end(), accessed_id);
	if (robAlreadyAccessed_it != m_robAlreadyAccessed.get(context)->end()) {
	  (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::CACHED;    // the ROB was already accessed
	} else {
	  (p_robMonStruct->requested_ROBs)[id].rob_history = robmonitor::RETRIEVED; // first time the ROB is accessed
	  m_robAlreadyAccessed.get(context)->push_back(accessed_id);
	}
	(p_robMonStruct->requested_ROBs)[id].rob_size = ((*map_it).second)->fragment_size_word();
	if ( (*map_it).second->nstatus() != 0 ) {
	  const uint32_t* it_status;
	  (*map_it).second->status(it_status);
	  for (uint32_t k=0; k < (*map_it).second->nstatus(); k++) {
	    (p_robMonStruct->requested_ROBs)[id].rob_status_words.push_back( *(it_status+k) );
	  }//end loop over status words

	  //* fill monitoring histogram for ROB generic status
	  if ( m_hist_genericStatusForROB ) {
	    if ((*it_status) != 0) m_hist_genericStatusForROB->Fill(eformat::helper::SourceIdentifier((*map_it).second->source_id()).human_detector().c_str(),
								    genericStatusName(eformat::helper::Status(*it_status).generic()).c_str(),1.);
	  }

	  //* fill monitoring histogram for ROB specific status
	  if ( m_hist_specificStatusForROB ) {
	    if ((*it_status) != 0) {
	      std::bitset<16> specificBits(eformat::helper::Status(*it_status).specific());
	      for (unsigned int index=0; index < 16; ++index) { 
		if (specificBits[index]) m_hist_specificStatusForROB->Fill(eformat::helper::SourceIdentifier((*map_it).second->source_id()).human_detector().c_str(),
									   m_vec_SpecificStatus[index].c_str(),1.);
	      }
	    }
	  }
	}//end nstatus() != 0
      } //end detailed monitoring
    } else { // ROB not found in cache
      number_ROB_not_found++;
    } 
  } // end loop over requested ROBs

  // stop timer
  struct timeval time_stop;
  if ( m_doMonitoring || p_robMonStruct ) {
    gettimeofday(&time_stop, 0);
    int secs = 0 ;
    if (time_stop.tv_sec >= time_start.tv_sec)
      secs = time_stop.tv_sec - time_start.tv_sec;

    int usecs = time_stop.tv_usec - time_start.tv_usec;
    float mtime = static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;
	
    //* timing histogram
    if (m_hist_timeROBretrieval) m_hist_timeROBretrieval->Fill(mtime);

    //* detailed monitoring
    if ( p_robMonStruct ) {
      p_robMonStruct->start_time_of_ROB_request    = time_start;
      p_robMonStruct->end_time_of_ROB_request      = time_stop;
    }
  }

  if(logLevel() <= MSG::DEBUG)
    logStream() << MSG::DEBUG 
		<< " ---> Number of ROB Id s requested : " << robIds.size()
		<< ". Number of ROB Id s found/not found = " << number_ROB_found 
		<< "/" << number_ROB_not_found
		<< " for Lvl1 id = "<< getEvent()->lvl1_id()
		<< endmsg;

  //* histograms for number of requested/received ROBs
  if ( m_hist_requestedROBsPerCall ) m_hist_requestedROBsPerCall->Fill(robIds.size());
  if ( m_hist_receivedROBsPerCall )  m_hist_receivedROBsPerCall->Fill(number_ROB_found);

  // add the ROB monitoring structure to the collection
  if ( p_robMonCollection ) p_robMonCollection->push_back( p_robMonStruct );
  return;
}

void MonROBDataProviderSvc::setNextEvent(const std::vector<ROBF>& result) 
{   
  const EventContext context{ Gaudi::Hive::currentContext() };  
  ROBDataProviderSvc::setNextEvent(context, result);
  m_robAlreadyAccessed.get(context)->clear();
  m_robAlreadyAccessed.get(context)->resize( m_eventsCache.get(context)->robmap.size() );
  return; 
}

void MonROBDataProviderSvc::setNextEvent(const RawEvent* re) 
{ 
  const EventContext context{ Gaudi::Hive::currentContext() };
  ROBDataProviderSvc::setNextEvent(context, re);
  m_robAlreadyAccessed.get(context)->clear();
  m_robAlreadyAccessed.get(context)->resize( m_eventsCache.get(context)->robmap.size() );
  return ;
}

/** return ROBData for ROBID
 */ 
void MonROBDataProviderSvc::getROBData(const std::vector<uint32_t>& robIds, std::vector<const ROBF*>& robFragments, std::string callerName) 
{
  ROBDataProviderSvc::getROBData(robIds,robFragments, callerName);
  return ; 
}

/// Retrieve the whole event.
const RawEvent* MonROBDataProviderSvc::getEvent(){
  return ROBDataProviderSvc::getEvent(); 
}

// handler for BeginRun
void MonROBDataProviderSvc::handle(const Incident& incident) {
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
  uint32_t n_bins_partEBSubDet = 60 ;

  // *-- number of requested ROBs per call
  m_hist_requestedROBsPerCall = new TH1F (m_histProp_requestedROBsPerCall.value().title().c_str(),
					  (m_histProp_requestedROBsPerCall.value().title()+";number of ROBs").c_str(), 
					  m_histProp_requestedROBsPerCall.value().bins(), 
					  m_histProp_requestedROBsPerCall.value().lowEdge(), 
					  m_histProp_requestedROBsPerCall.value().highEdge());
  if (m_hist_requestedROBsPerCall) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
     m_hist_requestedROBsPerCall->SetCanExtend(TH1::kAllAxes);
#else     
     m_hist_requestedROBsPerCall->SetBit(TH1::kCanRebin);
#endif     
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
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
     m_hist_receivedROBsPerCall->SetCanExtend(TH1::kAllAxes);
#else
     m_hist_receivedROBsPerCall->SetBit(TH1::kCanRebin);
#endif     
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
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
     m_hist_timeROBretrieval->SetCanExtend(TH1::kAllAxes);
#else
     m_hist_timeROBretrieval->SetBit(TH1::kCanRebin);
#endif
    if( rootHistSvc->regHist(path + m_hist_timeROBretrieval->GetName(), m_hist_timeROBretrieval).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_timeROBretrieval->GetName() << endmsg;
    }
  }

  // *-- Generic Status for ROBs per sub detector
  m_hist_genericStatusForROB = new TH2F ("GenericStatusForROBsFromSubDetectors",
					 "GenericStatusForROBsFromSubDetectors;;",
					 n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet,
					 m_map_GenericStatus.size(), 0., (float) m_map_GenericStatus.size());
  if (m_hist_genericStatusForROB) {
    uint32_t n_tmp_bin = 1;
    for (uint16_t i=0; i<256; i++) {
      eformat::helper::SourceIdentifier tmpsrc( (eformat::SubDetector) i,0);
      if ((tmpsrc.human_detector() != "UNKNOWN") && (n_tmp_bin <= n_bins_partEBSubDet)) {
        m_hist_genericStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, tmpsrc.human_detector().c_str() );
        n_tmp_bin++;
      }
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
    for (uint16_t i=0; i<256; i++) {
      eformat::helper::SourceIdentifier tmpsrc( (eformat::SubDetector) i,0);
      if ((tmpsrc.human_detector() != "UNKNOWN") && (n_tmp_bin <= n_bins_partEBSubDet)) {
        m_hist_specificStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, tmpsrc.human_detector().c_str() );
        n_tmp_bin++;
      }
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
