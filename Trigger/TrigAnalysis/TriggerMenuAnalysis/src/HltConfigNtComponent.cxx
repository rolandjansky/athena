/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  HltConfigNtComponent.cxx
*/
#include "TriggerMenuAnalysis/HltConfigNtComponent.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "boost/foreach.hpp"

using namespace std;

HltConfigNtComponent::HltConfigNtComponent(NtupleAlgorithm* algo, 
					   const NtComponent::NtComponentParameters& params) : 
  NtComponent::NtupleComponent(algo, params), 
  mTrigConfigSvc("TrigConf::IHLTConfigSvc/TrigConfigSvc", algo->name()), 
  mEventInfoKey("ByteStreamEventInfo"), mRun(0), 
  mChainNames(0), mChainCounters(0) {
}

HltConfigNtComponent::~HltConfigNtComponent() {
}

StatusCode HltConfigNtComponent::setupServices() {
  if (NtComponent::NtupleComponent::setupServices().isFailure()) {
    (*m_msg) << MSG::DEBUG << "Cannot setup service " << endreq;
    return StatusCode::FAILURE;
  }
  if (mTrigConfigSvc.retrieve().isFailure()) {
    (*m_msg) << MSG::DEBUG << "Cannot retrieve TrigConfigSvc" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode HltConfigNtComponent::book() {
  map<string, string>::const_iterator p = 
    m_parameters.containerKeys.find("EventInfo");
  if (p != m_parameters.containerKeys.end()) {
    mEventInfoKey = p->second;
  } else {
    (*m_msg) << MSG::INFO << "No EventInfo key specified, use " 
	     << mEventInfoKey << endreq;
  }
  
  mChainNames = new std::vector<std::string>();
  mChainCounters = new std::vector<int>();

  m_tree->Branch("Run", &mRun, "Run/I");
  m_tree->Branch("ChainNames", "std::vector<std::string>", &mChainNames);
  m_tree->Branch("ChainCounters", "std::vector<int>", &mChainCounters);
  return StatusCode::SUCCESS;
}

StatusCode HltConfigNtComponent::fill() {
  clear();

  const EventInfo* ei=0;
  if ( (*m_evtStore)->retrieve(ei).isFailure()) {
    (*m_msg) << MSG::WARNING << "Failed to retrieve EventInfo "
	     << endreq;
  }
  if (ei) {
    const EventID *eid = ei->event_ID();
    if (eid) mRun = eid->run_number();
  }

  const TrigConf::HLTChainList* chainlist = mTrigConfigSvc->chainList();
  if (chainlist) {
    BOOST_FOREACH(TrigConf::HLTChain* chain, *chainlist) {
      mChainNames->push_back( chain->chain_name());
      mChainCounters->push_back( chain->chain_counter());
    }
  }
  return StatusCode::SUCCESS;
}

void HltConfigNtComponent::clear() {
  mRun = 0;
  if (mChainNames) mChainNames->clear();
  if (mChainCounters) mChainCounters->clear();
}

