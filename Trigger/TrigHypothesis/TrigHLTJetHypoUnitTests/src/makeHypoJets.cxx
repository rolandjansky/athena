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
    return std::shared_ptr<const HypoJet::IJet>(new TLorentzVectorAsIJet(factory.make(eta, 10.)));
  };

  std::transform(etas.begin(),
                 etas.end(),
                 std::back_inserter(jets),
                 make_jet);
  
  return jets;
}


