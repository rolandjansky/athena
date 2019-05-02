/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     MaximumBipartiteGroupsMatcherMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "./MaximumBipartiteGroupsMatcherMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FordFulkerson.h"
#include "./ITrigJetHypoInfoCollector.h"
#include <cmath>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <map>

MaximumBipartiteGroupsMatcherMT::MaximumBipartiteGroupsMatcherMT(const ConditionsMT& cs):
  m_conditions(cs), 
  m_pass(false),
  m_G(FlowNetwork(0)){
  // FIXME enforce !m_conditions.empty();
  if (!m_conditions.empty()){
    m_compound = m_conditions.front().capacity() > 1;
  } 
}
  
bool
MaximumBipartiteGroupsMatcherMT::match(const HypoJetGroupCIter& groups_b,
                                       const HypoJetGroupCIter& groups_e,
                                       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
				       bool debug){
  /* setup a FlowNetwork. 
     if group size == 1:
     nodes include source, conditions, jets groups, sink
     otherwise:
     nodes include source, conditions, jets groups, jets, sink
     
     Then solve the network (FordFulkerson)
  */
  
  auto out = std::unique_ptr<std::ofstream>(nullptr);
  if (debug){
    std::string fn = "MatcherDebug.log";
    out.reset(new std::ofstream(fn));
    *out << "sent 0\n";
  }
  
  if(m_conditions.empty()){
    if(collector){
      collector->collect("MaximumBipartiteGroupsMatcherMT", "No conditions");
    }
    return false;
  }

  
  // Determine jet group - condition satisfaction.
  
  constexpr double unitCapacity = 1.0;
  if(out){*out << "sent 100\n";}

  if((groups_e - groups_b) <= 0) {
    if(collector){
      collector->collect("MaximumBipartiteGroupsMatcherMT", "No jet groups");
    }
    return false;
  }

  if(out){*out << "sent 120\n";}

  std::map<int, const HypoJetVector*>jet_groups;  //index - jet group for matches
  std::vector<std::shared_ptr<FlowEdge>> edges;
  std::size_t jetGroupOffset = m_conditions.size();
  std::size_t icond{0};

  std::size_t sourceCapacity{0};  // used for viability check before running FF.
  
  for(const auto& cond : m_conditions){
    ++icond;
    bool c_is_satisfied = false;
    // jetgroup node numbers follow immediately after the condition node
    // numbers jet each group is considered n times where n is the
    // number of satisfied groups (alg bails out if there is an unsatisfoed
    // group).
    std::size_t ijg{jetGroupOffset};
    for (auto jg = groups_b; jg != groups_e; ++jg){
      ++ijg;
      if (cond.isSatisfied(*jg, collector)){
	// add edge source - condition once only.
	if(!c_is_satisfied){
	  c_is_satisfied = true;
	  // add source-condition link if condition satisfied
	  edges.push_back(std::make_shared<FlowEdge>(0,
						     icond,
						     cond.capacity()));
	  sourceCapacity += cond.capacity();
	  if(collector){
	    std::stringstream ss;
	    ss << 0 << "->" << icond << '\n';
	    collector->collect("MatcherMT", ss.str());
	  }
	}

	// keep track of satisfying groups for later expansion.
	jet_groups[ijg] = &(*jg);  
	// add condition- group link if condition satisfied
	edges.push_back(std::make_shared<FlowEdge>(icond,
						   ijg,
						   cond.capacity()));
	if(collector){
	  std::stringstream ss;
	  ss << icond << "->" << ijg << '\n';
	  collector->collect("MatcherMT", ss.str());
	}
      }
    }
    
    if(out){*out << "sent 300\n";}
    if(!c_is_satisfied){
      if(collector){
	collector->collect("MaximumBipartiteGroupsMatcherMT",
			   "Unsatisfied Condition" + cond.toString());
      }
      return false;
    }
  }
  if(out){*out << "sent 500\n";}

  //contains jets: jet - node for jets found in matching groups (nb type order)
  std::map<pHypoJet, int> jets; 

  // node = 0 for the source node.
  int jnode{jetGroupOffset + (groups_e-groups_b)};

  if(collector){
    collector->collect("MaximumBipartiteGroupsMatcherMT",
		       "Νο. of matched jet groups: " +
		       std::to_string(jet_groups.size()) + "\n");
  }

  // now add the jetgroup - jet edges
  std::size_t sinkCapacity{0};
  for(const auto& jg : jet_groups){
    for(const auto& j : *(jg.second)){

      auto iter = jets.find(j);
      if (iter == jets.end()){
	jets[j] = ++jnode;
	edges.push_back(std::make_shared<FlowEdge>(jg.first,
						   jnode,
						   unitCapacity));
	++sinkCapacity;
	if(collector){
	  std::stringstream ss;
	  ss << jg.first << "->" << jnode << '\n';
	  collector->collect("MatcherMT", ss.str());
	}
      } else {
	edges.push_back(std::make_shared<FlowEdge>(jg.first,
						   iter->second,
						   unitCapacity));
	++sinkCapacity;
	if(collector){
	  std::stringstream ss;
	  ss << jg.first << "->" << iter->first << '\n';
	  collector->collect("MatcherMT", ss.str());
	}
      }
    }
  }

  if(sinkCapacity < sourceCapacity){
    // no hope of matching
     if(collector){
       std::string msg =
	 "source capacity > sink capacity: " +
	 std::to_string(sourceCapacity) + " " +
	 std::to_string(sinkCapacity);
       
       collector->collect("MaximumBipartiteGroupsMatcherMT", msg);
     }
     return false;
  }

  if(collector){
    collector->collect("MaximumBipartiteGroupsMatcherMT - fn before",
		       "Νο of matched jets in matched groups: " + std::to_string(jets.size())+ "\n");
  }
  if(out){*out << "sent 600\n";}
  
  int V = jnode + 2;  // now we have all the nodes, can index the sink
  if(out){*out << "sent 600 " << V << '\n';}

  // finally add the jet - sink edges.
  for(const auto j: jets){
    edges.push_back(std::make_shared<FlowEdge>(j.second,
					       V-1,
					       unitCapacity));
    if(collector){
      std::stringstream ss;
      ss << j.first << "->" << V-1 << '\n';
      collector->collect("MatcherMT", ss.str());
    }
  }

  if(out){*out << "sent 700 " << V << "\n";}
  
  m_G = FlowNetwork(V);
  for(const auto & edge: edges){
    m_G.addEdge(edge);
  }
  
  if(collector){
    std::stringstream ss;
    ss << m_G;
    collector->collect("MaximumBipartiteGroupsMatcher -before", ss.str());
  }
		     
  if(out){*out << "sent 800\n";}
  
  FordFulkerson ff (m_G, 0, V-1);

  if(collector){
    std::stringstream ss;
    ss << m_G;
    collector->collect("MaximumBipartiteGroupsMatcher - after", ss.str());
  }
  if(out){*out << "sent 900\n";}
  
  double totalCapacity = 0;
  for(const auto& cond : m_conditions){
    totalCapacity += cond.capacity();
  }
  bool pass = std::round(ff.value()) == totalCapacity;
  
  
  // loop over edges, figure out if it is a condition - jet edge
  // figure out if it the jet is used (flow == 1)
  // add the jet to passing_jets. As this is a set, duplicates
  // will be removed (each edge appears twice in G).
  
  HypoJetVector passing_jets;
  std::map<int, pHypoJet> nodeToJet;

  // record which jets participate in the pass decision
  auto mapend = nodeToJet.end();
  
  for(const auto& nj : jets){
    nodeToJet.insert(std::map<int, pHypoJet>::value_type(nj.second,
							 nj.first));
  }
  for(auto edge : m_G.edges()){
    auto nj = nodeToJet.find(edge->to());  // index corresponds ot jet?
    if (nj == mapend){continue;} // no 
    if (std::round(edge->flow()) == 1){ // jet participates in hypo decision?
      passing_jets.push_back(nj->second);
    }
  }
  
  return pass;
  
}


std::string MaximumBipartiteGroupsMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "MaimumBipartiteMatcherMT\n";
  ss << "ConditionsMT:\n";

  for(auto c : m_conditions){ ss << c.toString() << '\n';}
  ss << "FlowNetwork:\n";
  ss << m_G << '\n';
  return ss.str();
}


ConditionsMT MaximumBipartiteGroupsMatcherMT::getConditions() const noexcept {
  return m_conditions;
}


