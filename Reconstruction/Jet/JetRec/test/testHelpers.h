/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/IParticleExtractor.h"
#include "JetRec/PseudoJetContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#ifndef GENERATIONBASE
#include "xAODJet/JetTrigAuxContainer.h"
#endif // NOT GENERATIONBASE
#include "SGTools/TestStore.h" 

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <array>

#include <iostream>
#include <sstream>
#include <memory>

int JetContainerIndex (0);


xAOD::JetContainer* store(xAOD::JetContainer* jc) {
  std::ostringstream os;
  os << "JetCont"<<JetContainerIndex++;
  SGTest::store.record(jc, os.str());
  return jc;
}

xAOD::JetContainer* makeJetContainer_(const std::vector<xAOD::JetFourMom_t>& v,
                                      bool isTrigger= false,
                                      bool storeIt=true) {
  xAOD::JetContainer* jc = new xAOD::JetContainer;
  if (isTrigger){
#ifndef GENERATIONBASE
    jc->setStore(new xAOD::JetTrigAuxContainer);
#endif
  } else {
    jc->setStore(new xAOD::JetAuxContainer);
  }

  for (const auto& m : v){
    xAOD::Jet* j = new xAOD::Jet;
    jc->push_back(j);
    j->setJetP4(m);
  }
  return storeIt ? store(jc):jc; 
}


xAOD::JetContainer* makeJetContainer_(bool isTrigger=false, bool storeIt = true) {
  std::vector<xAOD::JetFourMom_t> inMomenta;
  inMomenta.push_back(xAOD::JetFourMom_t(50., 0., 0., 0.)); //pt, eta, phi, m
  inMomenta.push_back(xAOD::JetFourMom_t(60., 0., 0., 0.)); //pt, eta, phi, m
  return makeJetContainer_(inMomenta, isTrigger, storeIt);
}


void typeAdjust(xAOD::JetContainer* c){
  // Adjust Jet container to allow it
  // to be treated as an IParticle container by the test store
  // - full StoreGate does not need this.
  SG::DataProxy* proxy = SGTest::store.proxy(c);
  if (proxy) {
    proxy->setTransientID (ClassID_traits<xAOD::IParticleContainer>::ID());
  }
}

    

struct IPtoPSConverter{
  std::size_t ind{0};
  fastjet::PseudoJet operator()(xAOD::IParticle* ip){
    fastjet::PseudoJet psj(ip->p4());
    psj.set_user_index(ind++);
    return psj;
  }
};

std::unique_ptr<const IParticleExtractor> makeExtractor(const xAOD::IParticleContainer* iparticles,
                                  bool isGhost, bool isTrigger=false){
  
  // Create an Extractor
  std::string label("EMTopo");
  return std::make_unique<const IParticleExtractor>(iparticles,
                                                    label,
                                                    isGhost,
                                                    isTrigger);
}

