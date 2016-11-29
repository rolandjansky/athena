/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/MaximumBipartiteGroupsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory.h"
#include "./MockJetWithLorentzVector.h"
#include "./TLorentzVectorFactory.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <TLorentzVector.h>
#include <memory>
#include <iostream>


/*
 * MaximumBipartiteMatcher functionality tested:
 * 0 fail if no jet vector indices
 * 1 fail if no if there are fewer passing jets than conditions
 * 2 pass if there are at least as many passing jets as conditions
 * 3 conditions are ordered by threshold
 * 4 jets are ordered by Et
 *
 * Mocked objects:
 * - jet: will be ordered on ET, need TLorentzVector, hence
 *        MockJetWithLorentzVector
 * - ICondition
 */

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;


class MaximumBipartiteGroupsMatcherTest: public ::testing::Test {
public:
  MaximumBipartiteGroupsMatcherTest() {


    std::vector<double> etaMins{-1., -1., -1.};
    std::vector<double> etaMaxs{1., 1, 1.};
    std::vector<double> thresholds{100., 120, 140.};

    m_conditions = conditionsFactory(etaMins, etaMaxs, thresholds);
    m_name = "testOrderedConditionsMatcher";
    m_nconditions = m_conditions.size();
  }

  Conditions m_conditions;
  int m_nconditions;
  std::string m_name;
};


TEST_F(MaximumBipartiteGroupsMatcherTest, zeroInputJets){
  /* test with 0 jets - fails, no passed for failed jets */

  HypoJetVector jets;
  MaximumBipartiteGroupsMatcher matcher(m_conditions, m_name);
  matcher.match(jets.begin(), jets.end());
  
  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(0));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(0));
}


TEST_F(MaximumBipartiteGroupsMatcherTest, tooFewSelectedJets){
  /* pass fewer jets than indicies. Fails, all jets are bad */

  double eta{5};
  double et{100.};

  auto factory = TLorentzVectorFactory();
  auto tl = factory.make(eta, et);

  MockJetWithLorentzVector jet0(tl);
  MockJetWithLorentzVector jet1{tl};

  HypoJetVector jets{&jet0, &jet1};

  MaximumBipartiteGroupsMatcher matcher(m_conditions, m_name);
  matcher.match(jets.begin(), jets.end());

  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(0));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(2));
}


TEST_F(MaximumBipartiteGroupsMatcherTest, oneSelectedJet){
  /* 1 jet1 over highest threshold - check good/bad jet list, fail. */

  double eta{5};
  double et{100.};

  auto factory = TLorentzVectorFactory();
  auto tl = factory.make(eta, et);

  MockJetWithLorentzVector jet0(tl);
  MockJetWithLorentzVector jet1{tl};
  MockJetWithLorentzVector jet2{tl};

  eta = 0;
  et = 150;
  auto tl0 = factory.make(eta, et);
  MockJetWithLorentzVector jet3{tl0};

  HypoJetVector jets{&jet0, &jet1, &jet2, &jet3};

  EXPECT_CALL(jet0, eta()).Times(m_nconditions);
  EXPECT_CALL(jet1, eta()).Times(m_nconditions);
  EXPECT_CALL(jet2, eta()).Times(m_nconditions);
  EXPECT_CALL(jet3, eta()).Times(m_nconditions);

  EXPECT_CALL(jet0, et()).Times(m_nconditions);
  EXPECT_CALL(jet1, et()).Times(m_nconditions);
  EXPECT_CALL(jet2, et()).Times(m_nconditions);
  EXPECT_CALL(jet3, et()).Times(m_nconditions);

  MaximumBipartiteGroupsMatcher matcher(m_conditions, m_name);
  matcher.match(jets.begin(), jets.end());

  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(1));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(3));
}


TEST_F(MaximumBipartiteGroupsMatcherTest, twoSelectedJets){
  /* 2 jets over repsective thresholds - check good/bad jet list, fail. */


  double eta{5};
  double et{100.};

  auto factory = TLorentzVectorFactory();
  auto tl0 = factory.make(eta, et);

  MockJetWithLorentzVector jet0(tl0);

  eta = 0.1;
  et = 139;
  auto tl1 = factory.make(eta, et);

  MockJetWithLorentzVector jet1{tl1};


  eta = 5.;
  et = 100.;
  auto tl2 = factory.make(eta, et);
  MockJetWithLorentzVector jet2{tl2};

  eta = 0;
  et = 150;
  auto tl3 = factory.make(eta, et);
  MockJetWithLorentzVector jet3{tl3};

  HypoJetVector jets{&jet0, &jet1, &jet2, &jet3};


  EXPECT_CALL(jet0, eta()).Times(m_nconditions);
  EXPECT_CALL(jet1, eta()).Times(m_nconditions);
  EXPECT_CALL(jet2, eta()).Times(m_nconditions);
  EXPECT_CALL(jet3, eta()).Times(m_nconditions);

  EXPECT_CALL(jet0, et()).Times(m_nconditions);
  EXPECT_CALL(jet1, et()).Times(m_nconditions);
  EXPECT_CALL(jet2, et()).Times(m_nconditions);
  EXPECT_CALL(jet3, et()).Times(m_nconditions);

  MaximumBipartiteGroupsMatcher matcher(m_conditions, m_name);
  matcher.match(jets.begin(), jets.end());

  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(2));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(2));
}


TEST_F(MaximumBipartiteGroupsMatcherTest, threeSelectedJets){
  /* 3 jets over repsective thresholds - check good/bad jet list, pass.
     Expect no failed jets (alg stops on success) and no checks on the
     unused jet*/

  double eta{5};
  double et{100.};

  auto factory = TLorentzVectorFactory();
  auto tl0 = factory.make(eta, et);

  MockJetWithLorentzVector jet0(tl0);

  eta = 0.1;
  et = 139;
  auto tl1 = factory.make(eta, et);

  MockJetWithLorentzVector jet1{tl1};


  eta = 0.5;
  et = 141.;
  auto tl2 = factory.make(eta, et);
  MockJetWithLorentzVector jet2{tl2};

  eta = -0.2;
  et = 101.;
  auto tl3 = factory.make(eta, et);
  MockJetWithLorentzVector jet3{tl3};

  HypoJetVector jets{&jet0, &jet1, &jet2, &jet3};


  EXPECT_CALL(jet1, eta()).Times(m_nconditions);
  EXPECT_CALL(jet2, eta()).Times(m_nconditions);
  EXPECT_CALL(jet3, eta()).Times(m_nconditions);
  EXPECT_CALL(jet0, eta()).Times(m_nconditions);

  EXPECT_CALL(jet1, et()).Times(m_nconditions);
  EXPECT_CALL(jet2, et()).Times(m_nconditions);
  EXPECT_CALL(jet3, et()).Times(m_nconditions);
  EXPECT_CALL(jet0, et()).Times(m_nconditions);

  MaximumBipartiteGroupsMatcher matcher(m_conditions, m_name);
  matcher.match(jets.begin(), jets.end());

  EXPECT_TRUE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(3));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(1));
}


TEST_F(MaximumBipartiteGroupsMatcherTest, fourSelectedJets){
  /* 4 jets over repsective thresholds - check good/bad jet list, pass.
     Expect no failed jets (alg stops on success) and no checks on the
     unused jet*/

  double eta{-0.6};
  double et{180.};

  auto factory = TLorentzVectorFactory();
  auto tl0 = factory.make(eta, et);

  MockJetWithLorentzVector jet0(tl0);

  eta = 0.1;
  et = 139;
  auto tl1 = factory.make(eta, et);

  MockJetWithLorentzVector jet1{tl1};


  eta = 0.5;
  et = 175.;
  auto tl2 = factory.make(eta, et);
  MockJetWithLorentzVector jet2{tl2};

  eta = -0.2;
  et = 101.;
  auto tl3 = factory.make(eta, et);
  MockJetWithLorentzVector jet3{tl3};

  HypoJetVector jets{&jet0, &jet1, &jet2, &jet3};

  EXPECT_CALL(jet0, eta()).Times(m_nconditions);
  EXPECT_CALL(jet1, eta()).Times(m_nconditions);
  EXPECT_CALL(jet2, eta()).Times(m_nconditions);
  EXPECT_CALL(jet3, eta()).Times(m_nconditions);

  EXPECT_CALL(jet0, et()).Times(m_nconditions);
  EXPECT_CALL(jet1, et()).Times(m_nconditions);
  EXPECT_CALL(jet2, et()).Times(m_nconditions);
  EXPECT_CALL(jet3, et()).Times(m_nconditions);

  MaximumBipartiteGroupsMatcher matcher(m_conditions, m_name);
  matcher.match(jets.begin(), jets.end());

  EXPECT_TRUE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(3));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(1));
}



TEST_F(MaximumBipartiteGroupsMatcherTest, overlappingEtaRegions){
  /* 4 jets over repsective thresholds - check good/bad jet list, pass.
     Expect no failed jets (alg stops on success) and no checks on the
     unused jet*/

  std::vector<double> etaMins{-1.00, -1.00, -0.25, -0.25};
  std::vector<double> etaMaxs{ 0.25,  0.25,  1.00,  1.00};
  std::vector<double> thresholds{100., 80, 140., 90.};

  auto conditions = conditionsFactory(etaMins, etaMaxs, thresholds);
  std::string name = "overlappingRegionsMatcher";
  int nconditions = conditions.size();

  double eta{0.1};
  double et{180.};

  auto factory = TLorentzVectorFactory();
  auto tl0 = factory.make(eta, et);

  MockJetWithLorentzVector jet0(tl0);

  eta = 0.1;
  et = 101;
  auto tl1 = factory.make(eta, et);

  MockJetWithLorentzVector jet1{tl1};


  eta = 0.1;
  et = 91.;
  auto tl2 = factory.make(eta, et);
  MockJetWithLorentzVector jet2{tl2};

  eta = 0.1;
  et = 81.;
  auto tl3 = factory.make(eta, et);
  MockJetWithLorentzVector jet3{tl3};

  HypoJetVector jets{&jet0, &jet1, &jet2, &jet3};

  EXPECT_CALL(jet0, eta()).Times(nconditions);
  EXPECT_CALL(jet1, eta()).Times(nconditions);
  EXPECT_CALL(jet2, eta()).Times(nconditions);
  EXPECT_CALL(jet3, eta()).Times(nconditions);

  EXPECT_CALL(jet0, et()).Times(nconditions);
  EXPECT_CALL(jet1, et()).Times(nconditions);
  EXPECT_CALL(jet2, et()).Times(nconditions);
  EXPECT_CALL(jet3, et()).Times(nconditions);

  MaximumBipartiteGroupsMatcher matcher(conditions, name);
  matcher.match(jets.begin(), jets.end());

  EXPECT_TRUE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(4));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(0));
}
