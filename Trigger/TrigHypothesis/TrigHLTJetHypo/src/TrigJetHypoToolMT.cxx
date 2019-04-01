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
#include "./TrigJetHypoPrinterVisitor.h"

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

  try{
    pass = m_helper->pass(hypoJets);
  } catch(std::exception& e){
    ATH_MSG_ERROR("Exception raised by the TrigJetHypoToolHelperMT: " 
                  << e.what());
    return StatusCode::FAILURE;
  }

  // accumulateTime(steady_clock::now() - t);
  
  ATH_MSG_DEBUG("hypo testing done chain  "
                << " no of input jets " << jets->size()
                << " pass " << pass );

  if(m_visitDebug){

    TrigJetHypoPrinterVisitor visitor;
    m_helper->accept(visitor);

    SG::ReadHandle<xAOD::EventInfo> evt (m_evt);
    if (!evt.isValid()) {
      ATH_MSG_WARNING( "Could not get event info from TDS."  );
      visitor.write(name(), m_eventSN->getSN());      
    } else {
      visitor.write(name(), evt->runNumber(), evt->eventNumber());
    }
  }
  // delete the xAOD::Jet wrappers
  for(auto i : hypoJets){delete i;}

  return StatusCode::SUCCESS;
}


const HLT::Identifier& TrigJetHypoToolMT::getId() const{
  return m_decisionId;
} 
