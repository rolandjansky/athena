/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopJetSubstructure/SubjetMaker.h"

#include "xAODJet/Jet.h"

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/ClusterSequenceArea.hh>
#include <fastjet/AreaDefinition.hh>
#include <fastjet/contrib/Nsubjettiness.hh>
#include <fastjet/contrib/Njettiness.hh>
#include <fastjet/contrib/NjettinessPlugin.hh>
#include <fastjet/tools/Filter.hh>

using namespace fastjet;
using namespace fastjet::contrib;

top::SubjetMaker::SubjetMaker() {
}

top::SubjetMaker::~SubjetMaker() {
}

void top::SubjetMaker::correctJet(xAOD::Jet& jet) {
  //std::cout << "Making Subjets" << std::endl;
  xAOD::JetConstituentVector vec = jet.getConstituents();
  std::vector<fastjet::PseudoJet> p_c;

  for(auto it : vec){   
    PseudoJet p(0,0,0,0);
    float pt = (*it)->pt();
    float y = (*it)->rapidity();
    float phi = (*it)->phi();
    float m = (*it)->m();
    if (y != y) {
      continue;
    }
    else {
    p.reset_PtYPhiM(pt, y, phi, m);
    p_c.push_back(p);
    }
    
  }

  JetDefinition jet_def_small = JetDefinition(cambridge_algorithm, 0.2, fastjet::E_scheme, fastjet::Best);
  ClusterSequence cs_small(p_c, jet_def_small);
  std::vector<PseudoJet> ljets = sorted_by_pt(cs_small.inclusive_jets(20e3));
  if (ljets.size() == 0) {
    return;
  }
  std::vector<float> sje,sjpx,sjpy,sjpz;
  for (size_t z = 0; z < ljets.size(); ++z) {
    sje.push_back(ljets[z].e());
    sjpx.push_back(ljets[z].px());
    sjpy.push_back(ljets[z].py());
    sjpz.push_back(ljets[z].pz());
  }
 jet.auxdata<std::vector<float> >("Subjet_E") = sje;
 jet.auxdata<std::vector<float> >("Subjet_Px") = sjpx;
 jet.auxdata<std::vector<float> >("Subjet_Py") = sjpy;
 jet.auxdata<std::vector<float> >("Subjet_Pz") = sjpz;

}

void top::SubjetMaker::print(std::ostream& o) const {
  o << "SubjetMaker" << std::endl;
}
