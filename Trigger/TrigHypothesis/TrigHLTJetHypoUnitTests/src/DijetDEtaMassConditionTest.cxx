/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetDEtaMassCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "./MockJetWithLorentzVector.h"
#include "gtest/gtest.h"
#include "./TLorentzVectorFactory.h"
#include <TLorentzVector.h>
#include "gmock/gmock.h"
#include <array>

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;


class DijetDEtaMassConditionTest: public ::testing::Test {
public:
  virtual void SetUp() {

    auto factory = TLorentzVectorFactory();
    
    m_eta0 = 0.5;
    m_eta1 = -0.5;
    m_et0 = 100.;
    m_et1 = 100.;


    m_tl0 = factory.make(m_eta0, m_et0);
    m_tl1 = factory.make(m_eta1, m_et1);

    m_eps = 0.00001;
    m_masseps = 0.01;

  }


  TLorentzVector m_tl0;
  TLorentzVector m_tl1;

  double m_eta0;
  double m_eta1;
  double m_et0;
  double m_et1;
  double m_eps;
  double m_masseps;
  
};


TEST_F(DijetDEtaMassConditionTest, artefacts){
  /* sanity checks on the TLorentzVectors and IJets
     used to test the DijetDEtaMassCondition objects */

  MockJetWithLorentzVector jet0{m_tl0};
  MockJetWithLorentzVector jet1{m_tl1};

  EXPECT_CALL(jet0, et()); 
  EXPECT_CALL(jet0, eta());

  EXPECT_CALL(jet1, et()); 
  EXPECT_CALL(jet1, eta());

  EXPECT_NEAR(m_tl0.Eta(), m_eta0, m_eps);
  EXPECT_NEAR(m_tl0.Et(), m_et0, m_eps);
  EXPECT_NEAR(m_tl1.Eta(), m_eta1, m_eps);
  EXPECT_NEAR(m_tl1.Et(), m_et1, m_eps);

  EXPECT_NEAR(jet0.eta(), m_eta0, m_eps);
  EXPECT_NEAR(jet0.et(), m_et0, m_eps);
  EXPECT_NEAR(jet1.eta(), m_eta1, m_eps);
  EXPECT_NEAR(jet1.et(), m_et1, m_eps);
  
  EXPECT_NEAR((m_tl0 + m_tl1).M(), 104.219, m_masseps);
}


TEST_F(DijetDEtaMassConditionTest, accepts) {

  MockJetWithLorentzVector jet0{m_tl0};
  MockJetWithLorentzVector jet1{m_tl1};

  EXPECT_CALL(jet0, eta());

  EXPECT_CALL(jet1, eta());

  HypoJetVector jets{&jet0, &jet1};

  DijetDEtaMassCondition condition(-1., 1., -5., 5., 100., 110.);

  EXPECT_TRUE(condition.isSatisfied(jets));
}


TEST_F(DijetDEtaMassConditionTest, belowEtaMinCut) {

  MockJetWithLorentzVector jet0{m_tl0};
  MockJetWithLorentzVector jet1{m_tl1};

  EXPECT_CALL(jet0, eta());

  EXPECT_CALL(jet1, eta());

  HypoJetVector jets{&jet0, &jet1};

  DijetDEtaMassCondition condition(-0.5+0.001, 1., -5., 5., 100., 110.);
  EXPECT_FALSE(condition.isSatisfied(jets));
}


TEST_F(DijetDEtaMassConditionTest, aboveEtaMaxCut) {
  DijetDEtaMassCondition condition(-1.0, 0.5-0.001, -5., 5., 100., 110.);


  MockJetWithLorentzVector jet0{m_tl0};
  MockJetWithLorentzVector jet1{m_tl1};

  EXPECT_CALL(jet0, eta());

  EXPECT_CALL(jet1, eta());

  HypoJetVector jets{&jet0, &jet1};

  EXPECT_FALSE(condition.isSatisfied(jets));
}


TEST_F(DijetDEtaMassConditionTest, belowYStarCut) {
  DijetDEtaMassCondition condition(-1.0, 1.0, 1.0 + 0.001, 5., 100., 110.);

  MockJetWithLorentzVector jet0{m_tl0};
  MockJetWithLorentzVector jet1{m_tl1};

  EXPECT_CALL(jet0, eta());

  EXPECT_CALL(jet1, eta());

  HypoJetVector jets{&jet0, &jet1};

  EXPECT_FALSE(condition.isSatisfied(jets));
}


TEST_F(DijetDEtaMassConditionTest, aboveYStarCut) {
  DijetDEtaMassCondition condition(-1.0, 1.0, -5.0, 1-0.001, 100., 110.);

  MockJetWithLorentzVector jet0{m_tl0};
  MockJetWithLorentzVector jet1{m_tl1};

  EXPECT_CALL(jet0, eta());

  EXPECT_CALL(jet1, eta());

  HypoJetVector jets{&jet0, &jet1};

  EXPECT_FALSE(condition.isSatisfied(jets));
}


TEST_F(DijetDEtaMassConditionTest, belowMassCut) {
  DijetDEtaMassCondition condition(-1.0, 1.0, -5.0, 5.0, 104.3, 110.);

  MockJetWithLorentzVector jet0{m_tl0};
  MockJetWithLorentzVector jet1{m_tl1};

  EXPECT_CALL(jet0, eta());

  EXPECT_CALL(jet1, eta());

  HypoJetVector jets{&jet0, &jet1};
  EXPECT_FALSE(condition.isSatisfied(jets));
}

TEST_F(DijetDEtaMassConditionTest, aboveMassCut) {
  DijetDEtaMassCondition condition(-1.0, 1.0, -5.0, 5.0, 100., 104.2);

  MockJetWithLorentzVector jet0{m_tl0};
  MockJetWithLorentzVector jet1{m_tl1};

  EXPECT_CALL(jet0, eta());

  EXPECT_CALL(jet1, eta());

  HypoJetVector jets{&jet0, &jet1};

  EXPECT_FALSE(condition.isSatisfied(jets));
}


TEST_F(DijetDEtaMassConditionTest, tooManyJets) {

  DijetDEtaMassCondition condition(-1.0, 1.0, -5.0, 5.0, 100., 110.);

  MockJetWithLorentzVector jet0{m_tl0};
  MockJetWithLorentzVector jet1{m_tl1};

  HypoJetVector jets{&jet0, &jet1};
  jets.push_back(&jet0);

  EXPECT_THROW(condition.isSatisfied(jets), std::runtime_error);
}


TEST_F(DijetDEtaMassConditionTest, tooFewJets) {

  DijetDEtaMassCondition condition(-1.0, 1.0, -5.0, 5.0, 100., 110.);

  MockJetWithLorentzVector jet0{m_tl0};

  HypoJetVector jets{&jet0};

  
  EXPECT_THROW(condition.isSatisfied(jets), std::runtime_error);
}
