/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/IParticleExtractor.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#ifndef GENERATIONBASE
#include "xAODJet/JetTrigAuxContainer.h"
#endif
#include "SGTools/TestStore.h" 

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <array>
#include "./testHelpers.h"

#include <iostream>
#include <sstream>

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;


using fastjet::PseudoJet;
using xAOD::IParticle;
using xAOD::JetContainer;
using xAOD::Jet;
using xAOD::JetFourMom_t;


class ExtractorTest: public ::testing::Test {
public:
  virtual void SetUp() {
    
  }
};



TEST_F(ExtractorTest, addJet) {

  // std::vector<JetFourMom_t> momenta;
  // momenta.push_back(JetFourMom_t(100., 0., 0., 0.)); //pt, eta, phi, m

  JetContainer* toAssociate = makeJetContainer_();
  typeAdjust(toAssociate);

  bool storeIt = false;
  JetContainer* outputs = makeJetContainer_(storeIt);

  auto jet = (*outputs)[0];

  // Create an Extractor
  std::string label("EMTopo");
  bool isGhost = false;
  IParticleExtractor ext0(toAssociate, label, isGhost);
 
   
  EXPECT_TRUE((jet->getConstituents()).size() == 0);
   
  std::vector<int> indices {0};
   
  ext0.addToJet(*jet, indices); 
  
  EXPECT_TRUE((jet->getConstituents()).size() == 1);
  const IParticle* in = ((jet->getConstituents()).at(0)).rawConstituent();
  const IParticle* out = toAssociate->at(0);
  EXPECT_TRUE(in == out);
   
   // Create another Extractor
  isGhost = true;
  IParticleExtractor ext1(toAssociate, label, isGhost);
  
  std::vector< const Jet* > retrievedJets;
  jet->getAssociatedObjects( "EMTopo", retrievedJets);
  EXPECT_TRUE(retrievedJets.size() == 0);

  ext1.addToJet(*jet, indices); 
  
  jet->getAssociatedObjects( "EMTopo", retrievedJets);
  
  EXPECT_TRUE(retrievedJets.size() == 1);
  EXPECT_TRUE(retrievedJets[0] == toAssociate->at(0));

  //Excercise the dump functions:

  // std::cout << ext0.toString(0) << '\n';
  // std::cout << ext1.toString(1) << '\n';

  ext0.toString(0);
  ext1.toString(1);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  SGTest::initTestStore();
  return RUN_ALL_TESTS();
}
