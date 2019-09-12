/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/../src/MaximumBipartiteGroupsMatcherMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/../src/ConditionsDefsMT.h"
#include "TrigHLTJetHypo/../src/IGroupsMatcherMT.h"
#include "TrigHLTJetHypo/../src/conditionsFactoryMT.h"
#include "TrigHLTJetHypo/../src/xAODJetCollector.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"
#include "../src/MockJetWithLorentzVector.h"
#include "../src//TLorentzVectorFactory.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <TLorentzVector.h>
#include <memory>
#include <iostream>


/*
 * MaximumBipartiteMatcherMT functionality tested:
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
  ~MaximumBipartiteGroupsMatcherMTTest(){std::cerr<< " ~MaximumBipartiteGroupsMatcherMTTest()\n";}
  
  ConditionsMT m_conditions;
  int m_nconditions;
};


TEST_F(MaximumBipartiteGroupsMatcherMTTest, zeroInputJets){
  /* test with 0 jets - fails, no passed for failed jets */

  HypoJetGroupVector groups;

  std::unique_ptr<IGroupsMatcherMT> matcher(nullptr);

  matcher.reset(new MaximumBipartiteGroupsMatcherMT(std::move(m_conditions)));
  xAODJetCollector j_collector;
  std::unique_ptr<ITrigJetHypoInfoCollector> d_collector(nullptr);

  bool pass = *(matcher->match(groups.cbegin(), groups.cend(),
  			       j_collector, d_collector));


  EXPECT_FALSE(pass);
}


TEST_F(MaximumBipartiteGroupsMatcherMTTest, tooFewSelectedJets){
  /* pass fewer jets than indicies. Fails, all jets are bad */

  double eta{5};
  double et{100.};

  auto factory = TLorentzVectorFactory();
  auto tl = factory.make(eta, et);

  MockJetWithLorentzVector jet0(tl);
  MockJetWithLorentzVector jet1{tl};

  HypoJetVector jets{&jet0, &jet1};

  std::unique_ptr<IGroupsMatcherMT> matcher(nullptr);
  matcher.reset(new MaximumBipartiteGroupsMatcherMT(std::move(m_conditions)));


  xAODJetCollector j_collector;
  std::unique_ptr<ITrigJetHypoInfoCollector> d_collector(nullptr);

  auto grouper = SingleJetGrouper{};
  auto begin = jets.begin();
  auto end = jets.end();
  auto groups = grouper.group(begin, end)[0];
  
  bool pass = *(matcher->match(groups.begin(),
			       groups.end(),
			       j_collector,
			       d_collector));

  EXPECT_FALSE(pass);
  EXPECT_TRUE(j_collector.empty());

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
  et = 150000;
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

  std::unique_ptr<IGroupsMatcherMT> matcher(nullptr);
  matcher.reset(new MaximumBipartiteGroupsMatcherMT(std::move(m_conditions)));



  xAODJetCollector j_collector;
  std::unique_ptr<ITrigJetHypoInfoCollector> d_collector(nullptr);

  auto grouper = SingleJetGrouper{};
  auto begin = jets.begin();
  auto end = jets.end();
  auto groups = grouper.group(begin, end)[0];
  
  bool pass = *(matcher->match(groups.begin(),
			       groups.end(),
			       j_collector,
			       d_collector));
  
  EXPECT_FALSE(pass);
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

  std::unique_ptr<IGroupsMatcherMT> matcher(nullptr);
  matcher.reset(new MaximumBipartiteGroupsMatcherMT(std::move(m_conditions)));



  
  xAODJetCollector j_collector;
  std::unique_ptr<ITrigJetHypoInfoCollector> d_collector(nullptr);

  auto grouper = SingleJetGrouper{};
  auto begin = jets.begin();
  auto end = jets.end();
  auto groups = grouper.group(begin, end)[0];
  
  bool pass = *(matcher->match(groups.begin(),
			       groups.end(),
			       j_collector,
			       d_collector));
  

  EXPECT_FALSE(pass);

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

  std::unique_ptr<IGroupsMatcherMT> matcher(nullptr);
  matcher.reset(new MaximumBipartiteGroupsMatcherMT(std::move(m_conditions)));


  xAODJetCollector j_collector;
  std::unique_ptr<ITrigJetHypoInfoCollector> d_collector(nullptr);

  auto grouper = SingleJetGrouper{};
  auto begin = jets.begin();
  auto end = jets.end();
  auto groups = grouper.group(begin, end)[0];
  
  bool pass = *(matcher->match(groups.begin(),
			       groups.end(),
			       j_collector,
			       d_collector));

  EXPECT_TRUE(pass);
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

  std::unique_ptr<IGroupsMatcherMT> matcher(nullptr);
 matcher.reset(new MaximumBipartiteGroupsMatcherMT(std::move(m_conditions)));

  
  xAODJetCollector j_collector;
  std::unique_ptr<ITrigJetHypoInfoCollector> d_collector(nullptr);

  auto grouper = SingleJetGrouper{};
  auto begin = jets.begin();
  auto end = jets.end();
  auto groups = grouper.group(begin, end)[0];
  
  bool pass = *(matcher->match(groups.begin(),
			       groups.end(),
			       j_collector,
			       d_collector));
  
  EXPECT_TRUE(pass);
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
  std::string name = "overlappingRegionsMatcherMT";
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
  
  std::unique_ptr<IGroupsMatcherMT> matcher(nullptr);
  matcher.reset(new MaximumBipartiteGroupsMatcherMT(std::move(conditions)));

  xAODJetCollector j_collector;
  std::unique_ptr<ITrigJetHypoInfoCollector> d_collector(nullptr);
  // d_collector.reset(new DebugInfoCollector("overlapping"));

  auto grouper = SingleJetGrouper{};
  auto begin = jets.begin();
  auto end = jets.end();
  auto groups = grouper.group(begin, end)[0];
  
  bool pass = *(matcher->match(groups.begin(),
			       groups.end(),
			       j_collector,
			       d_collector));

  if(d_collector){
    d_collector->write();
  }
  
  EXPECT_TRUE(pass);
}
