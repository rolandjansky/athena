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

TriggerPseudoJetGetter::TriggerPseudoJetGetter(const std::string& s,
                                               const std::string& name,
                                               const IInterface* i) 
  : PseudoJetGetter(s, name, i), m_primed(false), m_pseudoJetVector(0){
  declareInterface<ITriggerPseudoJetGetter>(this);
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

