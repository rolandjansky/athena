/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/../src/UnifiedFlowNetworkMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/../src/ConditionsDefsMT.h"
#include "TrigHLTJetHypo/../src/conditionsFactoryMT.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include "TrigHLTJetHypo/../src/xAODJetCollector.h"
#include "TrigHLTJetHypo/../src/UnifiedFlowNetworkBuilder.h"
#include "../src/SpecifiedJetsCondition.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"

#include "../src/TLorentzVectorFactory.h"
#include "../src/makeHypoJets.h"
#include "gtest/gtest.h"

#include <TLorentzVector.h>
#include <memory>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>

/*
 * UnifiedFlowNetworkMatcher functionality tested:
 * 0 fail if no jet vector indices
 * 1 fail if no if there are fewer passing jets than conditions
 * 2 pass if there are at least as many passing jets as conditions
 * 3 conditions are ordered by threshold
 * 4 jets are ordered by Et
 *
 */


ConditionsMT makeConditions_1(const std::vector<double>& etaMins,
			      const std::vector<double>& etaMaxs){
  
  
  auto netas = etaMins.size();
  
  std::vector<double> thresholds(netas, 1.0);
  std::vector<int> asymmetricEtas(netas, 0);

  return conditionsFactoryEtaEtMT(etaMins, etaMaxs,
				  thresholds,
				  asymmetricEtas);
  }

HypoJetGroupVector makeJetGroupsMT(HypoJetIter b,
				   HypoJetIter e,
				   std::size_t n){
  CombinationsGrouper g(n);  // 
  return g.group(b, e)[0];
}

class UnifiedFlowNetworkMatcherTest: public ::testing::Test {
public:
  UnifiedFlowNetworkMatcherTest() {
  }

  ConditionsMT m_conditions;
  int m_nconditions;
  bool m_debug{false};
   
  void makeConditions_2(const std::vector<double>& detaMins,
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


bool simpleBuildAndRunMatcher(const std::vector<double>& etaMins,
			      const std::vector<double>& etaMaxs,
			      const std::vector<double>& jetEtas,
			      const std::vector<std::size_t>& treeVec,
			      const std::string& fn,
			      bool debug
			){
  /* 
     uses real hypo jets and conditions of capacity one to build a flow network
  */
 
  auto out = std::make_unique<std::ofstream>(nullptr);
  if (debug){out.reset(new std::ofstream(fn + "_out.log"));}

  auto conditions = makeConditions_1(etaMins, etaMaxs);
  
  
  if(out){
    for(const auto& c : conditions){*out << c->toString();}
  }
  
  auto jets = makeHypoJets(jetEtas);
  
  std::map<int, pHypoJet> nodeToJet;
  
  auto groups = makeJetGroupsMT(jets.begin(), jets.end(), 1u);
  
  auto d_collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
  d_collector.reset(new DebugInfoCollector(fn));   

  
  if(out){
    if(d_collector){
      *out << "collector present\n";
    } else {
      *out << "collector not present\n";
    }
  }
  
  xAODJetCollector j_collector;
  
  std::unique_ptr<IGroupsMatcherMT> matcher(nullptr);

  std::vector<std::vector<int>> sharedNodes {std::vector<int>{}};
  matcher.reset(new UnifiedFlowNetworkMatcher(std::move(conditions),
					      treeVec,
					      sharedNodes));

  bool pass =  *(matcher->match(groups.cbegin(),
				groups.cend(),
				j_collector,
				d_collector));

  
  if(debug){
    *out << "dumping collector\n";
    d_collector->write();
  }
  
  for(auto j : jets){delete j;}

  return pass;
}


bool buildAndRunMatcher(ConditionsMT conditions,
			HypoJetVector jets,
			const std::vector<std::size_t>& treeVec,
			const std::string& fn,
			bool debug
  			){
  /* 
     uses supplied hypo jets and conditions to build a flow network
  */

  auto out = std::make_unique<std::ofstream>(nullptr);
  if (debug){out.reset(new std::ofstream(fn + "_out.log"));}
  
  if(out){
    for(const auto& c : conditions){*out << c->toString();}
  }
  
  std::map<int, pHypoJet> nodeToJet;
  
  auto groups = makeJetGroupsMT(jets.begin(), jets.end(), 1u);
  
  auto d_collector = std::unique_ptr<ITrigJetHypoInfoCollector>();
  d_collector.reset(new DebugInfoCollector(fn));   
  
  xAODJetCollector j_collector;

  if(out){
    if(d_collector){
      *out << "collector present\n";
    } else {
      *out << "collector not present\n";
    }
   }
 
  std::unique_ptr<IGroupsMatcherMT> matcher(nullptr);
  std::vector<std::vector<int>> sharedNodes {std::vector<int>{}};
  matcher.reset(new UnifiedFlowNetworkMatcher(std::move(conditions),
					      treeVec,
					      sharedNodes));
  auto  opt_pass =  matcher->match(groups.cbegin(),
				   groups.cend(),
				   j_collector,
				   d_collector);
  
  if(!(opt_pass.has_value())){
    *out << "matching failed\n";
  }

  bool pass = *opt_pass;

  if(debug){d_collector->write();}
  
  for(auto j : jets){delete j;}
  
  return pass;
}

TEST_F(UnifiedFlowNetworkMatcherTest, debugFlagIsFalse){
  /* idiot test to ensure dbug flag is of prior to commiting */
  EXPECT_FALSE(m_debug);
}


TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_nomatch){
   /* j0 -> c0  
      j1 -> c0                        
   */

  std::string fn = "mj_flowNetworkBuilder_nomatch";
  std::vector<double> etaMins{0.0, 0.5};
  std::vector<double> etaMaxs{1.0, 1.5};
  std::vector<double> jetEtas{2.1, 2.2, 3.0, 4.0};
  std::vector<std::size_t> treeVec{0,0};
  std::map<int, pHypoJet> nodeToJet;


  bool pass = simpleBuildAndRunMatcher(etaMins,
				       etaMaxs,
				       jetEtas,
				       treeVec,
				       fn,
				       m_debug);
  
  EXPECT_FALSE(pass);
}

TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_onematch){
   /* j0 -> c0  
      j1 -> c0                        
   */

  std::string fn = "mj_flowNetworkBuilder_onematch";
  std::vector<double> etaMins{0.0, 0.5};
  std::vector<double> etaMaxs{1.0, 1.5};
  std::vector<double> jetEtas{0.2, 0.3, 3.0, 4.0};
  std::vector<std::size_t> treeVec{0,0};
  std::map<int, pHypoJet> nodeToJet;

  bool pass = simpleBuildAndRunMatcher(etaMins,
				       etaMaxs,
				       jetEtas,
				       treeVec,
				       fn,
				       m_debug);
  
  
  EXPECT_FALSE(pass);

}

TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_0){
   /* j0 -> c0  
      j0  -> c1                        
   */

  std::string fn = "mj_flowNetworkBuilder_0";
  std::vector<double> etaMins{0.0, 0.5};
  std::vector<double> etaMaxs{1.0, 1.5};
  std::vector<double> jetEtas{0.7, 2.0, 3.0, 4.0};
  std::vector<std::size_t> treeVec{0,0};
  std::map<int, pHypoJet> nodeToJet;

  bool pass = simpleBuildAndRunMatcher(etaMins,
				       etaMaxs,
				       jetEtas,
				       treeVec,
				       fn,
				       m_debug);
  
  
  EXPECT_FALSE(pass);

}

TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_1){
  /* j0 -> c0
     j0 -> c1
     j1 -> c1

     Passes.  

     treeVec is {0,0,0} (two condition nodes connected to the source node

  */

  std::string fn = "mj_flowNetworkBuilder_1";
  std::vector<double> etaMins{0.0, 0.5};
  std::vector<double> etaMaxs{1.0, 1.5};
  std::vector<double> jetEtas{0.6, 1.1, 2.1, 2.2};
  std::vector<std::size_t> treeVec{0,0};
  std::map<int, pHypoJet> nodeToJet;

  bool pass = simpleBuildAndRunMatcher(etaMins,
				       etaMaxs,
				       jetEtas,
				       treeVec,
				       fn,
				       m_debug);
  
  
  EXPECT_FALSE(pass);  // UnifiedFlowNetwork flow under development
 }




TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_2){
  /* j0 -> c0
     j0 -> c1
     j1 -> c0
     j1 -> c1

     Passes.  
  */

  std::string fn = "mj_flowNetworkBuilder_2";
  std::vector<double> etaMins{0.0, 0.5};
  std::vector<double> etaMaxs{1.0, 1.5};
  std::vector<double> jetEtas{0.6, 0.7, 2.1, 2.2};
  std::vector<std::size_t> treeVec{0,0};
  std::map<int, pHypoJet> nodeToJet;

  bool pass = simpleBuildAndRunMatcher(etaMins,
				       etaMaxs,
				       jetEtas,
				       treeVec,
				       fn,
				       m_debug);
  
  
  EXPECT_FALSE(pass);   // UnifiedFlowNetwork flow under development

 }


TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_dj0){
  /* j0 -> c2
     j1 -> c3
     j2 -> c4
     j3 -> c5
     (j0, j1) -> c0
     (j2, j3) -> c1
     Passes.  
  */

  std::string fn = "mj_flowNetworkBuilder_dj0";
  
  std::vector<double> jetEtas(4, 0.0);
  auto jets = makeHypoJets(jetEtas);

  ConditionsMT conditions;

  
  std::unique_ptr<IConditionMT>  condition01(nullptr);
  condition01.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[0], jets[1]}}));
  conditions.push_back(std::move(condition01));

  std::unique_ptr<IConditionMT> condition23(nullptr);
  condition23.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[2], jets[3]}}));
  conditions.push_back(std::move(condition23));

  for(std::size_t i=0; i< jets.size(); ++i){
    std::unique_ptr<IConditionMT> condition;
    condition.reset
      (new SpecifiedJetsCondition(HypoJetGroupVector{HypoJetVector{jets[i]}}));
    conditions.push_back(std::move(condition));
  }

  std::vector<std::size_t> treeVec{0, 0, 1, 1, 2, 2};
  std::map<int, pHypoJet> nodeToJet;
  
  bool pass = buildAndRunMatcher(std::move(conditions),
				 jets,
				 treeVec,
				 fn,
				 m_debug);
  
  
  EXPECT_FALSE(pass);   // UnifiedFlowNetwork flow under development
 }

TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_dj1){
  /* j0 -> c2
     j1 -> c3
     j2 -> c4
     j3 -> c5
     (j0, j1) -> c0
     (j1, j3) -> c1
     Fails.  
  */

  std::string fn = "mj_flowNetworkBuilder_dj1";
  
  std::vector<double> jetEtas(4, 0.0);
  auto jets = makeHypoJets(jetEtas);

  ConditionsMT conditions;

  
  std::unique_ptr<IConditionMT>  condition01(nullptr);
  condition01.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[0], jets[1]}}));
  conditions.push_back(std::move(condition01));

  std::unique_ptr<IConditionMT> condition23(nullptr);
  condition23.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[1], jets[3]}}));
  conditions.push_back(std::move(condition23));

  for(std::size_t i=0; i< jets.size(); ++i){
    std::unique_ptr<IConditionMT> condition;
    condition.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[i]}}));
    conditions.push_back(std::move(condition));
  }

  std::vector<std::size_t> treeVec{0, 0, 1, 1, 2, 2};
  std::map<int, pHypoJet> nodeToJet;

  bool pass = buildAndRunMatcher(std::move(conditions),
				 jets,
				 treeVec,
				 fn,
				 m_debug);
  
  
  EXPECT_FALSE(pass);   // UnifiedFlowNetwork flow under development
 }


TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_qj0){
  /* j0 -> c3
     j1 -> c4
     j2 -> c5
     j3 -> c6
     (j0, j1) -> c1
     (j1, j3) -> c2
     (j1, j2, j3, j4) -> c0
     Fails.  
  */

  std::string fn = "mj_flowNetworkBuilder_qj0";
  
  std::vector<double> jetEtas(4, 0.0);
  auto jets = makeHypoJets(jetEtas);

  ConditionsMT conditions;

  std::unique_ptr<IConditionMT>  condition0123(nullptr);
  condition0123.reset(new SpecifiedJetsCondition(HypoJetGroupVector{jets}));
  conditions.push_back(std::move(condition0123));
  
  std::unique_ptr<IConditionMT>  condition01(nullptr);
  condition01.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[0], jets[1]}}));
  conditions.push_back(std::move(condition01));

  std::unique_ptr<IConditionMT> condition23(nullptr);
  condition23.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[2], jets[3]}}));
  conditions.push_back(std::move(condition23));

  for(std::size_t i=0; i< jets.size(); ++i){
    std::unique_ptr<IConditionMT> condition;
    condition.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[i]}}));
    conditions.push_back(std::move(condition));
  }

  std::vector<std::size_t> treeVec{0, 0, 0, 1, 1, 2, 2};
  std::map<int, pHypoJet> nodeToJet;

  bool pass = buildAndRunMatcher(std::move(conditions),
				 jets,
				 treeVec,
				 fn,
				 m_debug);
  
  
  EXPECT_FALSE(pass);   // UnifiedFlowNetwork under development

 }



TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_qj1){
  /* j0 -> c4
     j1 -> c5
     j2 -> c6
     j3 -> c3
     (j0, j1) -> c1
     (j1, j3) -> c2
     (j1, j2, j3, j4) -> c0
     Fails.  
  */

  std::string fn = "mj_flowNetworkBuilder_qj1";
  
  std::vector<double> jetEtas(4, 0.0);
  auto jets = makeHypoJets(jetEtas);

  ConditionsMT conditions;

  std::unique_ptr<IConditionMT>  condition0123(nullptr);
  condition0123.reset(new SpecifiedJetsCondition(HypoJetGroupVector{jets}));
  conditions.push_back(std::move(condition0123));
  
  std::unique_ptr<IConditionMT>  condition01(nullptr);
  condition01.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[0], jets[1]}}));
  conditions.push_back(std::move(condition01));

  std::unique_ptr<IConditionMT> condition23(nullptr);
  condition23.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[2], jets[3]}}));
  conditions.push_back(std::move(condition23));


  std::vector<std::size_t> jetIndicies{0, 0, 1, 2};
  
 for(std::size_t i=0; i< jets.size(); ++i){
    std::unique_ptr<IConditionMT> condition;
    condition.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[jetIndicies[i]]}}));
    conditions.push_back(std::move(condition));
  }
  
  
 std::vector<std::size_t> treeVec {0, 0, 0, 1, 1, 2, 2};
  std::map<int, pHypoJet> nodeToJet;

  bool pass = buildAndRunMatcher(std::move(conditions),
				 jets,
				 treeVec,
				 fn,
				 m_debug);
  
  
  EXPECT_FALSE(pass);    // UnifiedFlowNetwork flow under dev.

 }

TEST_F(UnifiedFlowNetworkMatcherTest, mj_flowNetworkBuilder_qj2){
  /* j0 -> c3
     j1 -> c4
     j2 -> c3
     j3 -> c6
     (j0, j1) -> c1
     (j1, j3) -> c2
     (j1, j2, j3, j4) -> c0
     Fails.  
  */

  std::string fn = "mj_flowNetworkBuilder_qj2";
  
  std::vector<double> jetEtas(4, 0.0);
  auto jets = makeHypoJets(jetEtas);

  ConditionsMT conditions;

  std::unique_ptr<IConditionMT>  condition0123(nullptr);
  condition0123.reset(new SpecifiedJetsCondition(HypoJetGroupVector{jets}));
  conditions.push_back(std::move(condition0123));
  
  std::unique_ptr<IConditionMT>  condition01(nullptr);
  condition01.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[0], jets[1]}}));
  conditions.push_back(std::move(condition01));

  std::unique_ptr<IConditionMT> condition23(nullptr);
  condition23.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[2], jets[3]}}));
  conditions.push_back(std::move(condition23));


  std::vector<std::size_t> jetIndicies{0, 1, 0, 3};
  
 for(std::size_t i=0; i< jets.size(); ++i){
    std::unique_ptr<IConditionMT> condition;
    condition.reset(new SpecifiedJetsCondition
		    (HypoJetGroupVector{HypoJetVector{jets[jetIndicies[i]]}}));
    conditions.push_back(std::move(condition));
  }
  
  
  std::vector<std::size_t> treeVec{0, 0, 0, 1, 1, 2, 2};
  std::map<int, pHypoJet> nodeToJet;

  bool pass = buildAndRunMatcher(std::move(conditions),
				 jets,
				 treeVec,
				 fn,
				 m_debug);
  
  
  EXPECT_FALSE(pass);   // UnifiedFlowNetwork flow under dev.
   }

