/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SelectedJetsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionBridge.h"
#include "./MockJetWithLorentzVector.h"
#include "./MockCondition.h"
#include "./TLorentzVectorFactory.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <TLorentzVector.h>
#include <memory>
#include <iostream>


/*
 * SelectedJetsMatcher functionality tested:
 * 0 fail if no jet vector indices
 * 1 fail if no if jets is less than the number of jet vector indices
 * 2 all pass: no of passing jets == number of input jets
 * 3 all fail: no of failing jets == number of input jets
 * 4 pass result reported is as expected
 * 5 fail result reported is as expected
 * 6 check that multiple Conditions  work as expected.
 * 7 pass result independance of order in which the indices are passed in
 *
 * Mocked objects:
 * - jet: will be ordered on ET, need TLorentzVector
 * - ICondition
 */

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;


class SelectedJetsMatcherTest: public ::testing::Test {
public:
  virtual void SetUp() {
    /* create the compnents needed to instantiate a SelectedJetsMatcher */

    auto factory = TLorentzVectorFactory();
    
    m_eta0 = 0.5;
    m_eta1 = -0.5;
    m_eta2 = 1.5;
    m_eta2 = 1.3;
    m_et0 = 100.;
    m_et1 = 100.;
    m_et2 = 150.;
    m_et3 = 180.;


    m_tl0 = factory.make(m_eta0, m_et0);
    m_tl1 = factory.make(m_eta1, m_et1);
    m_tl2 = factory.make(m_eta2, m_et2);
    m_tl3 = factory.make(m_eta3, m_et3);

    m_jet0 = new MockJetWithLorentzVector(m_tl0);
    m_jet1 = new MockJetWithLorentzVector(m_tl1);
    m_jet2 = new MockJetWithLorentzVector(m_tl2);
    m_jet3 = new MockJetWithLorentzVector(m_tl3);

    m_vjets.push_back(m_jet1);
    m_vjets.push_back(m_jet3);
    m_vjets.push_back(m_jet2);
    m_vjets.push_back(m_jet0);

    m_eps = 0.00001;
    m_masseps = 0.01;

  }

  virtual void TearDown() {
    delete m_jet0;
    delete m_jet1;
    delete m_jet2;
    delete m_jet3;
  }


  TLorentzVector m_tl0;
  TLorentzVector m_tl1;
  TLorentzVector m_tl2;
  TLorentzVector m_tl3;
  HypoJet::IJet* m_jet0;
  HypoJet::IJet* m_jet1;
  HypoJet::IJet* m_jet2;
  HypoJet::IJet* m_jet3;
  HypoJetVector m_vjets;

  double m_eta0;
  double m_eta1;
  double m_eta2;
  double m_eta3;
  double m_et0;
  double m_et1;
  double m_et2;
  double m_et3;
  double m_eps;
  double m_masseps;

  Conditions m_conditions;
  std::vector<unsigned int> m_indices;
  HypoJetVector m_jets;
};


TEST_F(SelectedJetsMatcherTest, zeroInputIndices){
  /* test with 0 jets - fails, no passed for failed jets */
 
 std::string name{"zeroInputIndicies"};
 EXPECT_THROW(SelectedJetsMatcher matcher(m_conditions, m_indices, name),
	      std::out_of_range);
}


TEST_F(SelectedJetsMatcherTest, zeroInputJets){
  /* test with 0 jets - fails, no passed for failed jets */
 
 std::string name{"zeroInputJets"};
 m_indices.push_back(0);
 SelectedJetsMatcher matcher(m_conditions, m_indices, name);
  
  matcher.match(m_jets.begin(), m_jets.end());
  
  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(0));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(0));
}


TEST_F(SelectedJetsMatcherTest, tooFewInputJets){
  /* pass dewer jets than indicies. Fails, all jets are bad */
 
  std::string name{"toFewInputJets"};
  m_indices.push_back(1);
  m_indices.push_back(2);
  SelectedJetsMatcher matcher(m_conditions, m_indices, name);
  
  m_jets.push_back(m_jet0);
  matcher.match(m_jets.begin(), m_jets.end());
  
  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(0));
  EXPECT_EQ(matcher.failedJets().size(), m_jets.size());
}


TEST_F(SelectedJetsMatcherTest, passesJets1and2){
  /* pass 3 jets, select 1, 2; passes. indexed jets are passdt jets */
 
  std::string name{"passesJeta1And2"};
  m_indices.push_back(1);
  m_indices.push_back(2);

  m_jets.push_back(m_jet0);
  m_jets.push_back(m_jet1);
  m_jets.push_back(m_jet2);


  MockCondition* condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_)).WillOnce(Return(true));

  std::shared_ptr<ICondition> pCondition(condition);

  m_conditions.push_back(ConditionBridge(pCondition));
  
  SelectedJetsMatcher matcher(m_conditions, m_indices, name);
  
  matcher.match(m_jets.begin(), m_jets.end());

  EXPECT_TRUE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(2));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(0));
  
}


TEST_F(SelectedJetsMatcherTest, passesJets0and1){
  /* pass 3 jets, select 0, 1; passes. indexed jets are passed jets */
 
  std::string name{"passesJeta1And2"};
  m_indices.push_back(0);
  m_indices.push_back(1);

  m_jets.push_back(m_jet0);
  m_jets.push_back(m_jet1);
  m_jets.push_back(m_jet2);


  MockCondition* condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_)).WillOnce(Return(true));

  std::shared_ptr<ICondition> pCondition(condition);

  m_conditions.push_back(ConditionBridge(pCondition));
  
  SelectedJetsMatcher matcher(m_conditions, m_indices, name);
  
  matcher.match(m_jets.begin(), m_jets.end());

  EXPECT_TRUE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(2));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(0));
  
}


TEST_F(SelectedJetsMatcherTest, failsJets0and1){
  /* pass 3 jets, select 0, 1; fails. indexed jets are failed jets */
 
  std::string name{"passesJeta1And2"};
  m_indices.push_back(0);
  m_indices.push_back(1);

  m_jets.push_back(m_jet0);
  m_jets.push_back(m_jet1);
  m_jets.push_back(m_jet2);


  MockCondition* condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_)).WillOnce(Return(false));

  std::shared_ptr<ICondition> pCondition(condition);

  m_conditions.push_back(ConditionBridge(pCondition));
  
  SelectedJetsMatcher matcher(m_conditions, m_indices, name);
  
  matcher.match(m_jets.begin(), m_jets.end());

  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(0));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(2));
  
}


TEST_F(SelectedJetsMatcherTest, twoConditionsPass1){
  /* pass 3 jets, select 0, 1; two conditions, The second passes. 
     indexed jets are passed jets */
 
  std::string name{"passesJeta1And2"};
  m_indices.push_back(0);
  m_indices.push_back(1);

  m_jets.push_back(m_jet0);
  m_jets.push_back(m_jet1);
  m_jets.push_back(m_jet2);

  MockCondition* condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_)).WillOnce(Return(false));
  std::shared_ptr<ICondition> pCondition0(condition);

  condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_)).WillOnce(Return(true));
  std::shared_ptr<ICondition> pCondition1(condition);

  m_conditions.push_back(ConditionBridge(pCondition0));
  m_conditions.push_back(ConditionBridge(pCondition1));
  
  SelectedJetsMatcher matcher(m_conditions, m_indices, name);
  
  matcher.match(m_jets.begin(), m_jets.end());

  EXPECT_TRUE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(2));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(0));
  
}



TEST_F(SelectedJetsMatcherTest, twoConditionsFail){
  /* pass 3 jets, select 0, 1; two conditions, both fail. 
     indexed jets are failed jets */
 
  std::string name{"passesJeta1And2"};
  m_indices.push_back(0);
  m_indices.push_back(1);

  m_jets.push_back(m_jet0);
  m_jets.push_back(m_jet1);
  m_jets.push_back(m_jet2);

  MockCondition* condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_)).WillOnce(Return(false));
  std::shared_ptr<ICondition> pCondition0(condition);

  condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_)).WillOnce(Return(false));
  std::shared_ptr<ICondition> pCondition1(condition);

  m_conditions.push_back(ConditionBridge(pCondition0));
  m_conditions.push_back(ConditionBridge(pCondition1));
  
  SelectedJetsMatcher matcher(m_conditions, m_indices, name);
  
  matcher.match(m_jets.begin(), m_jets.end());

  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(0));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(2));
  
}



TEST_F(SelectedJetsMatcherTest, twoConditionsReverseIndexOrder){
  /* pass 3 jets, select 0, 1; two conditions, The second passes. 
     indexed jets are passed jets */
 
  std::string name{"passesJeta1And2"};
  m_indices.push_back(1);
  m_indices.push_back(0);

  m_jets.push_back(m_jet0);
  m_jets.push_back(m_jet1);
  m_jets.push_back(m_jet2);

  MockCondition* condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_)).WillOnce(Return(false));
  std::shared_ptr<ICondition> pCondition0(condition);

  condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_)).WillOnce(Return(true));
  std::shared_ptr<ICondition> pCondition1(condition);

  m_conditions.push_back(ConditionBridge(pCondition0));
  m_conditions.push_back(ConditionBridge(pCondition1));
  
  SelectedJetsMatcher matcher(m_conditions, m_indices, name);
  
  matcher.match(m_jets.begin(), m_jets.end());

  EXPECT_TRUE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(2));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(0));
  
}




TEST_F(SelectedJetsMatcherTest, checkJetOdering){
  /* pass 3 jets out of ordered jets; fails check they are ordered
   by the matcher */
 
  std::string name{"passesJeta1And2"};
  m_indices.push_back(0);
  m_indices.push_back(1);

  // choose jets with different Et values
  m_jets.push_back(m_jet1);
  m_jets.push_back(m_jet2);
  m_jets.push_back(m_jet3);

  MockCondition* condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_)).WillOnce(Return(true));

  std::shared_ptr<ICondition> pCondition(condition);

  m_conditions.push_back(ConditionBridge(pCondition));

  SelectedJetsMatcher matcher(m_conditions, m_indices, name);

  // jets out of order befor matching
  EXPECT_LT((m_jets[0]->p4()).Et(), (m_jets[1]->p4()).Et());
  EXPECT_LT((m_jets[1]->p4()).Et(), (m_jets[2]->p4()).Et());
  
  matcher.match(m_jets.begin(), m_jets.end());

  EXPECT_TRUE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(2));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(0));

  // jets orderedby matcher
  EXPECT_GT((m_jets[0]->p4()).Et(), m_jets[1]->p4().Et());
  EXPECT_GT((m_jets[1]->p4()).Et(), m_jets[2]->p4().Et());
  
}
