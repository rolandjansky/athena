/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetOrToolMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetOrToolMT.h"

#include "GaudiKernel/StatusCode.h"

#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetOrToolMT::TrigJetOrToolMT(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent) :
  base_class(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)) {
}


TrigJetOrToolMT::~TrigJetOrToolMT(){
}

StatusCode TrigJetOrToolMT::initialize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigJetOrToolMT::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigJetOrToolMT::decide(const xAOD::JetContainer* jets,
                                           bool& pass) const {

  m_lhs->decide(jets, pass);
  if (not pass){
    m_rhs->decide(jets, pass);
    ATH_MSG_DEBUG("LHS failed " << " RHS " <<std::boolalpha << pass);
  } else {
    ATH_MSG_DEBUG("LHS passed");
  }

  return StatusCode::SUCCESS;
}


const HLT::Identifier& TrigJetOrToolMT::getId() const{
  return m_decisionId;
} 
