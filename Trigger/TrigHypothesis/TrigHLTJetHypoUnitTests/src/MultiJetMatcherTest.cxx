/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/MultiJetMatcher.h"
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

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"
#include <iostream>
class CounterCondition: public ICondition{
public:
  bool isSatisfied(const HypoJetVector& v) const{
    std::cout << "CounterCondition Et values \n";
    for(auto j : v){std::cout<< (j->p4()).Et()<<"\n";}
    return true;
  }

  std::string toString() const noexcept{return "CounterCondition";}
  double orderingParameter() const noexcept{return 0;}
};

/*
 * MultiJetMatcher functionality tested:
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


class MultiJetMatcherTest: public ::testing::Test {
public:
  virtual void SetUp() {
    /* create the compnents needed to instantiate a MultiJetMatcher */

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

    m_eps = 0.001;
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


TEST_F(MultiJetMatcherTest, zeroLoopJets){
  /* test with 0 jets to loop over - matcher fails.
     Conditions fail, all jets are bad.
  */
 
  std::string name{"zeroInputIndicess"};
  m_jets.push_back(m_jet0);
 
  MultiJetMatcher matcher(m_conditions, 0, name);
  matcher.match(m_jets.begin(), m_jets.end());

  
  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(0));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(1));
}


TEST_F(MultiJetMatcherTest, noConditions){
  /*  no conditions: Matcher fails. All jets are bad.*/
 
  std::string name{"zeroInputIndicess"};
  m_jets.push_back(m_jet0);
 
  MultiJetMatcher matcher(m_conditions, 0, name);
  matcher.match(m_jets.begin(), m_jets.end());

  
  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(0));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(1));
}


TEST_F(MultiJetMatcherTest, threeChoosetwoJetsAccepts){
  /*  3 jets, choose 2. Expect Condtions to be called 3 times 
      All combinations set to pass */
 
  std::string name{"zeroInputIndicess"};
  m_jets.push_back(m_jet0);
  m_jets.push_back(m_jet1);
  m_jets.push_back(m_jet2);
 
   MockCondition* condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_))
    .Times(3)
    .WillRepeatedly(Return(true));

  /*
 CounterCondition* condition = new CounterCondition;
  */

  std::shared_ptr<ICondition> pCondition(condition);

  m_conditions.push_back(ConditionBridge(pCondition));
 
  EXPECT_TRUE((m_jets.end() - m_jets.begin()) == 3);
  MultiJetMatcher matcher(m_conditions, 2, name);
  matcher.match(m_jets.begin(), m_jets.end());

  
  EXPECT_TRUE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(3));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(0));
}



TEST_F(MultiJetMatcherTest, threeChoosetwoJetsFails){
  /*  3 jets, choose 2. Expect Condtions to be called 3 times 
      All combinations set to fail */
 
  std::string name{"zeroInputIndicess"};
  m_jets.push_back(m_jet0);
  m_jets.push_back(m_jet1);
  m_jets.push_back(m_jet2);
 
   MockCondition* condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_))
    .Times(3)
    .WillRepeatedly(Return(false));

  /*
 CounterCondition* condition = new CounterCondition;
  */

  std::shared_ptr<ICondition> pCondition(condition);

  m_conditions.push_back(ConditionBridge(pCondition));
 
  EXPECT_TRUE((m_jets.end() - m_jets.begin()) == 3);
  MultiJetMatcher matcher(m_conditions, 2, name);
  matcher.match(m_jets.begin(), m_jets.end());

  
  EXPECT_FALSE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(0));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(3));
}



TEST_F(MultiJetMatcherTest, fourChoosetwoJetsAccepts){
  /*  4 jets, choose 2. Expect Condtions to be called 6 times 
      All combinations set to pass */


  std::string name{"zeroInputIndicess"};
  m_jets.push_back(m_jet0);
  m_jets.push_back(m_jet1);
  m_jets.push_back(m_jet2);
  m_jets.push_back(m_jet3);
 
   MockCondition* condition = new MockCondition;
  EXPECT_CALL(*condition, isSatisfied(_))
    .Times(6)
    .WillRepeatedly(Return(true));

  /*
 CounterCondition* condition = new CounterCondition;
  */

  std::shared_ptr<ICondition> pCondition(condition);

  m_conditions.push_back(ConditionBridge(pCondition));
 
  EXPECT_TRUE((m_jets.end() - m_jets.begin()) == 4);
  MultiJetMatcher matcher(m_conditions, 2, name);
  matcher.match(m_jets.begin(), m_jets.end());

  
  EXPECT_TRUE(matcher.pass());
  EXPECT_EQ(matcher.passedJets().size(), static_cast<unsigned int>(4));
  EXPECT_EQ(matcher.failedJets().size(), static_cast<unsigned int>(0));
}
