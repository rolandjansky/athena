/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopJetSubstructure/LargeJetTrimmer.h"

#include "xAODJet/Jet.h"

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/ClusterSequenceArea.hh>
#include <fastjet/AreaDefinition.hh>
#include <fastjet/contrib/Nsubjettiness.hh>
#include <fastjet/contrib/Njettiness.hh>
#include <fastjet/contrib/NjettinessPlugin.hh>
#include <fastjet/tools/Filter.hh>

using namespace xAOD;
using namespace fastjet;
using namespace fastjet::contrib;

top::LargeJetTrimmer::LargeJetTrimmer() {
}

top::LargeJetTrimmer::~LargeJetTrimmer() {
}

void top::LargeJetTrimmer::correctJet(xAOD::Jet& jet) {
  JetConstituentVector vec = jet.getConstituents();
  std::vector<fastjet::PseudoJet> p_c;

  xAOD::JetConstituentVector::iterator it = vec.begin();
  xAOD::JetConstituentVector::iterator itE = vec.end();
  for( ; it != itE; it++){
    PseudoJet p(0,0,0,0);
    float pt = (*it)->pt();
    float y = (*it)->rapidity();
    float phi = (*it)->phi();
    float m = (*it)->m();
    p.reset_PtYPhiM(pt, y, phi, m);
    p_c.push_back(p);
  }
  
  JetDefinition jet_def_large = JetDefinition(antikt_algorithm, 1.0, fastjet::E_scheme, fastjet::Best);
  ClusterSequence cs_large(p_c, jet_def_large);
  std::vector<PseudoJet> ljets = sorted_by_pt(cs_large.inclusive_jets(50e3));
  Filter trimmer(0.3, SelectorPtFractionMin(0.05));
  if (ljets.size() == 0) {
    return;
  }
  PseudoJet tjet = trimmer(ljets[0]);
  jet.setJetP4(xAOD::JetFourMom_t(tjet.pt(), tjet.eta(), tjet.phi(), tjet.m()));
}

void top::LargeJetTrimmer::print(std::ostream& o) const {
  o << "LargeJetTrimmer" << std::endl;
}

