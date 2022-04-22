/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODBase/IParticleContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#ifndef GENERATIONBASE
#include "xAODJet/JetTrigAuxContainer.h"
#endif
#include "SGTools/TestStore.h" 

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include <sstream>

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

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

class JetTest: public ::testing::Test {
public:
  virtual void SetUp() {
    
  }
};


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
#ifndef GENERATIONBASE
    jc->setStore(new xAOD::JetTrigAuxContainer);
#endif
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

    
TEST_F(JetTest, jetFunctions) {
  // Excercise calls to  setAssociatedObjects and
  // addConstituent of xAOD::Jet used by IParticleExtractor

  JetContainer* constituentJets = makeJetContainer_();
  typeAdjust(constituentJets);
  
  JetContainer* associatedJets = makeJetContainer_();

  typeAdjust(associatedJets);

  std::vector<const Jet*> v_constituents(associatedJets->begin(),
                                         associatedJets->end());
  std::string label("EMTopo");

  std::vector<JetFourMom_t> outMomenta;
  outMomenta.push_back(JetFourMom_t(100., 0., 0., 0.)); //pt, eta, phi, m
  outMomenta.push_back(JetFourMom_t(110., 0., 0., 0.)); //pt, eta, phi, m

  JetContainer* outputs = makeJetContainer_(outMomenta, false);
  auto jet0 = (*outputs)[0];
  auto jet1 = (*outputs)[1];

  EXPECT_TRUE(jet0->pt() == 100.);
  EXPECT_TRUE(jet1->pt() == 110.);
    
  jet0->setAttribute(label+"Pt", 100.);
  jet1->setAttribute(label+"Count", 100);
    
  jet0->setAssociatedObjects(label, v_constituents);
  for(const Jet* j  : *constituentJets){jet1->addConstituent(j);}

  EXPECT_TRUE(true);
}

#ifndef GENERATIONBASE
TEST_F(JetTest, triggerJetFunctions) {

  std::vector<JetFourMom_t> outMomenta;
  outMomenta.push_back(JetFourMom_t(100., 0., 0., 0.)); //pt, eta, phi, m
  outMomenta.push_back(JetFourMom_t(110., 0., 0., 0.)); //pt, eta, phi, m

  JetContainer* outputs = makeJetContainer_(outMomenta, true);
  auto jet0 = (*outputs)[0];
  // auto jet1 = (*outputs)[1];

  std::string label = "EMTopo";
  jet0->setAttribute<float>(label + "Pt", 10.);
  jet0->setAttribute<int>(label+"Count", 10);
  jet0->setAttribute("ECF1", 1.0); // double

  EXPECT_TRUE(true);
}
#endif

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  // SGTest::TestStore testStore;
  SGTest::initTestStore();
  return RUN_ALL_TESTS();
}
