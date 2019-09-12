/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./makeHypoJets.h"
#include "./TLorentzVectorFactory.h"
#include "./TLorentzVectorAsIJet.h"



HypoJetVector makeHypoJets(const std::vector<double>& etas){

  HypoJetVector jets;
  
  TLorentzVectorFactory factory;
  
  auto make_jet = [&factory](double eta){
    return new TLorentzVectorAsIJet(factory.make(eta, 10.));
  };

  std::transform(etas.begin(),
                 etas.end(),
                 std::back_inserter(jets),
                 make_jet);
  
  return jets;
}

/*
HypoJetVector makeHypoJets(const std::vector<double>& etas) {
  auto gmockJets = makeGMockJets(etas);
  HypoJetVector jets(gmockJets.begin(), gmockJets.end());
  return jets;
}


HypoJetVector makeHypoJets(const std::vector<MockJetWithLorentzVector*>& gj) {
  HypoJetVector jets(gj.begin(), gj.end());
  return jets;
}
*/

