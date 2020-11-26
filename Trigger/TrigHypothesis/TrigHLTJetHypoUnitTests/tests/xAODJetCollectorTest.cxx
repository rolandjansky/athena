/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJet/Jet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"
#include "../src/TLorentzVectorAsIJet.h"
#include "TrigHLTJetHypo/../src/xAODJetCollector.h"
#include <TLorentzVector.h>
#include "gtest/gtest.h"

/*
using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;
*/

class xAODJetCollectorTest: public ::testing::Test {
public:
  xAODJetCollectorTest() {}
};


TEST_F(xAODJetCollectorTest, zeroJets){
  // zero jets in. Jet collector is empty.
  
  HypoJetVector jets;
  xAODJetCollector collector;
  collector.addJets(jets.begin(), jets.end());
  EXPECT_TRUE(collector.empty());
}


TEST_F(xAODJetCollectorTest, multipleInputJets){
  //multiple jets in. Jet collector is not empty and has the correct number.

  constexpr std::size_t njets{11};
  std::vector<xAOD::Jet> xaodjets (njets);
  HypoJetVector jets;
  jets.reserve (njets);
  for(unsigned int ijet = 0; ijet < njets; ++ijet){
    jets.emplace_back (new HypoJet::xAODJetAsIJet(&xaodjets[ijet], ijet));
  }

  xAODJetCollector collector;
  collector.addJets(jets.begin(), jets.end());
 
  EXPECT_FALSE(collector.empty());
  EXPECT_FALSE((collector.xAODJets()).size() == 10);
}


TEST_F(xAODJetCollectorTest, nonXAODJets){
  // multiple non-xAOD jets in. Jet collector is empty.

  unsigned int njets{11};
  HypoJetVector jets;
  jets.reserve (njets);
  for(unsigned int ijet = 0; ijet < njets; ++ijet){
    TLorentzVector v;
    jets.emplace_back (new TLorentzVectorAsIJet(v));
  }

  xAODJetCollector collector;
  collector.addJets(jets.begin(), jets.end());

  // These hypoJets are not xAOD jets.
  EXPECT_FALSE(collector.empty());
  EXPECT_TRUE(collector.size() == njets);
  EXPECT_TRUE((collector.hypoJets()).size() == njets);
  EXPECT_TRUE((collector.xAODJets()).size() == 0);
}

