/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/BoostToCenterOfMass.h"
#include "TLorentzVector.h"
using namespace std;

vector<fastjet::PseudoJet> JetSubStructureUtils::boostToCenterOfMass(fastjet::PseudoJet jet,
                                                   vector<fastjet::PseudoJet> constit_pseudojets) {
  vector<fastjet::PseudoJet> clusters;
  if(jet.e() < 1e-20) { // FPE
    return clusters;
  }

  double bx = jet.px()/jet.e();
  double by = jet.py()/jet.e();
  double bz = jet.pz()/jet.e();

  if(bx*bx + by*by + bz*bz >= 1) { // Faster than light
    return clusters;
  }

  for(unsigned int i1=0; i1 < constit_pseudojets.size(); i1++) {
    TLorentzVector v;
    v.SetPxPyPzE(constit_pseudojets.at(i1).px(), constit_pseudojets.at(i1).py(),constit_pseudojets.at(i1).pz(),constit_pseudojets.at(i1).e());
    v.Boost(-bx,-by,-bz);
    fastjet::PseudoJet v2(v.Px(), v.Py(), v.Pz(), v.E());
    clusters.push_back(v2);
  }

  return clusters;
}
