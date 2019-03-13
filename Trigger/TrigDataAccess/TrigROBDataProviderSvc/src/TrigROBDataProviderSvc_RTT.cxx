/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigROBDataProviderSvc_RTT.h"

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

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) 
#   define CAN_REBIN(hist)  hist->SetCanExtend(TH1::kAllAxes)
#else
#   define CAN_REBIN(hist)  hist->SetBit(TH1::kCanRebin)
#endif


TrigROBDataProviderSvc_RTT::TrigROBDataProviderSvc_RTT(const std::string& name, ISvcLocator* svcloc)
  : TrigROBDataProviderSvc(name,svcloc),
    m_enablePrefetchingAtAlgoLevel(true),
    // hit properties
    m_histProp_requestedROBsPerAlgo(Gaudi::Histo1DDef("RequestedROBsPerAlgo" ,0.,1.,1)),
    m_histProp_requestPerAlgo(Gaudi::Histo1DDef("RequestPerAlgo" ,0.,1.,1)), 
    m_histProp_callerPerEvent(Gaudi::Histo1DDef("CallerPerEvent" ,0.,1.,1)), 
    m_histProp_nocachedRequestedROBsPerAlgo(Gaudi::Histo1DDef("NoCachedRequestedROBsPerAlgo" ,0.,1.,1)),
    m_histProp_nocachedRequestPerAlgo(Gaudi::Histo1DDef("NoCachedRequestPerAlgo" ,0.,1.,1)),      
    m_histProp_declaredROBsPerAlgo(Gaudi::Histo1DDef("DeclaredROBsPerAlgo" ,0.,1.,1)),
    m_histProp_prefetchedROBsPerAlgo(Gaudi::Histo1DDef("PrefetchedROBsPerAlgo" ,0.,1.,1)),
    m_histProp_missingRequestedROBsPerAlgo(Gaudi::Histo1DDef("MissingRequestedROBsPerAlgo" ,0.,1.,1)),
    m_histProp_missingRequestPerAlgo(Gaudi::Histo1DDef("MissingRequestPerAlgo" ,0.,1.,1)),
    m_histProp_missingRequestedROBsPerCall(Gaudi::Histo1DDef("MissingRequestedROBsPerCall" ,0.,100.,100)),
    m_histProp_missingRequestedROBsPerAlgo_pref(Gaudi::Histo1DDef("MissingRequestedROBsPerAlgo_pref" ,0.,1.,1)),
    m_histProp_missingRequestPerAlgo_pref(Gaudi::Histo1DDef("MissingRequestPerAlgo_pref" ,0.,1.,1)),
    m_histProp_missingRequestedROBsPerCall_pref(Gaudi::Histo1DDef("MissingRequestedROBsPerCall_pref" ,0.,100.,100)),
    m_histProp_missingRequestedROBsPerAlgoButCached(Gaudi::Histo1DDef("MissingRequestedROBsPerAlgoButCached" ,0.,1.,1)),
    m_histProp_missingRequestPerAlgoButCached(Gaudi::Histo1DDef("MissingRequestPerAlgoButCached" ,0.,1.,1)),
    m_histProp_missingRequestedROBsPerCallButCached(Gaudi::Histo1DDef("MissingRequestedROBsPerCallButCached" ,0.,100.,100)),
    m_histProp_missingRequestedROBsPerAlgoButCached_pref(Gaudi::Histo1DDef("MissingRequestedROBsPerAlgoButCached_pref" ,0.,1.,1)),
    m_histProp_missingRequestPerAlgoButCached_pref(Gaudi::Histo1DDef("MissingRequestPerAlgoButCached_pref" ,0.,1.,1)),
    m_histProp_missingRequestedROBsPerCallButCached_pref(Gaudi::Histo1DDef("MissingRequestedROBsPerCallButCached_pref" ,0.,100.,100)),

    //hist init
    m_hist_requestedROBsPerAlgo(0),
    m_hist_requestedROBsPerCallPerAlgo(0),
    m_hist_requestPerAlgo(0),
    m_hist_callerPerEvent(0),
    m_hist_nocachedRequestedROBsPerAlgo(0),
    m_hist_nocachedRequestPerAlgo(0),
    m_hist_declaredROBsPerAlgo(0),
    m_hist_prefetchedROBsPerAlgo(0),
    m_hist_missingRequestedROBsPerAlgo(0),
    m_hist_missingRequestPerAlgo(0),
    m_hist_missingRequestedROBsPerCall(0),
    m_hist_missingRequestedROBsPerAlgo_pref(0),
    m_hist_missingRequestPerAlgo_pref(0),
    m_hist_missingRequestedROBsPerCall_pref(0),
    m_hist_missingRequestedROBsPerAlgoButCached(0),
    m_hist_missingRequestPerAlgoButCached(0),
    m_hist_missingRequestedROBsPerCallButCached(0),
    m_hist_missingRequestedROBsPerAlgoButCached_pref(0),
    m_hist_missingRequestPerAlgoButCached_pref(0),
    m_hist_missingRequestedROBsPerCallButCached_pref(0)

 {
  declareProperty("enablePrefetchingAtAlgoLevel", m_enablePrefetchingAtAlgoLevel=true, "Enables pre-fetching at algorithm level");
  declareProperty("HistMissingRequestedROBsPerCall", m_histProp_missingRequestedROBsPerCall,"Number of missing ROBs pre-requests per call");
  declareProperty("HistMissingRequestedROBsPerAlgo", m_histProp_missingRequestedROBsPerAlgo,"Number of missing ROBs pre-requests per algo");
  declareProperty("HistMissingRequestPerAlgo", m_histProp_missingRequestPerAlgo,"Number of missing pre-requests per algo");
  declareProperty("HistMissingRequestedROBsPerCall_pref", m_histProp_missingRequestedROBsPerCall_pref,"Number of missing ROBs pre-fetches per call");
  declareProperty("HistMissingRequestedROBsPerAlgo_pref", m_histProp_missingRequestedROBsPerAlgo_pref,"Number of missing ROBs pre-fetches per algo");
  declareProperty("HistMissingRequestPerAlgo_pref", m_histProp_missingRequestPerAlgo_pref,"Number of missing pre-fetches per algo");
  declareProperty("HistRequestedROBsPerAlgo", m_histProp_requestedROBsPerAlgo,"Number of requested ROBs per algo");
  declareProperty("HistRequestPerAlgo", m_histProp_requestPerAlgo,"Number of ROB requests per algo");
  declareProperty("HistCallerPerEvent", m_histProp_callerPerEvent,"Number of events the Algo requests ROBs");
  declareProperty("HistNoCachedRequestPerAlgo", m_histProp_nocachedRequestPerAlgo,"Number of no-cached ROB requests per algo");
  declareProperty("HistNoCachedRequestedROBsPerAlgo", m_histProp_nocachedRequestedROBsPerAlgo,"Number of no-cached requested ROBs per algo");
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

  logStream() << MSG::INFO << " ---> TrigROBDataProviderSvc_RTT = " << name() << " initialize "
	      << " - package version " << PACKAGE_VERSION << endmsg ;
  return sc;
}


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
  logStream() << MSG::DEBUG <<"TrigROBDataProviderSvc_RTT::setNextEvent map size is " << m_caller_robmap.size() <<". Now reset it"<< endmsg;
  logStream() << MSG::DEBUG << *this << endmsg;// print out the content of the map
  FillEvent();
  m_caller_robmap.clear(); 
  m_callerName = "UNKNOWN";
  return;
}


void TrigROBDataProviderSvc_RTT::setNextEvent(const RawEvent* re) 
{ 
  TrigROBDataProviderSvc::setNextEvent(re) ;
  logStream() << MSG::DEBUG << "TrigROBDataProviderSvc_RTT::setNextEvent map size is " << m_caller_robmap.size() <<". Now reset it"<<endmsg;
  logStream() << MSG::DEBUG << *this << endmsg; // print out the content of the map
  FillEvent();
  m_caller_robmap.clear(); 
  m_callerName = "UNKNOWN";
  return;
}


void TrigROBDataProviderSvc_RTT::FillEvent(){
  // fill the caller per event
 if (!m_caller_robmap.empty()){ 
    std::string caller;
    for (CALLER_ROBMAP::iterator map_it= m_caller_robmap.begin(); map_it!= m_caller_robmap.end(); ++map_it){
      caller=map_it->first;
      m_hist_callerPerEvent->Fill(caller.c_str(),1);
      m_hist_callerPerEvent->LabelsDeflate("X");
      //      std::cout <<"FPP setNextEvent lvl1="<< m_currentLvl1ID <<": caller "<< caller <<" executed in this event with ROBId size=" << (map_it->second).size()<<std::endl;
    } 
  }
}

void TrigROBDataProviderSvc_RTT::setCallerName(std::string_view callerName)
{
  return TrigROBDataProviderSvc::setCallerName(callerName);
}



void TrigROBDataProviderSvc_RTT::addROBData(const std::vector<uint32_t>& robIds, std::string_view callerName)
{
  //call the base class method
  TrigROBDataProviderSvc:: addROBData(robIds, callerName);
  
  std::vector<uint32_t> all_robIds;    
  //if the caller already declared other ROBs, just retrieve them and update uniquely
   if (robIds.size() != 0){
     CALLER_ROBMAP::iterator map_it =  m_caller_robmap.find(m_callerName) ;
     //not found (new caller)
     if(map_it == m_caller_robmap.end())   {
       m_caller_robmap.insert(std::pair<std::string, std::vector <uint32_t> >(m_callerName,robIds) );
     }
     else{
       all_robIds = (*map_it).second; 
       all_robIds.insert (all_robIds.end(), robIds.begin(), robIds.end());
     
       std::vector<uint32_t>::iterator remove_duplicate;
       sort(all_robIds.begin(), all_robIds.end()); 
       remove_duplicate = unique(all_robIds.begin(), all_robIds.end()); 
       all_robIds.erase(remove_duplicate, all_robIds.end());
       
     // replace the list of ROBs
       map_it->second = all_robIds;
     //
     }
   }

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
					    const std::string_view callerName)

{
  //call the base class method
  TrigROBDataProviderSvc:: getROBData(robIds, robFragments, callerName);

  // skip L1 ROBIds
  unsigned int foundL1=0;
  for(std::vector<uint32_t>::const_iterator it = robIds.begin(); it != robIds.end(); ++it){
    if (std::find(m_l1_ROB_ids.begin(), m_l1_ROB_ids.end(), (*it))!=m_l1_ROB_ids.end()) {
      foundL1++;
    }
  }
  if (robIds.size() == foundL1) return;


  if (foundL1 !=0)
    logStream() << MSG::WARNING <<"TrigROBDataProviderSvc_RTT::getROBData. Algo " <<m_callerName<<" called some L1 ROBIds. Is this an error?" <<endmsg;

  // monitor the requested ROBs
  if ( m_hist_requestedROBsPerAlgo ) {
    m_hist_requestedROBsPerAlgo->Fill(m_callerName.c_str(), robIds.size());
    m_hist_requestedROBsPerAlgo->LabelsDeflate("X");
  }

  // monitor the requested ROBs per Algo
  if ( m_hist_requestedROBsPerCallPerAlgo ) {
    m_hist_requestedROBsPerCallPerAlgo->Fill(m_callerName.c_str(), robIds.size(), 1.);
    m_hist_requestedROBsPerCallPerAlgo->LabelsDeflate("X");
  }

  // monitor the requests, just one per caller
  if ( m_hist_requestPerAlgo ) {
    m_hist_requestPerAlgo->Fill(m_callerName.c_str(), 1.);
    m_hist_requestPerAlgo->LabelsDeflate("X");
  }
  

  //compare the ROBs retrieved now with the requested ones:
  int missingRequestedROBsPerCall = 0;
  int missingRequestedROBsPerCall_pref = 0;
  int missingRequestedROBsPerCallButCached = 0;
  int missingRequestedROBsPerCallButCached_pref = 0;


  std::string pref_name = m_callerName + "_pref";
  std::pair <CALLER_ROBMAP::iterator, CALLER_ROBMAP::iterator> algo_rob_it = m_caller_robmap.equal_range(m_callerName);
  std::pair <CALLER_ROBMAP::iterator, CALLER_ROBMAP::iterator> algo_rob_pref_it = m_caller_robmap.equal_range(pref_name);

  logStream() << MSG::DEBUG << "TrigROBDataProviderSvc_RTT::getROBData. ROBs of algo "<<m_callerName<<": requested "<< robIds.size() <<" ROBIDs "
	      <<". Found "      << ( (m_caller_robmap.count(pref_name)>0)   ? (m_caller_robmap.find(pref_name)->second).size() : 0 )
  	      <<" pre-fetched, "<< ( (m_caller_robmap.count(m_callerName)>0)? (m_caller_robmap.find(m_callerName)->second).size(): 0) << " pre-declared" << endmsg;

  
  bool found        = false;
  bool found_pref   = false;
  bool found_cached = false;
  bool found_another_pref = false;

  std::vector<uint32_t> requested_robIds;
  std::vector<uint32_t> requested_pref_robIds;
  std::vector<uint32_t> cached_robIds;

  std::vector<uint32_t> not_prefetched_robIds;
  std::vector<uint32_t> not_predeclared_robIds;

  // compare the vectors of declared and required ROBIds
  for(std::vector<uint32_t>::const_iterator it = robIds.begin(); it != robIds.end(); ++it){

    // eclude L1 ROBs, may be removed
    if (std::find(m_l1_ROB_ids.begin(), m_l1_ROB_ids.end(), (*it))!=m_l1_ROB_ids.end()) {
      continue;
    }

    found = false;
    found_pref = false;
    found_cached=false;
    found_another_pref=false;

    // check if ROBId is already in cache
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

    // monitor not cached ROBIds
    if ( m_hist_nocachedRequestedROBsPerAlgo ) {
      m_hist_nocachedRequestedROBsPerAlgo->Fill(m_callerName.c_str(), (!found_cached)*robIds.size());
      m_hist_nocachedRequestedROBsPerAlgo->LabelsDeflate("X");
    }

    if ( m_hist_nocachedRequestPerAlgo ) {
      m_hist_nocachedRequestPerAlgo->Fill(m_callerName.c_str(), (!found_cached));
      m_hist_nocachedRequestPerAlgo->LabelsDeflate("X");
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


    if (!found_pref) {
      // search if another algorithm has predeclared it:
      CALLER_ROBMAP::const_iterator callit;
      std::vector <uint32_t> robIds_pre;
      std::string prefetching_caller="";
      for (callit=  m_caller_robmap.begin(); callit!= m_caller_robmap.end(); ++callit){
	prefetching_caller = (*callit).first;
	if (prefetching_caller.find("_pref") == std::string::npos) continue;// only prefetching algo
	//	if (m_callerName == prefetching_caller) continue;//not needed
	robIds_pre = (*callit).second;
	if (std::find(robIds_pre.begin(), robIds_pre.end(), (*it))!=robIds_pre.end()) {
	  found_another_pref=true;
	  break;
	}
      }

      if ((found_another_pref) && (m_callerName != prefetching_caller)){
	missingRequestedROBsPerCallButCached_pref++;// count the missing ROBs in this call during the pre-fetching, but data are cached by other algorithm
	logStream() << MSG::DEBUG <<"TrigROBDataProviderSvc_RTT::getROBData:  algo "<< m_callerName <<" ROB id 0x" << std::hex << (*it) << " was NOT predeclared in the PRE-FETCHING, but pre-declared by different algorithm " << prefetching_caller <<  std::dec << endmsg;
      }
      else {// this is the error
	// count the missing ROBs in this call during the pre-fetching, not cached or cached by this algo (meaning that the algo retrieved data without prefcthing)
	not_prefetched_robIds.push_back(*it);
	missingRequestedROBsPerCall_pref++;
	logStream() << MSG::DEBUG <<"TrigROBDataProviderSvc_RTT::getROBData: RTTerrors algo "<< m_callerName <<" ROB id 0x" << std::hex << (*it) << " was NOT predeclared in the PRE-FETCHING!"<<  std::dec << endmsg;
      }
      // check if the same algo prefetcehd any ROBIds
      if (m_caller_robmap.count(pref_name) != 0 ) {
	// print the already declared ROBSId by this algo
	logStream() << MSG::DEBUG <<"TrigROBDataProviderSvc_RTT::getROBData:  algo "<< m_callerName <<" predeclared in pre-fetching these ROBIds:" << endmsg;
	for (CALLER_ROBMAP::iterator iit=algo_rob_pref_it.first; iit!=algo_rob_pref_it.second; ++iit){//loop over the calls of this algo
	  requested_pref_robIds = iit->second;
	  for(std::vector<uint32_t>::const_iterator rit = requested_pref_robIds.begin(); rit != requested_pref_robIds.end(); ++rit){
	    logStream() << MSG::DEBUG << (*rit) <<", ";
	  }
	  logStream() << MSG::DEBUG << endmsg;
	}
      }
    }

    //2: search among the predeclared during the execution
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
 
    if (!found  && !found_pref){
      if (found_cached && ( m_callerName != cached_caller)){
	missingRequestedROBsPerCallButCached++;
	logStream() << MSG::DEBUG <<"TrigROBDataProviderSvc_RTT::getROBData:  algo "<< m_callerName <<" ROB id 0x" << std::hex << (*it) << " was NOT predeclared, but cached by different algorithm " << cached_caller <<  std::dec << endmsg;
      }
      else {
	not_predeclared_robIds.push_back(*it);
	missingRequestedROBsPerCall++; // count the missing ROBs in this call, not cached or cached by the same algo
	if (found_cached && ( m_callerName == cached_caller)){	 
	  logStream() << MSG::DEBUG <<"TrigROBDataProviderSvc_RTT::getROBData:  algo "<< m_callerName <<" ROB id 0x" << std::hex << (*it) << " was NOT predeclared, but cached "  <<  std::dec << endmsg;
	}
	else {
	  logStream() << MSG::DEBUG <<"TrigROBDataProviderSvc_RTT::getROBData:  algo "<< m_callerName <<" ROB id 0x" << std::hex << (*it) << " NOT predeclared (and not cached)!"<<  std::dec << endmsg;
	}
      }
      if (m_caller_robmap.count(m_callerName) != 0 ) {
	// print the already declared ROBSId by this algo
	logStream() << MSG::DEBUG <<"getROBData:  algo "<< m_callerName <<" predeclared these ROBIds:" << endmsg;
	for (CALLER_ROBMAP::iterator iit=algo_rob_it.first; iit!=algo_rob_it.second; ++iit){//loop over the calls of this algo
	  requested_robIds = iit->second;
	  for(std::vector<uint32_t>::const_iterator rit = requested_pref_robIds.begin(); rit != requested_pref_robIds.end(); ++rit){
	    logStream() << MSG::DEBUG << (*rit) <<", ";
	  }
	  logStream() << MSG::DEBUG<< endmsg;
	}
      }

    }
  }


  //monitor: prefetching
  if (missingRequestedROBsPerCall_pref>0){
    logStream() << MSG::WARNING << "getROBData: ---> RTTerrors in the PRE-FETCHING: algo " << m_callerName << " missed to declare "
						      << missingRequestedROBsPerCall_pref << " ROBs in this call,  " 
						      << missingRequestedROBsPerCallButCached_pref <<" ROBs are  declared by other algorithm"<< endmsg;
    logStream() << MSG::DEBUG <<"getROBData: --->           in the PRE-FETCHING: These are the missing ROBS: "<< std::hex;
    for(std::vector<uint32_t>::const_iterator rit = not_prefetched_robIds.begin(); rit != not_prefetched_robIds.end(); ++rit){
      logStream() << MSG::DEBUG << "0x"  << (*rit)  <<", ";
    } 
    logStream() << MSG::DEBUG << std::dec <<endmsg;
  }

  if ( m_hist_missingRequestedROBsPerCall_pref ) {
    m_hist_missingRequestedROBsPerCall_pref->Fill(missingRequestedROBsPerCall_pref);
  }

  if ( m_hist_missingRequestedROBsPerAlgo_pref ) {
    m_hist_missingRequestedROBsPerAlgo_pref->Fill(m_callerName.c_str(), missingRequestedROBsPerCall_pref);
    m_hist_missingRequestedROBsPerAlgo_pref->LabelsDeflate("X");
  }

  if ( m_hist_missingRequestPerAlgo_pref ) {
    bool nodeclared = (missingRequestedROBsPerCall_pref>0 && m_caller_robmap.count( pref_name ) == 0 );
    m_hist_missingRequestPerAlgo_pref->Fill(m_callerName.c_str(), nodeclared);
    m_hist_missingRequestPerAlgo_pref->LabelsDeflate("X");
  }

  //monitor: prefetching, with caching
 if ( m_hist_missingRequestedROBsPerCallButCached_pref ) {
    m_hist_missingRequestedROBsPerCallButCached_pref->Fill(missingRequestedROBsPerCallButCached_pref);
  }

  if ( m_hist_missingRequestedROBsPerAlgoButCached_pref ) {
    m_hist_missingRequestedROBsPerAlgoButCached_pref->Fill(m_callerName.c_str(), missingRequestedROBsPerCallButCached_pref);
    m_hist_missingRequestedROBsPerAlgoButCached_pref->LabelsDeflate("X");
  }

  if ( m_hist_missingRequestPerAlgoButCached_pref ) {
    bool nodeclared = (missingRequestedROBsPerCallButCached_pref>0 && m_caller_robmap.count( pref_name ) == 0 );
    m_hist_missingRequestPerAlgoButCached_pref->Fill(m_callerName.c_str(), nodeclared);
    m_hist_missingRequestPerAlgoButCached_pref->LabelsDeflate("X");
  }

  //monitor: executiuon
  if (missingRequestedROBsPerCall>0) {
    logStream() << MSG::WARNING << "getROBData: ---> RTTerrors in the EXECUTION: algo " << m_callerName << " missed to declare "
		<< missingRequestedROBsPerCall << " ROBs in this call , " 
		<< missingRequestedROBsPerCallButCached <<" ROBs are not declared, but cached by other algorithm "<< endmsg;
    logStream() << MSG::DEBUG <<"getROBData: --->           in the EXECUTION: These are the missing ROBS: "<< std::hex;
    for(std::vector<uint32_t>::const_iterator rit = not_predeclared_robIds.begin(); rit != not_predeclared_robIds.end(); ++rit){
      logStream() << MSG::DEBUG << "0x"<<(*rit) <<", ";
    } 
    logStream() << MSG::DEBUG <<std::dec <<endmsg;
  }
  if ( m_hist_missingRequestedROBsPerCall ) {
    m_hist_missingRequestedROBsPerCall->Fill(missingRequestedROBsPerCall);
  }

  if ( m_hist_missingRequestedROBsPerAlgo ) {
    m_hist_missingRequestedROBsPerAlgo->Fill(m_callerName.c_str(), missingRequestedROBsPerCall);
    m_hist_missingRequestedROBsPerAlgo->LabelsDeflate("X");
  }

  if ( m_hist_missingRequestPerAlgo ) {
    bool nodeclared = (missingRequestedROBsPerCall>0 && m_caller_robmap.count(m_callerName) == 0); 
    m_hist_missingRequestPerAlgo->Fill(m_callerName.c_str(), nodeclared);
    m_hist_missingRequestPerAlgo->LabelsDeflate("X");
  }

  //monitor execution, cached
 if ( m_hist_missingRequestedROBsPerCallButCached ) {
    m_hist_missingRequestedROBsPerCallButCached->Fill(missingRequestedROBsPerCallButCached);
  }

  if ( m_hist_missingRequestedROBsPerAlgoButCached ) {
    m_hist_missingRequestedROBsPerAlgoButCached->Fill(m_callerName.c_str(), missingRequestedROBsPerCallButCached);
    m_hist_missingRequestedROBsPerAlgoButCached->LabelsDeflate("X");
  }

  if ( m_hist_missingRequestPerAlgoButCached ) {
    bool nodeclared = (missingRequestedROBsPerCallButCached>0 && m_caller_robmap.count(m_callerName) == 0); 
    m_hist_missingRequestPerAlgoButCached->Fill(m_callerName.c_str(), nodeclared);
    m_hist_missingRequestPerAlgoButCached->LabelsDeflate("X");
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
  //
  m_hist_missingRequestedROBsPerAlgo = new TH1F (m_histProp_missingRequestedROBsPerAlgo.value().title().c_str(),
						 (m_histProp_missingRequestedROBsPerAlgo.value().title()+";").c_str(), 
						 m_histProp_missingRequestedROBsPerAlgo.value().bins(), 
						 m_histProp_missingRequestedROBsPerAlgo.value().lowEdge(), 
						 m_histProp_missingRequestedROBsPerAlgo.value().highEdge());
  if (m_hist_missingRequestedROBsPerAlgo) {
    CAN_REBIN(m_hist_missingRequestedROBsPerAlgo);
    if( rootHistSvc->regHist(path + m_hist_missingRequestedROBsPerAlgo->GetName(), m_hist_missingRequestedROBsPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerAlgo->GetName() << endmsg;
    }
  }

  //
  m_hist_missingRequestPerAlgo = new TH1F (m_histProp_missingRequestPerAlgo.value().title().c_str(),
						 (m_histProp_missingRequestPerAlgo.value().title()+";").c_str(), 
						 m_histProp_missingRequestPerAlgo.value().bins(), 
						 m_histProp_missingRequestPerAlgo.value().lowEdge(), 
						 m_histProp_missingRequestPerAlgo.value().highEdge());
  if (m_hist_missingRequestPerAlgo) {
    CAN_REBIN(m_hist_missingRequestPerAlgo);
    if( rootHistSvc->regHist(path + m_hist_missingRequestPerAlgo->GetName(), m_hist_missingRequestPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestPerAlgo->GetName() << endmsg;
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
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerCall->GetName() << endmsg;
    }
  }


  //
  m_hist_missingRequestedROBsPerAlgoButCached = new TH1F (m_histProp_missingRequestedROBsPerAlgoButCached.value().title().c_str(),
						 (m_histProp_missingRequestedROBsPerAlgoButCached.value().title()+";").c_str(), 
						 m_histProp_missingRequestedROBsPerAlgoButCached.value().bins(), 
						 m_histProp_missingRequestedROBsPerAlgoButCached.value().lowEdge(), 
						 m_histProp_missingRequestedROBsPerAlgoButCached.value().highEdge());
  if (m_hist_missingRequestedROBsPerAlgoButCached) {
    CAN_REBIN(m_hist_missingRequestedROBsPerAlgoButCached);
    if( rootHistSvc->regHist(path + m_hist_missingRequestedROBsPerAlgoButCached->GetName(), m_hist_missingRequestedROBsPerAlgoButCached).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerAlgoButCached->GetName() << endmsg;
    }
  }

  //
  m_hist_missingRequestPerAlgoButCached = new TH1F (m_histProp_missingRequestPerAlgoButCached.value().title().c_str(),
						 (m_histProp_missingRequestPerAlgoButCached.value().title()+";").c_str(), 
						 m_histProp_missingRequestPerAlgoButCached.value().bins(), 
						 m_histProp_missingRequestPerAlgoButCached.value().lowEdge(), 
						 m_histProp_missingRequestPerAlgoButCached.value().highEdge());
  if (m_hist_missingRequestPerAlgoButCached) {
    CAN_REBIN(m_hist_missingRequestPerAlgoButCached);
    if( rootHistSvc->regHist(path + m_hist_missingRequestPerAlgoButCached->GetName(), m_hist_missingRequestPerAlgoButCached).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestPerAlgoButCached->GetName() << endmsg;
    }
  }


 //
  m_hist_missingRequestedROBsPerCallButCached = new TH1F (m_histProp_missingRequestedROBsPerCallButCached.value().title().c_str(),
						 (m_histProp_missingRequestedROBsPerCallButCached.value().title()+";Number of non-predeclared ROBs").c_str(), 
						 m_histProp_missingRequestedROBsPerCallButCached.value().bins(), 
						 m_histProp_missingRequestedROBsPerCallButCached.value().lowEdge(), 
						 m_histProp_missingRequestedROBsPerCallButCached.value().highEdge());
  if (m_hist_missingRequestedROBsPerCallButCached) {
    if( rootHistSvc->regHist(path + m_hist_missingRequestedROBsPerCallButCached->GetName(), m_hist_missingRequestedROBsPerCallButCached).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerCallButCached->GetName() << endmsg;
    }
  }

 //
  m_hist_requestPerAlgo = new TH1F (m_histProp_requestPerAlgo.value().title().c_str(),
						 (m_histProp_requestPerAlgo.value().title()+";").c_str(), 
						 m_histProp_requestPerAlgo.value().bins(), 
						 m_histProp_requestPerAlgo.value().lowEdge(), 
						 m_histProp_requestPerAlgo.value().highEdge());
  if (m_hist_requestPerAlgo) {
    CAN_REBIN(m_hist_requestPerAlgo);
    if( rootHistSvc->regHist(path + m_hist_requestPerAlgo->GetName(), m_hist_requestPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_requestPerAlgo->GetName() << endmsg;
    }
  }

 //
  m_hist_callerPerEvent = new TH1F (m_histProp_callerPerEvent.value().title().c_str(),
						 (m_histProp_callerPerEvent.value().title()+";").c_str(), 
						 m_histProp_callerPerEvent.value().bins(), 
						 m_histProp_callerPerEvent.value().lowEdge(), 
						 m_histProp_callerPerEvent.value().highEdge());
  if (m_hist_callerPerEvent) {
    CAN_REBIN(m_hist_callerPerEvent);
    if( rootHistSvc->regHist(path + m_hist_callerPerEvent->GetName(), m_hist_callerPerEvent).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_callerPerEvent->GetName() << endmsg;
    }
  }

  //
  m_hist_requestedROBsPerAlgo = new TH1F (m_histProp_requestedROBsPerAlgo.value().title().c_str(),
					  (m_histProp_requestedROBsPerAlgo.value().title()+";").c_str(), 
					  m_histProp_requestedROBsPerAlgo.value().bins(), 
					  m_histProp_requestedROBsPerAlgo.value().lowEdge(), 
					  m_histProp_requestedROBsPerAlgo.value().highEdge());
  if (m_hist_requestedROBsPerAlgo) {
     CAN_REBIN(m_hist_requestedROBsPerAlgo);
    if( rootHistSvc->regHist(path + m_hist_requestedROBsPerAlgo->GetName(), m_hist_requestedROBsPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_requestedROBsPerAlgo->GetName() << endmsg;
    }
  }

  //
  m_hist_requestedROBsPerCallPerAlgo = new TH2F ("RequestedROBsPerCallPerAlgo","RequestedROBsPerCallPerAlgo;", 1, 0., 1., 100, 0., 2000.);
 if (m_hist_requestedROBsPerCallPerAlgo) {
    CAN_REBIN(m_hist_requestedROBsPerCallPerAlgo);
    if( rootHistSvc->regHist(path + m_hist_requestedROBsPerCallPerAlgo->GetName(), m_hist_requestedROBsPerCallPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_requestedROBsPerCallPerAlgo->GetName() << endmsg;
    }
  }

 //
  m_hist_nocachedRequestPerAlgo = new TH1F (m_histProp_nocachedRequestPerAlgo.value().title().c_str(),
						 (m_histProp_nocachedRequestPerAlgo.value().title()+";").c_str(), 
						 m_histProp_nocachedRequestPerAlgo.value().bins(), 
						 m_histProp_nocachedRequestPerAlgo.value().lowEdge(), 
						 m_histProp_nocachedRequestPerAlgo.value().highEdge());
  if (m_hist_nocachedRequestPerAlgo) {
     CAN_REBIN(m_hist_nocachedRequestPerAlgo);
    if( rootHistSvc->regHist(path + m_hist_nocachedRequestPerAlgo->GetName(), m_hist_nocachedRequestPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_nocachedRequestPerAlgo->GetName() << endmsg;
    }
  }

  //
  m_hist_nocachedRequestedROBsPerAlgo = new TH1F (m_histProp_nocachedRequestedROBsPerAlgo.value().title().c_str(),
					  (m_histProp_nocachedRequestedROBsPerAlgo.value().title()+";").c_str(), 
					  m_histProp_nocachedRequestedROBsPerAlgo.value().bins(), 
					  m_histProp_nocachedRequestedROBsPerAlgo.value().lowEdge(), 
					  m_histProp_nocachedRequestedROBsPerAlgo.value().highEdge());
  if (m_hist_nocachedRequestedROBsPerAlgo) {
     CAN_REBIN(m_hist_nocachedRequestedROBsPerAlgo);
    if( rootHistSvc->regHist(path + m_hist_nocachedRequestedROBsPerAlgo->GetName(), m_hist_nocachedRequestedROBsPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_nocachedRequestedROBsPerAlgo->GetName() << endmsg;
    }
  }


 //
  m_hist_missingRequestedROBsPerAlgo_pref = new TH1F (m_histProp_missingRequestedROBsPerAlgo_pref.value().title().c_str(),
						 (m_histProp_missingRequestedROBsPerAlgo_pref.value().title()+";").c_str(), 
						 m_histProp_missingRequestedROBsPerAlgo_pref.value().bins(), 
						 m_histProp_missingRequestedROBsPerAlgo_pref.value().lowEdge(), 
						 m_histProp_missingRequestedROBsPerAlgo_pref.value().highEdge());
  if (m_hist_missingRequestedROBsPerAlgo_pref) {
     CAN_REBIN(m_hist_missingRequestedROBsPerAlgo_pref);
    if( rootHistSvc->regHist(path + m_hist_missingRequestedROBsPerAlgo_pref->GetName(), m_hist_missingRequestedROBsPerAlgo_pref).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerAlgo_pref->GetName() << endmsg;
    }
  }

  //
  m_hist_missingRequestPerAlgo_pref = new TH1F (m_histProp_missingRequestPerAlgo_pref.value().title().c_str(),
						 (m_histProp_missingRequestPerAlgo_pref.value().title()+";").c_str(), 
						 m_histProp_missingRequestPerAlgo_pref.value().bins(), 
						 m_histProp_missingRequestPerAlgo_pref.value().lowEdge(), 
						 m_histProp_missingRequestPerAlgo_pref.value().highEdge());
  if (m_hist_missingRequestPerAlgo_pref) {
     CAN_REBIN(m_hist_missingRequestPerAlgo_pref);
    if( rootHistSvc->regHist(path + m_hist_missingRequestPerAlgo_pref->GetName(), m_hist_missingRequestPerAlgo_pref).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestPerAlgo_pref->GetName() << endmsg;
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
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerCall_pref->GetName() << endmsg;
    }
  }

//
  m_hist_missingRequestedROBsPerAlgoButCached_pref = new TH1F (m_histProp_missingRequestedROBsPerAlgoButCached_pref.value().title().c_str(),
						 (m_histProp_missingRequestedROBsPerAlgoButCached_pref.value().title()+";").c_str(), 
						 m_histProp_missingRequestedROBsPerAlgoButCached_pref.value().bins(), 
						 m_histProp_missingRequestedROBsPerAlgoButCached_pref.value().lowEdge(), 
						 m_histProp_missingRequestedROBsPerAlgoButCached_pref.value().highEdge());
  if (m_hist_missingRequestedROBsPerAlgoButCached_pref) {
     CAN_REBIN(m_hist_missingRequestedROBsPerAlgoButCached_pref);
    if( rootHistSvc->regHist(path + m_hist_missingRequestedROBsPerAlgoButCached_pref->GetName(), m_hist_missingRequestedROBsPerAlgoButCached_pref).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerAlgoButCached_pref->GetName() << endmsg;
    }
  }

  //
  m_hist_missingRequestPerAlgoButCached_pref = new TH1F (m_histProp_missingRequestPerAlgoButCached_pref.value().title().c_str(),
						 (m_histProp_missingRequestPerAlgoButCached_pref.value().title()+";").c_str(), 
						 m_histProp_missingRequestPerAlgoButCached_pref.value().bins(), 
						 m_histProp_missingRequestPerAlgoButCached_pref.value().lowEdge(), 
						 m_histProp_missingRequestPerAlgoButCached_pref.value().highEdge());
  if (m_hist_missingRequestPerAlgoButCached_pref) {
     CAN_REBIN(m_hist_missingRequestPerAlgoButCached_pref);
    if( rootHistSvc->regHist(path + m_hist_missingRequestPerAlgoButCached_pref->GetName(), m_hist_missingRequestPerAlgoButCached_pref).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestPerAlgoButCached_pref->GetName() << endmsg;
    }
  }

 //
  m_hist_missingRequestedROBsPerCallButCached_pref = new TH1F (m_histProp_missingRequestedROBsPerCallButCached_pref.value().title().c_str(),
						 (m_histProp_missingRequestedROBsPerCallButCached_pref.value().title()+";Number of non-predeclared ROBs").c_str(), 
						 m_histProp_missingRequestedROBsPerCallButCached_pref.value().bins(), 
						 m_histProp_missingRequestedROBsPerCallButCached_pref.value().lowEdge(), 
						 m_histProp_missingRequestedROBsPerCallButCached_pref.value().highEdge());
  if (m_hist_missingRequestedROBsPerCallButCached_pref) {
    if( rootHistSvc->regHist(path + m_hist_missingRequestedROBsPerCallButCached_pref->GetName(), m_hist_missingRequestedROBsPerCallButCached_pref).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_missingRequestedROBsPerCallButCached_pref->GetName() << endmsg;
    }
  }


  //
 
  m_hist_declaredROBsPerAlgo = new TH1F (m_histProp_declaredROBsPerAlgo.value().title().c_str(),
					  (m_histProp_declaredROBsPerAlgo.value().title()+";").c_str(), 
					  m_histProp_declaredROBsPerAlgo.value().bins(), 
					  m_histProp_declaredROBsPerAlgo.value().lowEdge(), 
					  m_histProp_declaredROBsPerAlgo.value().highEdge());
  if (m_hist_declaredROBsPerAlgo) {
     CAN_REBIN(m_hist_declaredROBsPerAlgo);
    if( rootHistSvc->regHist(path + m_hist_declaredROBsPerAlgo->GetName(), m_hist_declaredROBsPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_declaredROBsPerAlgo->GetName() << endmsg;
    }
  }

  //
  m_hist_prefetchedROBsPerAlgo = new TH1F (m_histProp_prefetchedROBsPerAlgo.value().title().c_str(),
					  (m_histProp_prefetchedROBsPerAlgo.value().title()+";").c_str(), 
					  m_histProp_prefetchedROBsPerAlgo.value().bins(), 
					  m_histProp_prefetchedROBsPerAlgo.value().lowEdge(), 
					  m_histProp_prefetchedROBsPerAlgo.value().highEdge());
  if (m_hist_prefetchedROBsPerAlgo) {
     CAN_REBIN(m_hist_prefetchedROBsPerAlgo);
    if( rootHistSvc->regHist(path + m_hist_prefetchedROBsPerAlgo->GetName(), m_hist_prefetchedROBsPerAlgo).isFailure() ) {
      logStream() << MSG::WARNING << "Can not register monitoring histogram: " << m_hist_prefetchedROBsPerAlgo->GetName() << endmsg;
    }
  }

  // release histogramming service
  rootHistSvc.release().ignore();
}

