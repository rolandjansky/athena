/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TriggerPseudoJetGetter.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// ********************************************************************
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolFactory.h"
#include "TrigHLTJetRec/TriggerPseudoJetGetter.h"
// #include "GaudiKernel/ToolFactory.h"
// #include "GaudiKernel/IToolFactory.h"

TriggerPseudoJetGetter::TriggerPseudoJetGetter(const std::string& name)
  :AsgTool(name),  m_primed(false), m_label(""), m_pseudoJetVector(nullptr){
  // declareInterface<ITriggerPseudoJetGetter>(this);
  declareProperty("Label", m_label); 
  declareProperty("OutputContainer", m_outputContainer); 
}

TriggerPseudoJetGetter::~TriggerPseudoJetGetter(){}

StatusCode TriggerPseudoJetGetter::initialize()
{
  return StatusCode::SUCCESS;
}

void TriggerPseudoJetGetter::prime(const PseudoJetVector* psjv)
{
  m_pseudoJetVector = psjv;
  m_primed = true;
}

const PseudoJetVector* TriggerPseudoJetGetter::get() const 
{
  if (!m_primed) {
    ATH_MSG_ERROR("Prime was not called before get()");
    return 0;
  }
  return m_pseudoJetVector;
}

// Write properties to the log
void TriggerPseudoJetGetter::print() const 
{
  ATH_MSG_DEBUG("The input trigger element contained "
                << m_pseudoJetVector->size()
                << " particles");
}


std::string TriggerPseudoJetGetter::label() const { return m_label;}


int TriggerPseudoJetGetter::inputContainerNames(std::vector<std::string>&) {
  return 0;
}

int TriggerPseudoJetGetter::outputContainerNames(std::vector<std::string>&) {
  return 0;
}

// needed for future changes to offline jet code
// StatusCode TriggerPseudoJetGetter::createAndRecord() const {
//   return StatusCode::FAILURE; // Trigger: should never be called.
// }
