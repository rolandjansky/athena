// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// this module allows sharing of the createPseudoJets function between
// trigger code and PseudoJetGetter

#ifndef PseudoJetCreatorFunctions_H
#define PseudoJetCreatorFunctions_H

#include "fastjet/PseudoJet.hh"
#include "xAODBase/IParticleContainer.h"
#include "xAODJet/JetContainerInfo.h"


template<class T> class ToolHandle;
class IIParticleRejectionTool;

namespace PseudoJetCreatorFunctions{
  using fastjet::PseudoJet;

  // noRejection set true for Trigger -  no longer true for jet from jet...
  std::vector<PseudoJet> 
  createPseudoJets(const xAOD::IParticleContainer*, 
                   xAOD::JetInput::Type, 
                   bool noNegE,
                   bool noRejection=false);
  
  // rejecter set up in initialisation
  std::vector<PseudoJet> 
  createPseudoJets(const xAOD::IParticleContainer*, 
                   xAOD::JetInput::Type,
                   ToolHandle<IIParticleRejectionTool>&);

  // ghost version
  std::vector<PseudoJet> 
  createGhostPseudoJets(const xAOD::IParticleContainer*, 
                   ToolHandle<IIParticleRejectionTool>&);
}


#endif
