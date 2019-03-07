/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetAndToolMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetAndToolMT.h"

#include "GaudiKernel/StatusCode.h"

#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetAndToolMT::TrigJetAndToolMT(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent) :
  base_class(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)) {
}


TrigJetAndToolMT::~TrigJetAndToolMT(){
}

StatusCode TrigJetAndToolMT::initialize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigJetAndToolMT::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigJetAndToolMT::decide(const xAOD::JetContainer* jets,
                                           bool& pass) const {

  m_lhs->decide(jets, pass);
  if (pass){
    ATH_MSG_DEBUG("LHS passed");
    m_rhs->decide(jets, pass);
    ATH_MSG_DEBUG("RHS " <<std::boolalpha << pass);
  } else {
    ATH_MSG_DEBUG("LHS failed");
  }    


  return StatusCode::SUCCESS;
}


const HLT::Identifier& TrigJetAndToolMT::getId() const{
  return m_decisionId;
} 
