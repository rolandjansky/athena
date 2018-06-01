/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/IParticleExtractor.h"
#include "JetRec/PseudoJetContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "SGTools/TestStore.h" 

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <array>

#include <iostream>
#include <sstream>

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;


using fastjet::PseudoJet;
using xAOD::IParticleContainer;
using xAOD::IParticle;
using xAOD::JetContainer;
using xAOD::Jet;
using xAOD::JetFourMom_t;

int JetContainerIndex (0);


JetContainer* store(JetContainer* jc) {
  std::ostringstream os;
  os << "JetCont"<<JetContainerIndex++;
  SGTest::store.record(jc, os.str());
  return jc;
}

JetContainer* makeJetContainer_(const std::vector<JetFourMom_t>& v,
                                bool isTrigger= false,
                                bool storeIt=true) {
  JetContainer* jc = new JetContainer;
  if (isTrigger){
    jc->setStore(new xAOD::JetTrigAuxContainer);
  } else {
    jc->setStore(new xAOD::JetAuxContainer);
  }

  for (const auto& m : v){
    Jet* j = new Jet;
    jc->push_back(j);
    j->setJetP4(m);
  }
  return storeIt ? store(jc):jc; 
}


JetContainer* makeJetContainer_(bool isTrigger=false, bool storeIt = true) {
  std::vector<JetFourMom_t> inMomenta;
  inMomenta.push_back(JetFourMom_t(50., 0., 0., 0.)); //pt, eta, phi, m
  inMomenta.push_back(JetFourMom_t(60., 0., 0., 0.)); //pt, eta, phi, m
  return makeJetContainer_(inMomenta, isTrigger, storeIt);
}


void typeAdjust(JetContainer* c){
  // Adjust Jet conatainer to allow it
  // to be treated as an IParticle container by the test store
  // - full StoreGate does not need this.
  SG::DataProxy* proxy = SGTest::store.proxy(c);
  if (proxy) {
    proxy->setTransientID (ClassID_traits<xAOD::IParticleContainer>::ID());
  }
}

    

struct IPtoPSConverter{
  std::size_t ind{0};
  PseudoJet operator()(IParticle* ip){
    PseudoJet psj(ip->p4());
    psj.set_user_index(ind++);
    return psj;
  }
};

IParticleExtractor* makeExtractor(const IParticleContainer* iparticles,
                                  bool isGhost, bool isTrigger=false){
  
  // Create an Extractor
  std::string label("EMTopo");
  IParticleExtractor* extractor = new IParticleExtractor(iparticles,
                                                         label,
                                                         isGhost,
                                                         isTrigger);
  return extractor;
}

