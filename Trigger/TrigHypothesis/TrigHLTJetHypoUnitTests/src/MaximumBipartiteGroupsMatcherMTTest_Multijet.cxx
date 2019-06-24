/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/../src//MaximumBipartiteGroupsMatcherMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/../src/ConditionsDefsMT.h"
#include "TrigHLTJetHypo/../src/conditionsFactoryMT.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include "TrigHLTJetHypo/../src/xAODJetCollector.h"
#include "TrigHLTJetHypo/../src/MultijetFlowNetworkBuilder.h"
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
using ::testing::AnyNumber;


class MaximumBipartiteGroupsMatcherMTTest_Multijet: public ::testing::Test {
public:
  MaximumBipartiteGroupsMatcherMTTest_Multijet() {
  }

  ConditionsMT m_conditions;
  int m_nconditions;
  bool m_debug{false};
  
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



std::vector<MockJetWithLorentzVector*>
makeGMockJets(const std::vector<double>& etas) {

  std::vector<MockJetWithLorentzVector*> jets;
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

HypoJetVector makeHypoJets(const std::vector<double>& etas) {
  auto gmockJets = makeGMockJets(etas);
  HypoJetVector jets(gmockJets.begin(), gmockJets.end());
  return jets;
}


HypoJetVector makeHypoJets(const std::vector<MockJetWithLorentzVector*>& gj) {
  HypoJetVector jets(gj.begin(), gj.end());
  return jets;
}



TEST_F(MaximumBipartiteGroupsMatcherMTTest_Multijet, debugFlagIsFalse){
  /* idiot test to ensure dbug flag is of prior to commiting */
   EXPECT_FALSE(m_debug);
}

TEST_F(MaximumBipartiteGroupsMatcherMTTest_Multijet, mj_flowNetworkBuilder_0){
  /* (j0, j1) -> c0                                                                                        
     (j0, j2) -> c0                                                                                        
     (j0, j1) -> c1                                                                                        
     Fails - j0 is shared.                                                                                 
  */

  auto out = std::make_unique<std::ofstream>(nullptr);
  if (m_debug){out.reset(new std::ofstream("mj_flowNetworkBuilder_0.log"));}

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

    
  auto g_jets = makeGMockJets(etas);

  for(auto j: g_jets){
    EXPECT_CALL(*j, eta()).Times(AnyNumber());
  }

  auto jets = makeHypoJets(g_jets);

  EXPECT_TRUE(jets.size() == 4);
  if(m_debug){
    for(const auto & j: jets){*out<< j << " " << j->toString() <<'\n';}
  }
  EXPECT_TRUE(m_conditions.size() == 2);
  auto builder = std::unique_ptr<IFlowNetworkBuilder>(nullptr);
  builder.reset(new MultijetFlowNetworkBuilder(m_conditions));
  std::map<int, pHypoJet> nodeToJet;

  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  EXPECT_TRUE(groups.size() == 6);

  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();                                          
  collector.reset(new DebugInfoCollector("mj_flowNetworkBuilder_0"));   

  auto G = builder->create(groups.begin(), groups.end(), collector, nodeToJet);
  EXPECT_FALSE(G.has_value());

  for(auto j : jets){delete j;}
}

TEST_F(MaximumBipartiteGroupsMatcherMTTest_Multijet, mj_flowNetworkBuilder_1){
  /* (j0, j1) -> c0                                                                                        
     (j0, j1) -> c1                                                                                        
     (j3, j4) -> c1                                                                                        
     Passes.                                                                                               
  */

  auto out = std::make_unique<std::ofstream>(nullptr);
  if (m_debug){out.reset(new std::ofstream("mj_flowNetworkBuilder_1.log"));}

  std::vector<double> detaMins{0., 0.};

  std::vector<double> detaMaxs{1.0, 2.0};
  makeConditions(detaMins, detaMaxs);


  if(out){
    for(const auto& c : m_conditions){*out << c.toString();}
  }

  std::vector<double> etas{-5.0, -4.9, 3.9, 5.0};
  EXPECT_TRUE(etas.size() == 4);

    
  auto g_jets = makeGMockJets(etas);

  for(auto j: g_jets){
    EXPECT_CALL(*j, eta()).Times(AnyNumber());
  }

  auto jets = makeHypoJets(g_jets);
  EXPECT_TRUE(jets.size() == 4);
  if(m_debug){
    for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
  }
  EXPECT_TRUE(m_conditions.size() == 2);

  auto builder = std::unique_ptr<IFlowNetworkBuilder>(nullptr);
  builder.reset(new MultijetFlowNetworkBuilder(m_conditions));
  std::map<int, pHypoJet> nodeToJet;

  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  EXPECT_TRUE(groups.size() == 6);
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
  collector.reset(new DebugInfoCollector("mj_flowNetworkBuilder_1_collector"));

  auto G = builder->create(groups.begin(), groups.end(), collector, nodeToJet);
  EXPECT_TRUE(G.has_value());

  if(m_debug){
    std::stringstream ss;
    ss << **G << '\n'; 
    collector->collect("FlowNetwork", ss.str());
    collector->write();
  }

  EXPECT_TRUE((*G)->V() == 14);
  EXPECT_TRUE(((*G)->edges()).size() == 13);

  for(auto j : jets){delete j;}
}

TEST_F(MaximumBipartiteGroupsMatcherMTTest_Multijet, mj_flowNetworkBuilder_2){
  /* (j0, j1) -> c0                                                                                        
     (j0, j1) -> c1                                                                                        
     (j3, j4) -> c1                                                                                        
     Passes.                                                                         
     Test with no collector                      
  */

  auto out = std::make_unique<std::ofstream>(nullptr);
  if (m_debug){out.reset(new std::ofstream("mj_flowNetworkBuilder_2.log"));}

  std::vector<double> detaMins{0., 0.};

  std::vector<double> detaMaxs{1.0, 2.0};
  makeConditions(detaMins, detaMaxs);


  if(out){
    for(const auto& c : m_conditions){*out << c.toString();}
  }

  std::vector<double> etas{-5.0, -4.9, 3.9, 5.0};
  EXPECT_TRUE(etas.size() == 4);
  
  auto g_jets = makeGMockJets(etas);

  for(auto j: g_jets){
    EXPECT_CALL(*j, eta()).Times(AnyNumber());
  }

  auto jets = makeHypoJets(g_jets);

  EXPECT_TRUE(jets.size() == 4);
  if(m_debug){
    for(const auto & j: jets){*out<<j<< " " << j->toString() <<'\n';}
  }
  EXPECT_TRUE(m_conditions.size() == 2);

  auto builder = std::unique_ptr<IFlowNetworkBuilder>(nullptr);
  builder.reset(new MultijetFlowNetworkBuilder(m_conditions));
  std::map<int, pHypoJet> nodeToJet;

  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  EXPECT_TRUE(groups.size() == 6);
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
  // collector.reset(new DebugInfoCollector("mj_flowNetworkBuilder_2_collector"));

  auto G = builder->create(groups.begin(), groups.end(), collector, nodeToJet);
  EXPECT_TRUE(G.has_value());

  EXPECT_TRUE((*G)->V() == 14);
  EXPECT_TRUE(((*G)->edges()).size() == 13);

  for(auto j : jets){delete j;}
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
  
  auto g_jets = makeGMockJets(etas);

  for(auto j: g_jets){
    EXPECT_CALL(*j, eta()).Times(AnyNumber());
  }

  auto jets = makeHypoJets(g_jets);

  if(m_debug){
    for(const auto & j: jets){*out<< j << " " << j->toString() <<'\n';}
  }
   EXPECT_TRUE(jets.size() == 4);

  
  EXPECT_TRUE(m_conditions.size() == 2);
  MaximumBipartiteGroupsMatcherMT matcher(m_conditions);
  auto groups = makeJetGroupsMT(jets.begin(), jets.end());
  EXPECT_TRUE(groups.size() == 6);
  auto collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
  collector.reset(new DebugInfoCollector("mj0"));

  xAODJetCollector jetCollector;
  
  auto pass = matcher.match(groups.begin(),
			    groups.end(),
			    jetCollector,
			    collector);
  
  if (m_debug){collector->write();}

  for(auto j : jets){delete j;}

  EXPECT_TRUE(jetCollector.empty());
  EXPECT_FALSE(*pass);
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
  
 auto g_jets = makeGMockJets(etas);

  for(auto j: g_jets){
    EXPECT_CALL(*j, eta()).Times(AnyNumber());
  }

  auto jets = makeHypoJets(g_jets);

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

  xAODJetCollector jetCollector;

  auto pass = matcher.match(groups.begin(),
			    groups.end(),
			    jetCollector,
			    collector,
			    m_debug);

  if(m_debug){collector->write();}
  
  for(auto j : jets){delete j;}

  EXPECT_TRUE(jetCollector.empty());
  EXPECT_FALSE(*pass);
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


   
  auto g_jets = makeGMockJets(etas);

  for(auto j: g_jets){
    EXPECT_CALL(*j, eta()).Times(AnyNumber());
  }

  auto jets = makeHypoJets(g_jets);
  
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

  xAODJetCollector jetCollector;

  auto pass = matcher.match(groups.begin(),
			    groups.end(),
			    jetCollector,
			    collector,
			    m_debug);

  if(m_debug){collector->write();}
  
  for(auto j : jets){delete j;}

  EXPECT_TRUE(jetCollector.empty()); // not xAOD jets
  
  EXPECT_TRUE(*pass);
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

  auto g_jets = makeGMockJets(etas);

  for(auto j: g_jets){
    EXPECT_CALL(*j, eta()).Times(AnyNumber());
  }

  auto jets = makeHypoJets(g_jets);

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

  xAODJetCollector jetCollector;

  auto pass = matcher.match(groups.begin(),
			    groups.end(),
			    jetCollector,
			    collector);

  if(m_debug){collector->write();}
  
  for(auto j : jets){delete j;}
  EXPECT_TRUE(jetCollector.empty()); //not xAOD jets
  EXPECT_TRUE(*pass);
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



  auto g_jets = makeGMockJets(etas);

  for(auto j: g_jets){
    EXPECT_CALL(*j, eta()).Times(AnyNumber());
  }

  auto jets = makeHypoJets(g_jets);
  
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

  xAODJetCollector jetCollector;
  auto pass = matcher.match(groups.begin(),
			    groups.end(),
			    jetCollector,
			    collector,
			    m_debug);

  if(m_debug){collector->write();}
  
  for(auto j : jets){delete j;}

  EXPECT_TRUE(jetCollector.empty()); // not xAOD jets
  EXPECT_TRUE(*pass);
}

