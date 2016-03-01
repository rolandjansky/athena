/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCondition.h"
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


class EtaEtConditionTest: public ::testing::Test {
public:

  EtaEtConditionTest() {

    auto factory = TLorentzVectorFactory();
    m_tl0 = factory.make(m_eta0, m_et0);

    m_eps = 0.00001;
    m_masseps = 0.01;


  }

  virtual void SetUp() {

    auto factory = TLorentzVectorFactory();
    
    m_eta0 = 0.5;
    m_et0 = 100.;


    m_tl0 = factory.make(m_eta0, m_et0);

    m_eps = 0.00001;
  }

  TLorentzVector m_tl0;

  double m_eta0;
  double m_et0;
  double m_eps;
  double m_masseps;
  
};


TEST_F(EtaEtConditionTest, artefacts){
  /* sanity checks on the TLorentzVectors and IJets
     used to test the EtaEtCondition objects */

  EXPECT_NEAR(m_tl0.Eta(), m_eta0, m_eps);
  EXPECT_NEAR(m_tl0.Et(), m_et0, m_eps);

  MockJetWithLorentzVector jet0{m_tl0};
  HypoJetVector jets{&jet0};

  EXPECT_EQ(jets.size(), static_cast<unsigned int>(1));
}


TEST_F(EtaEtConditionTest, accepts) {

  EtaEtCondition condition(-1., 1., 99.99999);

  MockJetWithLorentzVector jet0{m_tl0};
  HypoJetVector jets{&jet0};

  EXPECT_CALL(jet0, et()); 
  EXPECT_CALL(jet0, eta());


  EXPECT_TRUE(condition.isSatisfied(jets));
  /*
    std::cout << condition.toString() << '\n';
    std::cout << "jet Et: " << (m_jet0->p4()).Et()
    << " jet Eta: "<< m_jet0->p4().Eta() << '\n';
  */
}


TEST_F(EtaEtConditionTest, belowEtaMinCut) {
  EtaEtCondition condition(-0.5+0.001, 1., 100.);

  MockJetWithLorentzVector jet0{m_tl0};
  HypoJetVector jets{&jet0};

  EXPECT_CALL(jet0, et()); 
  EXPECT_CALL(jet0, eta());

  EXPECT_FALSE(condition.isSatisfied(jets));
}


TEST_F(EtaEtConditionTest, aboveEtaMaxCut) {
  EtaEtCondition condition(-1.0, 0.5-0.001, 100.);

  MockJetWithLorentzVector jet0{m_tl0};
  HypoJetVector jets{&jet0};

  EXPECT_CALL(jet0, et()); 
  EXPECT_CALL(jet0, eta());

  EXPECT_FALSE(condition.isSatisfied(jets));
}


TEST_F(EtaEtConditionTest, belowEtCut) {
  EtaEtCondition condition(-1.0, 1.0, 100.001);

  MockJetWithLorentzVector jet0{m_tl0};
  HypoJetVector jets{&jet0};

  EXPECT_CALL(jet0, et()); 
  EXPECT_CALL(jet0, eta());

  EXPECT_FALSE(condition.isSatisfied(jets));
}
