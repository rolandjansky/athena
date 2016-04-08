/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMenuAnalysis/ESNtComponent.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include "boost/foreach.hpp"

using namespace std;

ESNtComponent::ESNtComponent(NtupleAlgorithm* algo, 
			     const NtComponent::NtComponentParameters& params) : 
  NtComponent::NtupleComponent(algo, params), 
  m_trigConfigSvc("TrigConf::IHLTConfigSvc/TrigConfigSvc", algo->name()), 
  m_TDT("Trig::TrigDecisionTool/TrigDecisonTool"), 
  m_eventInfoKey("ByteStreamEventInfo"), m_chainNames(0), m_chainCounters(0) {
}

ESNtComponent::~ESNtComponent() {
}

StatusCode ESNtComponent::setupServices() {
  if (NtComponent::NtupleComponent::setupServices().isFailure()) {
    (*m_msg) << MSG::DEBUG << "Failed to setupService the NtupleCompoent"
	     << endreq;
    return StatusCode::FAILURE;
  }

  if (m_trigConfigSvc.retrieve().isFailure()) {
    (*m_msg) << MSG::DEBUG << "Cannot retrieve TrigConfigSvc" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_TDT.retrieve().isFailure()) {
    (*m_msg) << MSG::DEBUG << "Cannot retrieve TrigDecisionTool" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode ESNtComponent::book() {
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
  //  m_tree->Branch("ESChains", "std::vector<std::string>", &m_chainNames);
  m_tree->Branch("ESChainCounters", "std::vector<int>", &m_chainCounters);

  return StatusCode::SUCCESS;
}

StatusCode ESNtComponent::fill() {
   clear();

   bool found_es(false);

   const EventInfo* ei=0;
   if ( (*m_evtStore)->retrieve(ei).isFailure()) {
      (*m_msg) << MSG::WARNING << "Failed to retrieve EventInfo "
               << endreq;
   } else if (ei) {
      const TriggerInfo *ti = ei->trigger_info();
      if (ti) {
         const vector<TriggerInfo::StreamTag>& streams = ti->streamTags();

         for (unsigned int i=0; i<streams.size(); ++i) {
            const TriggerInfo::StreamTag& s = streams[i];
            if (s.type() == "express" && s.name() == "express") {
               found_es = true;
               break;
            }
         }
      }
   }
  
   const std::string k = "HLT_EXPRESS_OPI_EF";
   if (found_es && (*m_evtStore)->contains<TrigOperationalInfoCollection>(k)) {
      const TrigOperationalInfoCollection* coll=0;
      if ( (*m_evtStore)->retrieve(coll, k).isFailure()) {
         (*m_msg) << MSG::DEBUG 
                  << "Cannot retrieve TrigOperationalInfoCollection: "
                  << k << endreq;
      } else if (coll) {
         const TrigConf::HLTChainList* chainlist = m_trigConfigSvc->chainList();
      
         TrigOperationalInfoCollection::const_iterator p1;
         for (p1=coll->begin(); p1!=coll->end(); ++p1) {
            const TrigOperationalInfo* opi = *p1;
            if (opi) {
               pair<vector<string>, vector<float> > m = opi->infos();
               (*m_msg) << MSG::DEBUG 
                        << "Number of chains lead to express stream: "
                        << m.first.size() << endreq;
               for (unsigned int j=0; j<m.first.size(); ++j) {
                  (*m_msg) << MSG::DEBUG 
                           << "Express stream chain: " << m.first[j] << endreq;
                  int counter=-1;
                  if (chainlist) {
                     BOOST_FOREACH(const TrigConf::HLTChain* c, *chainlist) {
                        if (c && c->chain_name() == m.first[j]) {
                           counter = c->chain_counter();
                           break;
                        }
                     }
                  }
                  m_chainNames->push_back(m.first[j]);
                  m_chainCounters->push_back(counter);
               }
            }
         }
      }
   }
  

   return StatusCode::SUCCESS;
}

void ESNtComponent::clear() {
  if (m_chainNames) m_chainNames->clear();
  if (m_chainCounters) m_chainCounters->clear();
}

