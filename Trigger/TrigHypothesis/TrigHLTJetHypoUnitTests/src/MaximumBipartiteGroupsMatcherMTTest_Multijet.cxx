/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/../src//MaximumBipartiteGroupsMatcherMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/../src/ConditionsDefsMT.h"
#include "TrigHLTJetHypo/../src/conditionsFactoryMT.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"

#include "./MockJetWithLorentzVector.h"
#include "./TLorentzVectorFactory.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <TLorentzVector.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>


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


class MaximumBipartiteGroupsMatcherMTTest_Multijet: public ::testing::Test {
public:
  MaximumBipartiteGroupsMatcherMTTest_Multijet() {
  }

  ConditionsMT m_conditions;
  int m_nconditions;
  bool m_debug{true};
  
  HypoJetGroupVector makeJetGroupsMT(HypoJetIter b, HypoJetIter e){
    CombinationsGrouper g(2);  // dijet groups
    return g.group(b, e);
  }

  void makeConditions(const std::vector<double>& detaMins,
                      const std::vector<double>& detaMaxs){
    std::vector<double> massMins{0., 0.};
    std::vector<double> massMaxs{
      std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(),
        };


    std::vector<double> dphiMins{0., 0.};
    std::vector<double> dphiMaxs{
      std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(),
        };


    m_conditions = conditionsFactoryDijetMT(massMins, massMaxs,
                                            detaMins, detaMaxs,
                                            dphiMins, dphiMaxs);
    m_nconditions = m_conditions.size();
  }
};


HypoJetVector makeHypoJets(const std::vector<double>& etas) {

  HypoJetVector jets;
  // jets.resize(etas.size());

  auto factory = TLorentzVectorFactory();

  // auto dst = jets.begin();
  auto make_jet = [&factory](double eta){
    return new MockJetWithLorentzVector(factory.make(eta, 10.));
  };

  std::transform(etas.begin(),
                 etas.end(),
                 std::back_inserter(jets),
                 make_jet);

  return jets;
}

TEST_F(MaximumBipartiteGroupsMatcherMTTest_Multijet, mj0){
  /* (j0, j1) -> c0
     (j0, j2) -> c0
     (j0, j1) -> c1
     Fails - j0 is shared.
  */

  auto out = std::make_unique<std::ofstream>(nullptr);
  if (m_debug){out.reset(new std::ofstream("Multijet_mj0.log"));}

  std::vector<double> detaMins{3.6, 5.5};
  
  std::vector<double> detaMaxs{
    std::numeric_limits<double>::max(),
      std::numeric_limits<double>::max(),
      };
  makeConditions(detaMins, detaMaxs);


  if(out){
    for(const auto& c : m_conditions){*out << c.toString();}
  }

  std::vector<double> etas{-5.0, 1.0, -1.0, -2.5};
  EXPECT_TRUE(etas.size() == 4);
  
  auto jets = makeHypoJets(etas);
  EXPECT_TRUE(jets.size() == 4);
  if(m_debug){
    for(const auto & j: jets){*out<< j << " " << j->toString() <<'\n';}
  }
  EXPECT_TRUE(m_conditions.size() == 2);
  MaximumBipartiteGroupsMatcherMT matcher(m_conditions);
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  EXPECT_TRUE(groups.size() == 6);
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
  collector.reset(new DebugInfoCollector("mj0"));
  
  bool pass = matcher.match(groups.begin(), groups.end(), collector);
  if (m_debug){collector->write();}

  for(auto j : jets){delete j;}
  EXPECT_FALSE(pass);
}


TEST_F(MaximumBipartiteGroupsMatcherMTTest_Multijet, mj1){
  /* (j0, j3) -> c0
     (j0, j1) -> c1
     Fails - shared j0.
  */

  auto out = std::make_unique<std::ofstream>(nullptr);
  if (m_debug){out.reset(new std::ofstream("Multijet_mj1.log"));}

  std::vector<double> detaMins{3.6, 5.5};
  
  std::vector<double> detaMaxs{4.5, 6.5};
  makeConditions(detaMins, detaMaxs);


  if(out){
    for(const auto& c : m_conditions){*out << c.toString();}
  }

  std::vector<double> etas{-5.0, 1.0, -1.0, -2.5};
  EXPECT_TRUE(etas.size() == 4);
  
  auto jets = makeHypoJets(etas);
  EXPECT_TRUE(jets.size() == 4);
  if(m_debug){
    for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
  }
  EXPECT_TRUE(m_conditions.size() == 2);
  MaximumBipartiteGroupsMatcherMT matcher(m_conditions);
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  EXPECT_TRUE(groups.size() == 6);
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();

  if(m_debug){
    collector.reset(new DebugInfoCollector("mj1"));
  }
  
  bool pass = matcher.match(groups.begin(), groups.end(), collector, m_debug);

  if(m_debug){collector->write();}
  
  for(auto j : jets){delete j;}
  EXPECT_FALSE(pass);
}


TEST_F(MaximumBipartiteGroupsMatcherMTTest_Multijet, mj2){
  /* (j0, j1) -> c0
     (j3, j4) -> c1
     Passes 
  */

  auto out = std::make_unique<std::ofstream>(nullptr);
  if (m_debug){out.reset(new std::ofstream("Multijet_mj2.log"));}

  std::vector<double> detaMins{0.0, 1.0};
  
  std::vector<double> detaMaxs{1.5, 2.0};
  makeConditions(detaMins, detaMaxs);


  if(out){
    for(const auto& c : m_conditions){*out << c.toString();}
  }

  std::vector<double> etas{-5.0, -4.9, 5.0, 3.25};
  EXPECT_TRUE(etas.size() == 4);
  
  auto jets = makeHypoJets(etas);
  EXPECT_TRUE(jets.size() == 4);
  if(m_debug){
    for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
  }
  EXPECT_TRUE(m_conditions.size() == 2);
  MaximumBipartiteGroupsMatcherMT matcher(m_conditions);
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  EXPECT_TRUE(groups.size() == 6);
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>(nullptr);

  if(m_debug){
    collector.reset(new DebugInfoCollector("mj2"));
  }
  
  bool pass = matcher.match(groups.begin(), groups.end(), collector, m_debug);

  if(m_debug){collector->write();}
  
  for(auto j : jets){delete j;}
  EXPECT_TRUE(pass);
}


TEST_F(MaximumBipartiteGroupsMatcherMTTest_Multijet, mj3){
  /* (j0, j1) -> c0
     (j0, j1) -> c1
     (j3, j4) -> c1
     Passes.
  */

  auto out = std::make_unique<std::ofstream>(nullptr);
  if (m_debug){out.reset(new std::ofstream("Multijet_mj3.log"));}

  std::vector<double> detaMins{0., 0.};
  
  std::vector<double> detaMaxs{1.0, 2.0};
  makeConditions(detaMins, detaMaxs);


  if(out){
    for(const auto& c : m_conditions){*out << c.toString();}
  }

  std::vector<double> etas{-5.0, -4.9, 4.0, 5.0};
  EXPECT_TRUE(etas.size() == 4);
  
  auto jets = makeHypoJets(etas);
  EXPECT_TRUE(jets.size() == 4);
  if(m_debug){
    for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
  }
  EXPECT_TRUE(m_conditions.size() == 2);
  MaximumBipartiteGroupsMatcherMT matcher(m_conditions);
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  EXPECT_TRUE(groups.size() == 6);
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
  collector.reset(new DebugInfoCollector("mj3"));
  
  bool pass = matcher.match(groups.begin(), groups.end(), collector);

  if(m_debug){collector->write();}
  
  for(auto j : jets){delete j;}
  EXPECT_TRUE(pass);
}


TEST_F(MaximumBipartiteGroupsMatcherMTTest_Multijet, mj4){
  /* (j0, j1) -> c0
     (j0, j1) -> c1
     (j3, j4) -> c0
     (j3, j4) -> c1

     Passes.
  */

  auto out = std::make_unique<std::ofstream>(nullptr);
  if (m_debug){out.reset(new std::ofstream("Multijet_mj4.log"));}

  std::vector<double> detaMins{0., 0.};
  
  std::vector<double> detaMaxs{1.0, 2.0};
  makeConditions(detaMins, detaMaxs);


  if(out){
    for(const auto& c : m_conditions){*out << c.toString();}
  }

  std::vector<double> etas{-5., -4.9, 4.9, 5.};
  EXPECT_TRUE(etas.size() == 4);
  
  auto jets = makeHypoJets(etas);
  EXPECT_TRUE(jets.size() == 4);
  if(m_debug){
    for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
  }
  EXPECT_TRUE(m_conditions.size() == 2);
  MaximumBipartiteGroupsMatcherMT matcher(m_conditions);
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  EXPECT_TRUE(groups.size() == 6);
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
  
  if(m_debug){
    collector.reset(new DebugInfoCollector("mj4"));
  }

  bool pass = matcher.match(groups.begin(), groups.end(), collector, m_debug);

  if(m_debug){collector->write();}
  
  for(auto j : jets){delete j;}
  EXPECT_TRUE(pass);
}

