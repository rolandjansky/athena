/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetNotToolMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetNotToolMT.h"

#include "GaudiKernel/StatusCode.h"

#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetNotToolMT::TrigJetNotToolMT(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent) :
  base_class(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)) {
}


TrigJetNotToolMT::~TrigJetNotToolMT(){
}

StatusCode TrigJetNotToolMT::initialize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigJetNotToolMT::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigJetNotToolMT::decide(const xAOD::JetContainer* jets,
                                           bool& pass) const {

  m_hypoTool->decide(jets, pass);

  if(pass){
    ATH_MSG_DEBUG("hypoTool passed");
  } else {
    ATH_MSG_DEBUG("hypoTool failed");
  }

  pass = !pass;

  return StatusCode::SUCCESS;
}


const HLT::Identifier& TrigJetNotToolMT::getId() const{
  return m_decisionId;
} 
 
