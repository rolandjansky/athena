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
  m_trigConfigSvc("TrigConf::IHLTConfigSvc/TrigConfigSvc", algo->name()), 
  m_eventInfoKey("ByteStreamEventInfo"), m_run(0), 
  m_chainNames(0), m_chainCounters(0) {
}

HltConfigNtComponent::~HltConfigNtComponent() {
}

StatusCode HltConfigNtComponent::setupServices() {
  if (NtComponent::NtupleComponent::setupServices().isFailure()) {
    (*m_msg) << MSG::DEBUG << "Cannot setup service " << endreq;
    return StatusCode::FAILURE;
  }
  if (m_trigConfigSvc.retrieve().isFailure()) {
    (*m_msg) << MSG::DEBUG << "Cannot retrieve TrigConfigSvc" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode HltConfigNtComponent::book() {
  map<string, string>::const_iterator p = 
    m_parameters.containerKeys.find("EventInfo");
  if (p != m_parameters.containerKeys.end()) {
    m_eventInfoKey = p->second;
  } else {
    (*m_msg) << MSG::INFO << "No EventInfo key specified, use " 
	     << m_eventInfoKey << endreq;
  }
  
  m_chainNames = new std::vector<std::string>();
  m_chainCounters = new std::vector<int>();

  m_tree->Branch("Run", &m_run, "Run/I");
  m_tree->Branch("ChainNames", "std::vector<std::string>", &m_chainNames);
  m_tree->Branch("ChainCounters", "std::vector<int>", &m_chainCounters);
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
    if (eid) m_run = eid->run_number();
  }

  const TrigConf::HLTChainList* chainlist = m_trigConfigSvc->chainList();
  if (chainlist) {
    BOOST_FOREACH(TrigConf::HLTChain* chain, *chainlist) {
      m_chainNames->push_back( chain->chain_name());
      m_chainCounters->push_back( chain->chain_counter());
    }
  }
  return StatusCode::SUCCESS;
}

void HltConfigNtComponent::clear() {
  m_run = 0;
  if (m_chainNames) m_chainNames->clear();
  if (m_chainCounters) m_chainCounters->clear();
}

