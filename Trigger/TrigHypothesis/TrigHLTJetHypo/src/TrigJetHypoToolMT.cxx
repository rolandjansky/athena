/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoToolMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoToolMT.h"
#include "TrigJetHypoToolHelperMT.h"

#include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include "./DebugInfoCollector.h"

#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolMT::TrigJetHypoToolMT(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)),
  m_eventSN(std::make_unique<EventSN>()){
}


TrigJetHypoToolMT::~TrigJetHypoToolMT(){
}

StatusCode TrigJetHypoToolMT::initialize(){
  CHECK(m_evt.initialize());
  DebugInfoCollector collector(name());
  CHECK(m_helper->getDescription(collector));
  auto s = collector.toString();
  
  for(const auto& l : lineSplitter(s)){
    ATH_MSG_INFO(l);
  }
  
  if (m_visitDebug){
    collector.write();
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigJetHypoToolMT::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigJetHypoToolMT::decide(const xAOD::JetContainer* jets,
                                           bool& pass) const {

  pass = false;
  HypoJetVector hypoJets(jets->size());
  
  std::transform(jets -> begin(),
                 jets -> end(),
                 hypoJets.begin(),
                 xAODJetAsIJetFactory());

  /* apply cleaning and hypothesis alg */
  ATH_MSG_DEBUG("hypo helper start... "
                << " no of jets ... " 
                << jets->size() 
                << "...");

  // steady_clock::time_point t =  steady_clock::now();
  std::unique_ptr<ITrigJetHypoInfoCollector> infocollector(nullptr);
  if(m_visitDebug){
    auto collectorName = name() + std::to_string(m_eventSN->getSN());
    infocollector.reset(new  DebugInfoCollector(collectorName));
  } 

  try{
    pass = m_helper->pass(hypoJets, infocollector);
  } catch(std::exception& e){
    ATH_MSG_ERROR("Exception raised by the TrigJetHypoToolHelperMT: " 
                  << e.what());
    return StatusCode::FAILURE;
  }
  
  if (infocollector){
    infocollector->write();
  }

  // accumulateTime(steady_clock::now() - t);
  
  ATH_MSG_DEBUG("hypo testing done chain  "
                << " no of input jets " << jets->size()
                << " pass " << pass );
  return StatusCode::SUCCESS;
}

const HLT::Identifier& TrigJetHypoToolMT::getId() const{
  return m_decisionId;
} 
