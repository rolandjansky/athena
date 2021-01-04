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

#include "./testHelpers.h"


using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;


using fastjet::PseudoJet;
using xAOD::IParticleContainer;
using xAOD::IParticle;
using xAOD::JetContainer;
using xAOD::Jet;
using xAOD::JetFourMom_t;


class PseudoJetContainerOfflineTest: public ::testing::Test {
public:
  virtual void SetUp() {
    
    constexpr bool storeIt = true;
    constexpr bool isTrigger = false;
    
    m_toAssociate0 = makeJetContainer_(isTrigger, storeIt);
    typeAdjust(m_toAssociate0);
    std::transform(m_toAssociate0->begin(),
                   m_toAssociate0->end(),
                   std::back_inserter(m_pjVec0),
                   IPtoPSConverter());
    
    m_toAssociate1 = makeJetContainer_(isTrigger, storeIt);
    typeAdjust(m_toAssociate1);
    std::transform(m_toAssociate1->begin(),
                   m_toAssociate1->end(),
                   std::back_inserter(m_pjVec1),
                   IPtoPSConverter());


    bool ghost{false};
    m_pExtractor_noghost = (makeExtractor(m_toAssociate0, ghost));

    ghost = true;
    m_pExtractor_ghost = (makeExtractor(m_toAssociate1, ghost));

    JetContainer* outputs = makeJetContainer_(isTrigger, storeIt);
    m_testjet0 = (*outputs)[0];
    m_testjet1 = (*outputs)[1];
  }


protected:
  JetContainer* m_toAssociate0{nullptr};
  JetContainer* m_toAssociate1{nullptr};

  std::vector<PseudoJet> m_pjVec0;
  std::vector<PseudoJet> m_pjVec1;

  std::unique_ptr<const IParticleExtractor> m_pExtractor_noghost{};
  std::unique_ptr<const IParticleExtractor> m_pExtractor_ghost{};

  Jet* m_testjet0;
  Jet* m_testjet1;
};


TEST_F(PseudoJetContainerOfflineTest, check_fixtures) {
  EXPECT_FALSE(m_pjVec0.empty());

  std::size_t uui;

  EXPECT_TRUE((m_pjVec0.back()).user_index()+1 >= 0);
  uui = (m_pjVec0.back()).user_index()+1;
  EXPECT_TRUE(uui == m_pjVec0.size());
}

TEST_F(PseudoJetContainerOfflineTest, test_noghost) {
  // Test assigning IParticles to jets using a single Extractor
  // PseudoJetContainer. The Extractor has the ghost flag set to false.
  // The jet containers have a JetAuxContainer, and are stored in the test store
  


  // create the PseudoContainer
  PseudoJetContainer psc(std::move(m_pExtractor_noghost), m_pjVec0);

  // check the pseudojet accessors
  // EXPECT_TRUE(psc.asVectorPseudoJet() == m_pjVec0);  
  EXPECT_TRUE(*psc.casVectorPseudoJet() == m_pjVec0);

  psc.extractConstituents(*m_testjet0, m_pjVec0);

  auto constituents = m_testjet0->getConstituents();
  EXPECT_TRUE(constituents.size() == m_toAssociate0->size());
  for (const auto c : constituents) {
    EXPECT_TRUE(std::find(m_toAssociate0->begin(),
                          m_toAssociate0->end(),
                          c->rawConstituent()) != m_toAssociate0->end());
  }


  std::vector<const Jet*> associated;
    
  bool rc = m_testjet0->getAssociatedObjects("EMTopo", associated);
  EXPECT_FALSE(rc);
  EXPECT_TRUE(associated.empty());

}

TEST_F(PseudoJetContainerOfflineTest, test_ghost) {
  // Test assigning IParticles to jets using a single Extractor
  // PseudoJetContainer. The Extractor has the ghost flag set to true.
  // The jet containers have a JetAuxContainer, and are stored in the test store

  // create the PseudoContainer
  PseudoJetContainer psc(std::move(m_pExtractor_ghost), m_pjVec0);

  // check the pseudojet accessors
  // EXPECT_TRUE(psc.asVectorPseudoJet() == m_pjVec0);  
  EXPECT_TRUE(*psc.casVectorPseudoJet() == m_pjVec0);

  psc.extractConstituents(*m_testjet0, m_pjVec0);

  auto constituents = m_testjet0->getConstituents();
  EXPECT_TRUE(constituents.empty());

  std::vector<const Jet*> associated;
    
  bool rc = m_testjet0->getAssociatedObjects("EMTopo", associated);
  EXPECT_TRUE(rc);
  EXPECT_FALSE(associated.empty());
  for(const auto p : associated){
    EXPECT_FALSE(std::find(m_toAssociate1->begin(),
                           m_toAssociate1->end(),
                           p) == m_toAssociate1->end());
                           
  }

}

TEST_F(PseudoJetContainerOfflineTest, test_append) {
  // Test appending PseduoJetContainers
  // Created two single Extractor PseudoJetContainers
  // One Extractor has isGhost=true, the other false.
  // Append the PJContainers. Send in two test jets. The two extractors each
  // will recieve one of these jets.
  // check that the jets have the appropriate constituents or associated
  // objects set.

  bool debug{false};
  
  // create the PseudoContainers
  PseudoJetContainer psc0(std::move(m_pExtractor_noghost), m_pjVec0);
  PseudoJetContainer psc1(std::move(m_pExtractor_ghost), m_pjVec1);

  psc0.append(&psc1);

  const std::vector<PseudoJet>* pjFromPSC = psc0.casVectorPseudoJet();

  // usr indices have been modified

  std::vector<PseudoJet> spjVec0(pjFromPSC->begin(),
                                 pjFromPSC->begin()+2);

  std::vector<PseudoJet> spjVec1(pjFromPSC->begin()+2,
                                 pjFromPSC->end());
  
  

  if(debug){
    for(auto pj : spjVec0){
      std::cout << "pj indx spjVec0 "<<pj.user_index() << '\n';
    }

    for(auto pj : spjVec1){
      std::cout << "pj indx spjVec1 "<<pj.user_index() << '\n';
    }
  }

  psc0.extractConstituents(*m_testjet0, spjVec0);
  psc0.extractConstituents(*m_testjet1, spjVec1);

  if (debug) {
    std::cout<<"test jet0 " << m_testjet0 << '\n';
    std::cout<<"test jet1 " << m_testjet1 << '\n';
  }
  
  std::vector<const Jet*> associated0;
  std::vector<const Jet*> associated1;


  if (debug) {
    for(auto c : *m_toAssociate0){std::cout<<"m_toAssociate0 "<<c << '\n';}
    for(auto c : *m_toAssociate1){std::cout<<"m_toAssociate1 "<<c << '\n';}
  }
  
  // jet0 - associated particls route it to extractor 0
  // has ghost = false expect no associated objects, some constituents.
  // jet1 - associated particls route it to extractor 1
  // has ghost = true expect some associated objects, no constituents.
  auto constituents0 = m_testjet0->getConstituents();
  auto constituents1 = m_testjet1->getConstituents();

  if (debug) {
    std::cout << "constituents0 size() " << constituents0.size() << '\n';
    for(auto c : constituents0){
      std::cout<<"tj0 constituent "<<c->rawConstituent() << '\n';
    }

    std::cout << "constituents1 size() " << constituents1.size() << '\n';
    for(auto c : constituents1){
      std::cout<<"tj1 constituent "<<c->rawConstituent() << '\n';
    }
  }

  
  bool rc0 = m_testjet0->getAssociatedObjects("EMTopo", associated0);
  EXPECT_TRUE(rc0);

  bool rc1 = m_testjet1->getAssociatedObjects("EMTopo", associated1);
  EXPECT_TRUE(rc1);

  if (debug) {
    std::cout << "associated0 size() " << associated0.size() << '\n';
    for(auto i : associated0){std::cout << " tj0 associated element "
                                        << i <<'\n';}
    
    std::cout << "associated1 size() " << associated1.size() << '\n';
    for(auto i : associated1){std::cout << " tj1 associated element "
                                        << i <<'\n';}
  }
  
  EXPECT_TRUE(constituents0.size() == m_toAssociate0->size());
  for (const auto c : constituents0) {
    EXPECT_TRUE(std::find(m_toAssociate0->begin(),
                          m_toAssociate0->end(),
                          c->rawConstituent()) != m_toAssociate0->end());
  }

  EXPECT_TRUE(associated0.empty());

  EXPECT_TRUE(constituents1.empty());

  EXPECT_FALSE(associated1.empty());
  for(const auto p : associated1){
    EXPECT_FALSE(std::find(m_toAssociate1->begin(),
                           m_toAssociate1->end(),
                           p) == m_toAssociate1->end());
                           
  }
  // exercise the dumper
  if (debug) {
    std::cout << psc0.toString(1, 1) <<'\n';
  } else {
    psc0.toString(1, 1);
  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  SGTest::initTestStore();
  return RUN_ALL_TESTS();
}
