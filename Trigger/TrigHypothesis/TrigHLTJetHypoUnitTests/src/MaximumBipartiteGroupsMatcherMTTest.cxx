/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/../src//MaximumBipartiteGroupsMatcherMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/../src/ConditionsDefsMT.h"
#include "TrigHLTJetHypo/../src/conditionsFactoryMT.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include "TrigHLTJetHypo/../src/xAODJetCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"

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


class MaximumBipartiteGroupsMatcherMTTest: public ::testing::Test {
public:
  MaximumBipartiteGroupsMatcherMTTest() {


    std::vector<double> etaMins{-1., -1., -1.};
    std::vector<double> etaMaxs{1., 1, 1.};
    std::vector<double> thresholds{100., 120, 140.};
    std::vector<int> asymmetricEtas{0, 0, 0, 0};

    m_conditions = conditionsFactoryEtaEtMT(etaMins, etaMaxs,
                                            thresholds, asymmetricEtas);
    m_nconditions = m_conditions.size();
  }

  ConditionsMT m_conditions;
  int m_nconditions;
  bool m_debug{false};
};
HypoJetGroupVector
makeJetGroupsMT(HypoJetIter b, HypoJetIter e){
  CombinationsGrouper g(1);  // single jet groups
  return g.group(b, e)[0];
}

TEST_F(MaximumBipartiteGroupsMatcherMTTest, debugFlagIsFalse){
  /* idiot tst to ensure dbug flag is of prior to commiting */
   EXPECT_FALSE(m_debug);
}

TEST_F(MaximumBipartiteGroupsMatcherMTTest, zeroInputJets){
  /* test with 0 jets - fails, no passed for failed jets */

  MaximumBipartiteGroupsMatcherMT matcher(std::move(m_conditions));
  EXPECT_TRUE(true);
  HypoJetVector jets;
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  EXPECT_TRUE(true);
  auto visitor = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
  EXPECT_TRUE(true);

  xAODJetCollector jetCollector;
  auto pass = matcher.match(groups.begin(),
			    groups.end(),
			    jetCollector,
			    visitor,
			    false);

  EXPECT_TRUE(jetCollector.empty());
  EXPECT_FALSE(*pass);
}


TEST_F(MaximumBipartiteGroupsMatcherMTTest, tooFewSelectedJets){
  /* pass fewer jets than indicies. Fails, all jets are bad */

  double eta{5};
  double et{100.};

  auto factory = TLorentzVectorFactory();
  auto tl = factory.make(eta, et);

  MockJetWithLorentzVector jet0(tl);
  MockJetWithLorentzVector jet1{tl};

  EXPECT_CALL(jet0, eta()).Times(0);
  EXPECT_CALL(jet0, et()).Times(0);

  EXPECT_CALL(jet1, eta()).Times(0);
  EXPECT_CALL(jet1, et()).Times(0);
 
  HypoJetVector jets{&jet0, &jet1};
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  auto visitor = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
  if(m_debug){
    visitor.reset(new DebugInfoCollector("toofewselectedjets"));
  }
  MaximumBipartiteGroupsMatcherMT matcher(std::move(m_conditions));

  xAODJetCollector jetCollector;

  auto pass = matcher.match(groups.begin(),
			    groups.end(),
			    jetCollector,
			    visitor);

  if(visitor){visitor->write();}
  EXPECT_TRUE(pass.has_value());

  EXPECT_TRUE(jetCollector.empty());
  EXPECT_FALSE(*pass);
}


TEST_F(MaximumBipartiteGroupsMatcherMTTest, oneSelectedJet){
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

  // eta(), et() will be called by each of the three conditions.
  
  EXPECT_CALL(jet0, eta()).Times(3);
  EXPECT_CALL(jet0, et()).Times(3);
  EXPECT_CALL(jet1, eta()).Times(3);
  EXPECT_CALL(jet1, et()).Times(3);
  EXPECT_CALL(jet2, eta()).Times(3);
  EXPECT_CALL(jet2, et()).Times(3);
  EXPECT_CALL(jet3, eta()).Times(3);
  EXPECT_CALL(jet3, et()).Times(3);
  
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
  if(m_debug){
    collector.reset(new DebugInfoCollector("oneSelectedjets"));
  }
								      

  EXPECT_TRUE(groups.end() - groups.begin() == 4);
  auto groups_b = groups.begin();
  auto groups_e = groups.end();
  MaximumBipartiteGroupsMatcherMT matcher(std::move(m_conditions));

  xAODJetCollector jetCollector;
  
  auto pass = matcher.match(groups_b, groups_e, jetCollector, collector);

  if(m_debug){collector->write();}
  
  EXPECT_TRUE(jetCollector.empty());
  EXPECT_FALSE(*pass);
}



TEST_F(MaximumBipartiteGroupsMatcherMTTest, twoSelectedJets){
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

  MaximumBipartiteGroupsMatcherMT matcher(std::move(m_conditions));
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  auto visitor = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  xAODJetCollector jetCollector;

  auto pass =
    matcher.match(groups.begin(), groups.end(), jetCollector, visitor);

  EXPECT_TRUE(jetCollector.empty());
  EXPECT_FALSE(*pass);
}


TEST_F(MaximumBipartiteGroupsMatcherMTTest, threeSelectedJets){
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

  MaximumBipartiteGroupsMatcherMT matcher(std::move(m_conditions));
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());

  auto visitor = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
  if(m_debug){
    visitor.reset(new DebugInfoCollector("threeSelectedjets"));
  }

  xAODJetCollector jetCollector;

  auto pass =
    matcher.match(groups.begin(), groups.end(), jetCollector, visitor);

  if(m_debug){visitor->write();}
  
  EXPECT_TRUE(jetCollector.empty());  // not xAOD jets
  EXPECT_TRUE(*pass);
}


TEST_F(MaximumBipartiteGroupsMatcherMTTest, fourSelectedJets){
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

  MaximumBipartiteGroupsMatcherMT matcher(std::move(m_conditions));
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
  
  if(m_debug){
    collector.reset(new DebugInfoCollector("fourSelectedJets"));
  }

  xAODJetCollector jetCollector;
  
  auto pass = matcher.match(groups.begin(),
			    groups.end(),
			    jetCollector,
			    collector, m_debug);


  if(m_debug){collector->write();}
  
  EXPECT_TRUE(jetCollector.empty());  // not xOAD jets
  EXPECT_TRUE(*pass);
}



TEST_F(MaximumBipartiteGroupsMatcherMTTest, overlappingEtaRegions){
  /* 4 jets over repsective thresholds - check good/bad jet list, pass.
     Expect no failed jets (alg stops on success) and no checks on the
     unused jet*/

  std::vector<double> etaMins{-1.00, -1.00, -0.25, -0.25};
  std::vector<double> etaMaxs{ 0.25,  0.25,  1.00,  1.00};
  std::vector<double> thresholds{100., 80, 140., 90.};
  std::vector<int> asymmetricEtas{0, 0, 0, 0};

  auto conditions = conditionsFactoryEtaEtMT(etaMins, etaMaxs,
                                             thresholds, asymmetricEtas);
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

  MaximumBipartiteGroupsMatcherMT matcher(std::move(conditions));
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  
  auto visitor = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  xAODJetCollector jetCollector;

  if(m_debug){
    visitor.reset(new DebugInfoCollector("overlappingEtaRegions"));
  }
  
  auto pass = matcher.match(groups.begin(),
			    groups.end(),
			    jetCollector,
			    visitor);
  
  if(visitor){visitor -> write();}

  EXPECT_TRUE(jetCollector.empty());  // not xAOD jets
  EXPECT_TRUE(*pass);
}
