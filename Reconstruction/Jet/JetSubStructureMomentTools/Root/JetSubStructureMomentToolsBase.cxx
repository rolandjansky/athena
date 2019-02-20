/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetEDM/JetConstituentFiller.h"
#include "fastjet/ClusterSequence.hh"

using fastjet::PseudoJet;

JetSubStructureMomentToolsBase::JetSubStructureMomentToolsBase(std::string name) : 
  JetModifierBase(name)
{
  declareProperty("inputContainer",m_inputContainer = "");
  declareProperty("Prefix",        m_prefix         = "");
}

StatusCode JetSubStructureMomentToolsBase::initialize() {

  
  if(!m_inputContainer.empty() && m_prefix.empty()) {
    ATH_MSG_WARNING("No prefix defined, defaulting to " << m_inputContainer);
    m_prefix = m_inputContainer;
  }
  
  if(!m_prefix.empty() && m_prefix.find_last_of("_")+1 != m_prefix.size()) m_prefix += "_";
  
  ATH_MSG_DEBUG("InputContainer = " << m_inputContainer);
  ATH_MSG_DEBUG("Prefix = " << m_prefix);

  return StatusCode::SUCCESS;
}


bool JetSubStructureMomentToolsBase::SetupDecoration(PseudoJet& pseudojet,const  xAOD::Jet& jet) const {

  bool decorate = true;
  
  if (m_inputContainer.empty()) {
    if (!checkForConstituents(jet))
      decorate = false;
    else pseudojet = buildPseudoJet(jet);
  }
  else {
    std::vector<const xAOD::IParticle*> AssociatedParticles;
    if (!(jet.getAssociatedObjects(m_inputContainer,AssociatedParticles))){
      ATH_MSG_WARNING("InputContainer " << m_inputContainer << " unknown. Exiting..");
      decorate = false;
    }
    ATH_MSG_VERBOSE("Found " << AssociatedParticles.size() << " associated particles");
    if(!AssociatedParticles.size()) decorate = false; 
    else pseudojet = buildPseudoJet(AssociatedParticles);
  }

  return decorate;
}


PseudoJet JetSubStructureMomentToolsBase::buildPseudoJet (const xAOD::Jet & jet) const {
  std::vector<PseudoJet> constit_pseudojets = jet::JetConstituentFiller::constituentPseudoJets(jet);
  return fastjet::join(constit_pseudojets);
}

PseudoJet JetSubStructureMomentToolsBase::buildPseudoJet(const std::vector<const xAOD::IParticle*>& iparticles) const {
  std::vector<PseudoJet> pjs;
  pjs.reserve(iparticles.size());
  for (auto iparticle : iparticles)  
    pjs.push_back(PseudoJet(iparticle->p4()));
  return fastjet::join(pjs);
}
