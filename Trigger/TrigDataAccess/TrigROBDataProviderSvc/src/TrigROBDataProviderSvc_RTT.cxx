/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigROBDataProviderSvc_RTT.h"

#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IAlgorithm.h"

#include "StoreGate/StoreGateSvc.h"

#include "CxxUtils/excepts.h"

#include <TH1F.h>
#include <TH2F.h>

TrigROBDataProviderSvc_RTT::TrigROBDataProviderSvc_RTT(const std::string& name, ISvcLocator* svcloc)
  : TrigROBDataProviderSvc(name,svcloc),
    m_enablePrefetchingAtAlgoLevel(true),
    // hit properties
    m_histProp_requestedROBsPerAlgo(Gaudi::Histo1DDef("RequestedROBsPerAlgo" ,0.,1.,1)),
    m_histProp_requestPerAlgo(Gaudi::Histo1DDef("RequestPerAlgo" ,0.,1.,1)),      
    m_histProp_declaredROBsPerAlgo(Gaudi::Histo1DDef("DeclaredROBsPerAlgo" ,0.,1.,1)),
    m_histProp_prefetchedROBsPerAlgo(Gaudi::Histo1DDef("PrefetchedROBsPerAlgo" ,0.,1.,1)),
    m_histProp_missingRequestedROBsPerAlgo(Gaudi::Histo1DDef("MissingRequestedROBsPerAlgo" ,0.,1.,1)),
    m_histProp_missingRequestPerAlgo(Gaudi::Histo1DDef("MissingRequestPerAlgo" ,0.,1.,1)),
    m_histProp_missingRequestedROBsPerCall(Gaudi::Histo1DDef("MissingRequestedROBsPerCall" ,0.,100.,100)),
    m_histProp_missingRequestedROBsPerAlgo_pref(Gaudi::Histo1DDef("MissingRequestedROBsPerAlgo_pref" ,0.,1.,1)),
    m_histProp_missingRequestPerAlgo_pref(Gaudi::Histo1DDef("MissingRequestPerAlgo_pref" ,0.,1.,1)),
    m_histProp_missingRequestedROBsPerCall_pref(Gaudi::Histo1DDef("MissingRequestedROBsPerCall_pref" ,0.,100.,100)),
    //hist init
    m_hist_requestedROBsPerAlgo(0),
    m_hist_requestPerAlgo(0),
    m_hist_declaredROBsPerAlgo(0),
    m_hist_prefetchedROBsPerAlgo(0),
    m_hist_missingRequestedROBsPerAlgo(0),
    m_hist_missingRequestPerAlgo(0),
    m_hist_missingRequestedROBsPerCall(0),
    m_hist_missingRequestedROBsPerAlgo_pref(0),
    m_hist_missingRequestPerAlgo_pref(0),
    m_hist_missingRequestedROBsPerCall_pref(0)
 {
  declareProperty("enablePrefetchingAtAlgoLevel", m_enablePrefetchingAtAlgoLevel=true, "Enables pre-fetching at algorithm level");
  declareProperty("HistMissingRequestedROBsPerCall", m_histProp_missingRequestedROBsPerCall,"Number of missing ROBs pre-requests per call");
  declareProperty("HistMissingRequestedROBsPerAlgo", m_histProp_missingRequestedROBsPerAlgo,"Number of missing ROBs pre-requests per algo");
  declareProperty("HistMissingRequestPerAlgo", m_histProp_missingRequestPerAlgo,"Number of missing pre-requests per algo");
  declareProperty("HistMissingRequestedROBsPerCall_pref", m_histProp_missingRequestedROBsPerCall_pref,"Number of missing ROBs pre-fetches per call");
  declareProperty("HistMissingRequestedROBsPerAlgo_pref", m_histProp_missingRequestedROBsPerAlgo_pref,"Number of missing ROBs pre-fetches per algo");
  declareProperty("HistMissingRequestPerAlgo_pref", m_histProp_missingRequestPerAlgo_pref,"Number of missing pre-fetches per algo");
  declareProperty("HistRequestedROBsPerAlgo", m_histProp_requestedROBsPerAlgo,"Number of requested ROBs per algo");
  declareProperty("HistRequestPerAlgo", m_histProp_requestPerAlgo,"Number of pre-requests per algo");
  declareProperty("HistDeclaredROBsPerAlgo", m_histProp_declaredROBsPerAlgo,"Number of declared ROBs per algo");
  declareProperty("HistPrefetchedROBsPerAlgo", m_histProp_prefetchedROBsPerAlgo,"Number of prefetched ROBs per algo");
}

/// Query interface
StatusCode TrigROBDataProviderSvc_RTT::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IROBDataProviderSvc::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (IROBDataProviderSvc*)this;
  } else if ( ITrigROBDataProviderSvc::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (ITrigROBDataProviderSvc*)this;
  } else if ( ITrigROBDataProviderSvc_RTT::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (ITrigROBDataProviderSvc_RTT*)this;
  } else if ( IIncidentListener::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (IIncidentListener*)this;
  } else {
   return Service::queryInterface(riid, ppvInterface);
  } 
  addRef(); 
  return StatusCode::SUCCESS; 
}


StatusCode TrigROBDataProviderSvc_RTT::initialize()
{
  StatusCode sc = TrigROBDataProviderSvc::initialize();
  
  // fill the list of ROBs from L1 that must not be considered
  // logStream() << MSG::INFO << "  ---> TrigROBDataProviderSvc_RTT fill the list of ROBs that must be retained"<< endreq;
  m_l1_ROB_ids.reserve(11);   
  // ROBs from ROIB
  m_l1_ROB_ids.push_back(0x770001);  // CTP
  m_l1_ROB_ids.push_back(0x760001);  // muCTPi
  m_l1_ROB_ids.push_back(0x7300a8);  // Calo cluster RoI
  m_l1_ROB_ids.push_back(0x7300a9);  // Calo cluster RoI
  m_l1_ROB_ids.push_back(0x7300aa);  // Calo cluster RoI
  m_l1_ROB_ids.push_back(0x7300ab);  // Calo cluster RoI
  m_l1_ROB_ids.push_back(0x7500ac);  // Calo Jet/Energy RoI
  m_l1_ROB_ids.push_back(0x7500ad);  // Calo Jet/Energy RoI
  // ROBs from DAQ
  m_l1_ROB_ids.push_back(0x770000);  // CTP    DAQ
  m_l1_ROB_ids.push_back(0x760000);  // muCTPi DAQ
  // preload also L2 result, when available
  m_l1_ROB_ids.push_back(0x7b0000);  // L2 HLT result
  
  // for (std::vector<uint32_t>::iterator it = m_l1_ROB_ids.begin() ; it !=  m_l1_ROB_ids.end(); ++it){
  //   logStream() << MSG::INFO <<" # = "<< std::setw(5)  << " cache id = 0x" << std::hex << (*it) << std::dec << "\n"; 
  // }
  m_callerName = "UNKNOWN";
  m_missingPrefetchingPerEvent=false;

  logStream() << MSG::INFO << " ---> TrigROBDataProviderSvc_RTT = " << name() << " initialize "
	      << " - package version " << PACKAGE_VERSION << endreq ;
  return sc;
}


// void TrigROBDataProviderSvc_RTT::robmapPartialClear()
// { 
//   logStream() << MSG::DEBUG <<"TrigROBDataProviderSvc_RTT::robmapPartialClear: initial total size = " << sizeROBCache() <<endreq;  

//   std::vector<const ROBF*> l1_ROB_fragments;
//   l1_ROB_fragments.reserve(11);
//   m_online_robmap.clear();
//   ROBDataProviderSvc::getROBData(m_l1_ROB_ids, l1_ROB_fragments);

//   // add L1 fragments to map
//   for(std::vector<const ROBF*>::const_iterator it=l1_ROB_fragments.begin(); it != l1_ROB_fragments.end(); ++it) {
//     uint32_t id = (*it)->source_id() ;
//     m_online_robmap[id]= (**it) ;
//   }

//   logStream() << MSG::DEBUG << "TrigROBDataProviderSvc_RTT::robmapPartialClear() size="<<m_online_robmap.size() << endreq;
//   return;
// }


void TrigROBDataProviderSvc_RTT::print(std::ostream& os) const
{
  CALLER_ROBMAP::const_iterator it;
  std::vector <uint32_t> robIds;
  os << "TrigROBDataProviderSvc_RTT::print():\n";
  for (it=  m_caller_robmap.begin(); it!= m_caller_robmap.end(); ++it){
    os << (*it).first << " => ";
    robIds = (*it).second;
    os << "("<<robIds.size()<<")  ";
    for(std::vector<uint32_t>::const_iterator rit = robIds.begin(); rit != robIds.end(); ++rit){
      os << (*rit) <<", ";
    }
    os << '\n';
  }
  os <<"--------------------------------------\n";
  return;
}

std::ostream& operator << (std::ostream& os, const TrigROBDataProviderSvc_RTT& c)
{
  c.print(os);
  return os;
}

void TrigROBDataProviderSvc_RTT::setNextEvent(const std::vector<ROBF>& result) 
{ 
  TrigROBDataProviderSvc::setNextEvent(result) ;
  logStream() << MSG::DEBUG <<"TrigROBDataProviderSvc_RTT::setNextEvent map size is " << m_caller_robmap.size() <<". Now reset it"<< endreq;
  logStream() << MSG::DEBUG << *this << endreq;// print out the content of the map
  m_caller_robmap.clear(); 
  m_callerName = "UNKNOWN";
  m_missingPrefetchingPerEvent=false;
  return;
}


void TrigROBDataProviderSvc_RTT::setNextEvent(const RawEvent* re) 
{ 
  TrigROBDataProviderSvc::setNextEvent(re) ;
  logStream() << MSG::DEBUG << "TrigROBDataProviderSvc_RTT::setNextEvent map size is " << m_caller_robmap.size() <<". Now reset it"<<endreq;
  logStream() << MSG::DEBUG << *this << endreq; // print out the content of the map
  m_caller_robmap.clear(); 
  m_callerName = "UNKNOWN";
  m_missingPrefetchingPerEvent=false;
  return;
}

void TrigROBDataProviderSvc_RTT::setCallerName(std::string callerName)
{
  return TrigROBDataProviderSvc::setCallerName(callerName);
//  // caller name of this method
//   std::string caller_name("UNKNOWN");
//   if (callerName != "UNKNOWN") { 
//     caller_name = callerName;
//   }else{
//     IAlgorithm* alg(0);
//     if ( m_algContextSvc ) {
//       alg = m_algContextSvc->currentAlg();
//       caller_name = (alg ? alg->name() : "<NONE>");
//     }
//   }
//   m_callerName=caller_name; 
//   // logStream() << MSG::DEBUG << "TrigROBDataProviderSvc_RTT::setCallerName    : " << m_callerName <<endreq;
//   return;
}



void TrigROBDataProviderSvc_RTT::addROBData(const std::vector<uint32_t>& robIds, std::string callerName)
{
  //call the base class method
  TrigROBDataProviderSvc:: addROBData(robIds, callerName);

  setCallerName(callerName);
  //logStream() << MSG::DEBUG <<" TrigROBDataProviderSvc_RTT::addROBData(robIds, algo) with AlgoName = "<< m_callerName <<" robIds=" << robIds.size() << endreq;
   
   // fill the algo_map
   if ( m_callerName=="UNKNOWN" ||m_callerName=="<NONE>" ){
     logStream() << MSG::INFO <<"TrigROBDataProviderSvc_RTT::addROBData. Algo name is not set"<< endreq;
   }

   m_caller_robmap.insert(std::pair<std::string, std::vector <uint32_t> >(m_callerName,robIds) );

   //monitor pre-fetched ROBs
   if (m_callerName.find("_pref") != std::string::npos){
     if ( m_hist_prefetchedROBsPerAlgo ) {
       m_hist_prefetchedROBsPerAlgo->Fill(m_callerName.c_str(), robIds.size());
       m_hist_prefetchedROBsPerAlgo->LabelsDeflate("X");
     }
   }
   else  {
     // monitor the pre-declared ROBs
     if ( m_hist_declaredROBsPerAlgo ) {
       m_hist_declaredROBsPerAlgo->Fill(m_callerName.c_str(), robIds.size());
       m_hist_declaredROBsPerAlgo->LabelsDeflate("X");
     }
   }

   return;
}



void TrigROBDataProviderSvc_RTT::getROBData(const std::vector<uint32_t>& robIds, std::vector<const ROBF*>& robFragments,
					    const std::string callerName)

{
 //call the base class method
  TrigROBDataProviderSvc:: getROBData(robIds, robFragments, callerName);

  setCallerName(callerName);
  // logStream() << MSG::DEBUG <<"TrigROBDataProviderSvc_RTT::getROBData. Algo name is " <<m_callerName<< endreq;

  // monitor the requested ROBs
  if ( m_hist_requestedROBsPerAlgo ) {
    m_hist_requestedROBsPerAlgo->Fill(m_callerName.c_str(), robIds.size());
    m_hist_requestedROBsPerAlgo->LabelsDeflate("X");
  }

  // monitor the requests, just one per caller
  if ( m_hist_requestPerAlgo ) {
    m_hist_requestPerAlgo->Fill(m_callerName.c_str(), 1.);
    m_hist_requestPerAlgo->LabelsDeflate("X");
  }
  

  //compare the ROBs retrieved now with the requested ones:
  m_missingRequestedROBsPerCall = 0;
  m_missingRequestedROBsPerCall_pref = 0;


  std::string pref_name = m_callerName + "_pref";
  std::pair <CALLER_ROBMAP::iterator, CALLER_ROBMAP::iterator> algo_rob_it = m_caller_robmap.equal_range(m_callerName);
  std::pair <CALLER_ROBMAP::iterator, CALLER_ROBMAP::iterator> algo_rob_pref_it = m_caller_robmap.equal_range(pref_name);

  logStream() << MSG::DEBUG << "TrigROBDataProviderSvc_RTT::getROBData. Looking for "<<m_callerName<<" "<< robIds.size() <<" ROBIDs "
	      <<" in the map. Size pref="<<m_caller_robmap.count(pref_name) <<" exec= "<< m_caller_robmap.count(m_callerName) <<endreq;

  
  bool found        = false;
  bool found_pref   = false;
  bool found_cached = false;

  std::vector<uint32_t> requested_robIds;
  std::vector<uint32_t> requested_pref_robIds;
  std::vector<uint32_t> cached_robIds;

  // compare the vectors of declared and required ROBIds
  for(std::vector<uint32_t>::const_iterator it = robIds.begin(); it != robIds.end(); ++it){
    found = false;
    found_pref = false;
    found_cached=false;

    // check if ROB is already in cache
    //////////////////////////////////////////////
    std::string cached_caller;
    for (CALLER_ROBMAP::iterator map_it= m_caller_robmap.begin(); map_it!= m_caller_robmap.end(); ++map_it){
      cached_robIds=map_it->second;
      for(std::vector<uint32_t>::const_iterator rit = cached_robIds.begin(); rit != cached_robIds.end(); ++rit){
	if (*rit == *it) {
	  found_cached=true;
	  cached_caller = map_it->first;
	  break;
	}
      }
    }
    //1: search among the predeclared in the prefetching phase
    //////////////////////////////////////////////
    for (CALLER_ROBMAP::iterator iit=algo_rob_pref_it.first; iit!=algo_rob_pref_it.second; ++iit){//loop over the calls of this algo
      requested_pref_robIds = iit->second;
      for(std::vector<uint32_t>::const_iterator rit = requested_pref_robIds.begin(); rit != requested_pref_robIds.end(); ++rit){
	if (*rit == *it) {
	  found_pref=true;
	  break;
	}
      }
    }
    if (!found_pref){
      //serch among the L1 ROBs
      for(std::vector<uint32_t>::const_iterator rit = m_l1_ROB_ids.begin(); rit != m_l1_ROB_ids.end(); ++rit){
	if (*rit == *it) {
	  found_pref=true;
	  break;
	}
      }
    }

    if (!found_pref && found_cached){
      logStream() << MSG::DEBUG <<"getROBData:  algo "<< m_callerName <<" ROB id 0x" << std::hex << (*it) << " was prefecthed by different algorithm " << cached_caller <<  std::dec << endreq;
    }

    if (!found_pref && ! found_cached){
      logStream() << MSG::WARNING <<"getROBData: RTTERRORS algo "<< m_callerName <<" ROB id 0x" << std::hex << (*it) << " NOT predeclared/cached in hte prefetching!"<<  std::dec << endreq;
      if (m_caller_robmap.count(pref_name) != 0 ) {
	// print the already declared ROBSId by this algo
	for (CALLER_ROBMAP::iterator iit=algo_rob_pref_it.first; iit!=algo_rob_pref_it.second; ++iit){//loop over the calls of this algo
	  requested_pref_robIds = iit->second;
	  for(std::vector<uint32_t>::const_iterator rit = requested_pref_robIds.begin(); rit != requested_pref_robIds.end(); ++rit){
	    logStream() << MSG::DEBUG << (*rit) <<", ";
	  }
	  logStream() << MSG::DEBUG << endreq;
	}
      }
      m_missingPrefetchingPerEvent_pref=true;
      m_missingRequestedROBsPerCall_pref++; // count the missing ROBs in this call during the pre-fetching
    }

    //2: search among the predeclared
    //////////////////////////////////////////////
    for (CALLER_ROBMAP::iterator iit=algo_rob_it.first; iit!=algo_rob_it.second; ++iit){//loop over the calls of this algo
      requested_robIds = iit->second;
      for(std::vector<uint32_t>::const_iterator rit = requested_robIds.begin(); rit != requested_robIds.end(); ++rit){
	if (*rit == *it) {
	  found=true;
	  break;
	}
      }
    }

    if (!found){
      //serch among the L1 ROBs
      for(std::vector<uint32_t>::const_iterator rit = m_l1_ROB_ids.begin(); rit != m_l1_ROB_ids.end(); ++rit){
	if (*rit == *it) {
	  found=true;
	  break;
	}
      }
    }

    if (!found && found_cached){
      logStream() << MSG::DEBUG <<"getROBData:  algo "<< m_callerName <<" ROB id 0x" << std::hex << (*it) << " was cached by different algorithm " << cached_caller <<  std::dec << endreq;
    }

    if (!found && !found_cached){
      logStream() << MSG::WARNING <<"getROBData: RTTERRORS algo "<< m_callerName <<" ROB id 0x" << std::hex << (*it) << " NOT predeclared/cached!"<<  std::dec << endreq;
      if (m_caller_robmap.count(m_callerName) != 0 ) {
	// print the already declared ROBSId by this algo
	for (CALLER_ROBMAP::iterator iit=algo_rob_it.first; iit!=algo_rob_it.second; ++iit){//loop over the calls of this algo
	  requested_robIds = iit->second;
	  for(std::vector<uint32_t>::const_iterator rit = requested_pref_robIds.begin(); rit != requested_pref_robIds.end(); ++rit){
	    logStream() << MSG::DEBUG << (*rit) <<", ";
	  }
	  logStream() << MSG::DEBUG<< endreq;
	}
      }
      m_missingPrefetchingPerEvent=true;
      m_missingRequestedROBsPerCall++; // count the missing ROBs in this call
    }
  }


  //monitor: prefetching
  if (m_missingRequestedROBsPerCall_pref>0) logStream() << MSG::WARNING << "getROBData: RTTERRORS TOTAL algo " << m_callerName << " missed to declare "
							<< m_missingRequestedROBsPerCall_pref << " ROBs in the prefetching in this call" << endreq;
  if ( m_hist_missingRequestedROBsPerCall_pref ) {
    m_hist_missingRequestedROBsPerCall_pref->Fill(m_missingRequestedROBsPerCall_pref);
  }

  if ( m_hist_missingRequestedROBsPerAlgo_pref ) {
    m_hist_missingRequestedROBsPerAlgo_pref->Fill(m_callerName.c_str(), m_missingRequestedROBsPerCall_pref);
    m_hist_missingRequestedROBsPerAlgo_pref->LabelsDeflate("X");
  }

  if ( m_hist_missingRequestPerAlgo_pref ) {
    bool nodeclared = (m_missingRequestedROBsPerCall_pref>0 && m_caller_robmap.count( pref_name ) == 0 );
    m_hist_missingRequestPerAlgo_pref->Fill(m_callerName.c_str(), nodeclared);
    m_hist_missingRequestPerAlgo_pref->LabelsDeflate("X");
  }

  //monitor: executiuon
  if (m_missingRequestedROBsPerCall>0) logStream() << MSG::WARNING << "getROBData: RTTERRORS TOTAL algo " << m_callerName << " missed to declare "
						   << m_missingRequestedROBsPerCall << " ROBs in this call" << endreq;
  if ( m_hist_missingRequestedROBsPerCall ) {
    m_hist_missingRequestedROBsPerCall->Fill(m_missingRequestedROBsPerCall);
  }

  if ( m_hist_missingRequestedROBsPerAlgo ) {
    m_hist_missingRequestedROBsPerAlgo->Fill(m_callerName.c_str(), m_missingRequestedROBsPerCall);
    m_hist_missingRequestedROBsPerAlgo->LabelsDeflate("X");
  }

  if ( m_hist_missingRequestPerAlgo ) {
    bool nodeclared = (m_missingRequestedROBsPerCall>0 && m_caller_robmap.count(m_callerName) == 0); 
    m_hist_missingRequestPerAlgo->Fill(m_callerName.c_str(), nodeclared);
    m_hist_missingRequestPerAlgo->LabelsDeflate("X");
  }

  return;
}



// handler for BeginRun

void TrigROBDataProviderSvc_RTT::handle(const Incident& incident) 
{
  //base class method
  TrigROBDataProviderSvc::handle(incident);

  if (incident.type()!="BeginRun") return;
  if(logLevel() <= MSG::DEBUG)
    logStream() <<MSG::DEBUG << "In BeginRun incident." << endreq;
  
  // if detailed ROB monitoring is requested, check if the AlgContextSvc is running, 
  // if yes use it to obtain the calling algorithm name
  if ( m_doDetailedROBMonitoring.value() ) {
    if ( service("AlgContextSvc", m_algContextSvc, /*createIf=*/ false).isFailure() ) {
      logStream() << MSG::ERROR << "Error retrieving AlgContextSvc."  
    		  << "Calling algorithm name not available in detailed ROB monitoring" << endreq;
      m_algContextSvc=0;
    }
  }
  
  // define histograms if monitoring is requested
  if ( !m_doMonitoring.value() ) return;
      
  // find histogramming service
  ServiceHandle<ITHistSvc> rootHistSvc("THistSvc", name());
  if ((rootHistSvc.retrieve()).isFailure()) {
    logStream() << MSG::ERROR << "Unable to locate THistSvc" << endreq;
    rootHistSvc.release().ignore();
    return;
  }


  // *-- booking path
  std::string path = std::string("/EXPERT/")+getGaudiThreadGenericName(name())+"/";
  //
  m_hist_missingRequestedROBsPerAlgo = new TH1F (m_histProp_missingRequestedROBsPerAlgo.value().title().c_str(),
						 (m_histProp_missingRequestedROBsPerAlgo.value().title()+";").c_str(), 
						 m_histProp_missingRequestedROBsPerAlgo.value().bins(), 
						 m_histProp_missingRequestedROBsPerAlgo.value().lowEdge(), 
						 m_histProp_missingRequestedROBsPerAlgo.value().highEdge());
  if (m_hist_missingRequestedROBsPerAlgo) {
    m_hist_missingRequestedROBsPerAlgo->SetBit(TH1::kCanRebin);
    if( rootHistSvc->regHist(path + m_hist_missingRequestedROBsPerAlgo->GetName(), m_hist_missingRequestedROBsPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerAlgo->GetName() << endreq;
    }
  }

  //
  m_hist_missingRequestPerAlgo = new TH1F (m_histProp_missingRequestPerAlgo.value().title().c_str(),
						 (m_histProp_missingRequestPerAlgo.value().title()+";").c_str(), 
						 m_histProp_missingRequestPerAlgo.value().bins(), 
						 m_histProp_missingRequestPerAlgo.value().lowEdge(), 
						 m_histProp_missingRequestPerAlgo.value().highEdge());
  if (m_hist_missingRequestPerAlgo) {
    m_hist_missingRequestPerAlgo->SetBit(TH1::kCanRebin);
    if( rootHistSvc->regHist(path + m_hist_missingRequestPerAlgo->GetName(), m_hist_missingRequestPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestPerAlgo->GetName() << endreq;
    }
  }

 //
  m_hist_requestPerAlgo = new TH1F (m_histProp_requestPerAlgo.value().title().c_str(),
						 (m_histProp_requestPerAlgo.value().title()+";").c_str(), 
						 m_histProp_requestPerAlgo.value().bins(), 
						 m_histProp_requestPerAlgo.value().lowEdge(), 
						 m_histProp_requestPerAlgo.value().highEdge());
  if (m_hist_requestPerAlgo) {
    m_hist_requestPerAlgo->SetBit(TH1::kCanRebin);
    if( rootHistSvc->regHist(path + m_hist_requestPerAlgo->GetName(), m_hist_requestPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_requestPerAlgo->GetName() << endreq;
    }
  }

 //
  m_hist_missingRequestedROBsPerCall = new TH1F (m_histProp_missingRequestedROBsPerCall.value().title().c_str(),
						 (m_histProp_missingRequestedROBsPerCall.value().title()+";Number of non-predeclared ROBs").c_str(), 
						 m_histProp_missingRequestedROBsPerCall.value().bins(), 
						 m_histProp_missingRequestedROBsPerCall.value().lowEdge(), 
						 m_histProp_missingRequestedROBsPerCall.value().highEdge());
  if (m_hist_missingRequestedROBsPerCall) {
    if( rootHistSvc->regHist(path + m_hist_missingRequestedROBsPerCall->GetName(), m_hist_missingRequestedROBsPerCall).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerCall->GetName() << endreq;
    }
  }

  //
  m_hist_requestedROBsPerAlgo = new TH1F (m_histProp_requestedROBsPerAlgo.value().title().c_str(),
					  (m_histProp_requestedROBsPerAlgo.value().title()+";").c_str(), 
					  m_histProp_requestedROBsPerAlgo.value().bins(), 
					  m_histProp_requestedROBsPerAlgo.value().lowEdge(), 
					  m_histProp_requestedROBsPerAlgo.value().highEdge());
  if (m_hist_requestedROBsPerAlgo) {
    m_hist_requestedROBsPerAlgo->SetBit(TH1::kCanRebin);
    if( rootHistSvc->regHist(path + m_hist_requestedROBsPerAlgo->GetName(), m_hist_requestedROBsPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_requestedROBsPerAlgo->GetName() << endreq;
    }
  }

 //
  m_hist_missingRequestedROBsPerAlgo_pref = new TH1F (m_histProp_missingRequestedROBsPerAlgo_pref.value().title().c_str(),
						 (m_histProp_missingRequestedROBsPerAlgo_pref.value().title()+";").c_str(), 
						 m_histProp_missingRequestedROBsPerAlgo_pref.value().bins(), 
						 m_histProp_missingRequestedROBsPerAlgo_pref.value().lowEdge(), 
						 m_histProp_missingRequestedROBsPerAlgo_pref.value().highEdge());
  if (m_hist_missingRequestedROBsPerAlgo_pref) {
    m_hist_missingRequestedROBsPerAlgo_pref->SetBit(TH1::kCanRebin);
    if( rootHistSvc->regHist(path + m_hist_missingRequestedROBsPerAlgo_pref->GetName(), m_hist_missingRequestedROBsPerAlgo_pref).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerAlgo_pref->GetName() << endreq;
    }
  }

  //
  m_hist_missingRequestPerAlgo_pref = new TH1F (m_histProp_missingRequestPerAlgo_pref.value().title().c_str(),
						 (m_histProp_missingRequestPerAlgo_pref.value().title()+";").c_str(), 
						 m_histProp_missingRequestPerAlgo_pref.value().bins(), 
						 m_histProp_missingRequestPerAlgo_pref.value().lowEdge(), 
						 m_histProp_missingRequestPerAlgo_pref.value().highEdge());
  if (m_hist_missingRequestPerAlgo_pref) {
    m_hist_missingRequestPerAlgo_pref->SetBit(TH1::kCanRebin);
    if( rootHistSvc->regHist(path + m_hist_missingRequestPerAlgo_pref->GetName(), m_hist_missingRequestPerAlgo_pref).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestPerAlgo_pref->GetName() << endreq;
    }
  }

 //
  m_hist_missingRequestedROBsPerCall_pref = new TH1F (m_histProp_missingRequestedROBsPerCall_pref.value().title().c_str(),
						 (m_histProp_missingRequestedROBsPerCall_pref.value().title()+";Number of non-predeclared ROBs").c_str(), 
						 m_histProp_missingRequestedROBsPerCall_pref.value().bins(), 
						 m_histProp_missingRequestedROBsPerCall_pref.value().lowEdge(), 
						 m_histProp_missingRequestedROBsPerCall_pref.value().highEdge());
  if (m_hist_missingRequestedROBsPerCall_pref) {
    if( rootHistSvc->regHist(path + m_hist_missingRequestedROBsPerCall_pref->GetName(), m_hist_missingRequestedROBsPerCall_pref).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerCall_pref->GetName() << endreq;
    }
  }

  //
 
  m_hist_declaredROBsPerAlgo = new TH1F (m_histProp_declaredROBsPerAlgo.value().title().c_str(),
					  (m_histProp_declaredROBsPerAlgo.value().title()+";").c_str(), 
					  m_histProp_declaredROBsPerAlgo.value().bins(), 
					  m_histProp_declaredROBsPerAlgo.value().lowEdge(), 
					  m_histProp_declaredROBsPerAlgo.value().highEdge());
  if (m_hist_declaredROBsPerAlgo) {
    m_hist_declaredROBsPerAlgo->SetBit(TH1::kCanRebin);
    if( rootHistSvc->regHist(path + m_hist_declaredROBsPerAlgo->GetName(), m_hist_declaredROBsPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_declaredROBsPerAlgo->GetName() << endreq;
    }
  }

  //
  m_hist_prefetchedROBsPerAlgo = new TH1F (m_histProp_prefetchedROBsPerAlgo.value().title().c_str(),
					  (m_histProp_prefetchedROBsPerAlgo.value().title()+";").c_str(), 
					  m_histProp_prefetchedROBsPerAlgo.value().bins(), 
					  m_histProp_prefetchedROBsPerAlgo.value().lowEdge(), 
					  m_histProp_prefetchedROBsPerAlgo.value().highEdge());
  if (m_hist_prefetchedROBsPerAlgo) {
    m_hist_prefetchedROBsPerAlgo->SetBit(TH1::kCanRebin);
    if( rootHistSvc->regHist(path + m_hist_prefetchedROBsPerAlgo->GetName(), m_hist_prefetchedROBsPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_prefetchedROBsPerAlgo->GetName() << endreq;
    }
  }

  // release histogramming service
  rootHistSvc.release().ignore();
}

