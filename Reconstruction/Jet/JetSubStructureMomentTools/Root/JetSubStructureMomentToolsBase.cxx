/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <math.h>
#include <float.h>
#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetEDM/JetConstituentFiller.h"
#include "fastjet/ClusterSequence.hh"


using namespace std;
using fastjet::PseudoJet;

JetSubStructureMomentToolsBase::JetSubStructureMomentToolsBase(std::string name) : 
  JetModifierBase(name)
{
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
