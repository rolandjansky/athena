/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TriggerPseudoJetGetter2.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
//
// ********************************************************************
#include "TrigHLTJetRec/TriggerPseudoJetGetter2.h"
#include "TrigHLTJetRec/PseudoJetCreatorFunctions.h"
#include "JetRec/IParticleExtractor.h"
#include "JetEDM/JetConstituentFiller.h"

// string->inputtype
#include "xAODJet/JetContainerInfo.h"

TriggerPseudoJetGetter2::TriggerPseudoJetGetter2(const std::string& name)
  : AsgTool(name),  
    m_label("") {declareProperty("label", m_label);
}

TriggerPseudoJetGetter2::~TriggerPseudoJetGetter2(){}

StatusCode TriggerPseudoJetGetter2::initialize(){
  m_inputType = xAOD::JetInput::inputType(m_label); 
  return StatusCode::SUCCESS;
}

void TriggerPseudoJetGetter2::prime(const xAOD::CaloClusterContainer* inputs) {
  
  // use the input IParticles to make a PseudoJEtContainer
  // this method may be called more than once per event
  // label is a string which gives type information. It is used
  // to determine the function used to select the incomming IParticles.

  constexpr bool isGhost = false;
  auto extractor = std::make_unique<const IParticleExtractor>(inputs,
                                                              m_label,
                                                              isGhost);
  
  constexpr bool noRejection = true;
  std::vector<fastjet::PseudoJet> vpj = 
    PseudoJetCreatorFunctions::createPseudoJets(inputs, 
                                                m_inputType, 
                                                m_noNegE,
                                                noRejection);
  
  auto ppjc(std::make_unique<const PseudoJetContainer>(std::move(extractor), vpj));
  m_pseudoJetContainer.swap(ppjc);
}


const PseudoJetVector* TriggerPseudoJetGetter2::get() const {
  return m_pseudoJetContainer->casVectorPseudoJet();
}

// Write properties to the log
void TriggerPseudoJetGetter2::print() const {
  ATH_MSG_DEBUG("TriggerPseudoJetGetter2 dump:");    
  ATH_MSG_DEBUG("PseudoJet container " <<  *m_pseudoJetContainer);
  ATH_MSG_DEBUG("label               " << m_label);
  ATH_MSG_DEBUG("input type          "
                << xAOD::JetInput::typeName(m_inputType));
}


std::string TriggerPseudoJetGetter2::label() const { return m_label;}


StatusCode TriggerPseudoJetGetter2::createAndRecord() const{
  ATH_MSG_WARNING("Trigger: createAndRecord() does nothing");
  return StatusCode::SUCCESS;
}

int TriggerPseudoJetGetter2::outputContainerNames(std::vector<std::string>&) {

  return 1;
}

int TriggerPseudoJetGetter2::inputContainerNames(std::vector<std::string>&) {
  return 1;
}

