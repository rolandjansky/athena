/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/../src/PartitionsGroupsMatcherMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/PartitionsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/../src/ConditionsDefsMT.h"
#include "TrigHLTJetHypo/../src/conditionsFactoryMT.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include "TrigHLTJetHypo/../src/xAODJetCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"

#include "../src/TLorentzVectorAsIJet.h"
#include "../src/TLorentzVectorFactory.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <TLorentzVector.h>
#include <memory>
#include <iostream>


/*
 * PartitionsMatcher functionality tested:
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


class PartitionsGroupsMatcherMTTest: public ::testing::Test {
public:
  PartitionsGroupsMatcherMTTest() {


    std::vector<double> etaMins{-1., -1., -1.};
    std::vector<double> etaMaxs{1., 1, 1.};
    std::vector<double> thresholds{100., 120., 140.};
    std::vector<int> asymmetricEtas{0, 0, 0};

    m_conditions = conditionsFactoryEtaEtMT(etaMins, etaMaxs,
                                            thresholds, asymmetricEtas);
    m_nconditions = m_conditions.size();
  }

  ConditionsMT m_conditions;
  int m_nconditions;
  bool m_debug{false};
};

std::vector<HypoJetGroupVector>
makejetGroupsPartitionMT(HypoJetIter b, HypoJetIter e){
  std::vector<std::size_t> mults{1, 1};
  PartitionsGrouper g(mults);  // single jet groups
  return g.group(b, e);
}

TEST_F(PartitionsGroupsMatcherMTTest, debugFlagIsFalse){
  /* idiot tst to ensure dbug flag is of prior to commiting */
   EXPECT_FALSE(m_debug);
}

TEST_F(PartitionsGroupsMatcherMTTest, zeroInputJets){
  /* test with 0 jets - fails, no passed for failed jets */

  PartitionsGroupsMatcherMT matcher(std::move(m_conditions));
  // 
  EXPECT_TRUE(true);
  HypoJetVector jets;
  auto groups = makejetGroupsPartitionMT(jets.begin(), jets.end());
  EXPECT_TRUE(groups.empty());
}


TEST_F(PartitionsGroupsMatcherMTTest, tooFewSelectedJets){
   /* pass fewer jets than indicies. Fails, all jets are bad */
 
   double eta{5};
   double et{100.};
 
   auto factory = TLorentzVectorFactory();
   auto tlv = factory.make(eta, et);

   auto tl_j = std::make_shared<const TLorentzVectorAsIJet>(tlv);
   HypoJetVector jets;
   jets.push_back(tl_j);

   auto groupsVec = makejetGroupsPartitionMT(jets.begin(), jets.end());
   auto visitor = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);
   if(m_debug){
     visitor.reset(new DebugInfoCollector("toofewselectedjets"));
   }

   PartitionsGroupsMatcherMT matcher(std::move(m_conditions));
 
   xAODJetCollector jetCollector;

   for (const auto& groups : groupsVec){
     auto pass = matcher.match(groups.begin(),
			       groups.end(),
			       jetCollector,
			       visitor);
     
     //f(visitor){visitor->write();}
     EXPECT_TRUE(pass.has_value());
     EXPECT_FALSE(*pass);
     EXPECT_TRUE(jetCollector.empty());

     if(visitor){visitor->write();}

   }
 }
 

TEST_F(PartitionsGroupsMatcherMTTest, PassingJets){
   /* pass fewer jets than indicies. Fails, all jets are bad */


  std::vector<double> ets {200., 220., 225.};
  double eta{0.5};
  
  HypoJetVector jets;
  jets.resize(ets.size());

  auto factory = TLorentzVectorFactory();
  
  auto makeJetFromEt = [&factory, eta](double et){
    return std::make_shared<const TLorentzVectorAsIJet>(factory.make(eta, et));
  };

  std::transform(ets.begin(),
		 ets.end(),
		 jets.begin(),
		 makeJetFromEt);

  EXPECT_EQ(ets.size(), jets.size());
  EXPECT_EQ(m_conditions.size(), 3u);
  // --------
  std::vector<std::size_t> mults{1, 1, 1};
  PartitionsGrouper g(mults);  // single jet groups

  auto b = jets.begin();
  auto e = jets.end();
  auto groupsVec = g.group(b, e);
  //{[(1),(2), (3)], [(1), (3), (2)], [(2), (1), (3)], [(2), (3), (1)],
  // [(3), (1), (2)], [(3), (2), (1)]}
  EXPECT_EQ(groupsVec.size(), 6u);
  
  auto visitor = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);


  if(m_debug){
    visitor.reset(new DebugInfoCollector("PassingJetsTest"));
  }

  PartitionsGroupsMatcherMT matcher(std::move(m_conditions));

  for (const auto& groups : groupsVec){
    EXPECT_EQ(groups.size(), 3);
    xAODJetCollector jetCollector;

    auto pass = matcher.match(groups.begin(),
			      groups.end(),
			      jetCollector,
			      visitor);
    EXPECT_TRUE(pass.has_value());
    EXPECT_TRUE(*pass);
    EXPECT_EQ(jetCollector.size(), 9);  // non-xAOD HypoJets now collected
  }

  if(visitor){visitor->write();}

}
 


TEST_F(PartitionsGroupsMatcherMTTest, Passing3Failing1){
   /* pass fewer jets than indicies. Fails, all jets are bad */


  std::vector<double> ets {200., 220., 225., 50};
  double eta{0.5};
  
  HypoJetVector jets;
  jets.resize(ets.size());

  auto factory = TLorentzVectorFactory();
  
  auto makeJetFromEt = [&factory, eta](double et){
    return std::make_shared<const TLorentzVectorAsIJet>(factory.make(eta, et));
  };

  std::transform(ets.begin(),
		 ets.end(),
		 jets.begin(),
		 makeJetFromEt);

  EXPECT_EQ(ets.size(), jets.size());
  EXPECT_EQ(m_conditions.size(), 3u);

  std::vector<std::size_t> mults{1, 1, 1};
  PartitionsGrouper g(mults);  // single jet groups

  auto b = jets.begin();
  auto e = jets.end();
  auto groupsVec = g.group(b, e);
  auto visitor = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  if(m_debug){
    visitor.reset(new DebugInfoCollector("Passing3Failing1Test"));
  }
  PartitionsGroupsMatcherMT matcher(std::move(m_conditions));
  xAODJetCollector jetCollector;

  std::size_t npass{0};
  std::size_t ncall{0};

  for (const auto& groups : groupsVec){
    ++ncall;
    auto pass = matcher.match(groups.begin(),
			      groups.end(),
			      jetCollector,
			      visitor);
    if(pass.has_value()){
      if(*pass){++npass;}
    }
  }

  // calls: 4 jets, conditions need three: 4.3.2  = 24
  // pass: ignore failing jet. 3.2.1 = 6
  EXPECT_EQ(npass,  6u);
  EXPECT_EQ(ncall, 24u);  
}
 
