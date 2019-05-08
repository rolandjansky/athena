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
  m_pass(false){
  // FIXME enforce !m_conditions.empty();
  if (!m_conditions.empty()){
    m_compound = m_conditions.front().capacity() > 1;
  }
  
  // set up the source to conditions flowEdges, which never change.
  std::size_t icond{0};
  for(const auto& condition: m_conditions){
    auto edge = std::make_shared<FlowEdge>(0,
					   ++icond,
					   condition.capacity());
    m_srcToCondEdges.push_back(edge);
  }
}

bool
MaximumBipartiteGroupsMatcherMT::match(const HypoJetGroupCIter& groups_b,
                                       const HypoJetGroupCIter& groups_e,
                                       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
				       bool debug) const {
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
  }
  
  if(m_conditions.empty()){
    if(collector){
      collector->collect("MaximumBipartiteGroupsMatcherMT", "No conditions");
    }
    return false;
  }

  
  // Determine jet group - condition satisfaction.
  
  if((groups_e - groups_b) <= 0) {
    if(collector){
      collector->collect("MaximumBipartiteGroupsMatcherMT", "No jet groups");
    }
    return false;
  }

  int V{0};
  std::map<int, pHypoJet> nodeToJet; 

  auto edges = make_flowEdges(groups_b, groups_e, collector, V, nodeToJet);
  if(!edges.has_value()){return false;}
  FlowNetwork G(V);
  for(const auto & edge: (*edges)){
    G.addEdge(edge);
  }

  		     
  FordFulkerson ff (G, 0, V-1);
  
  if(collector){
    std::stringstream ss;
    ss << G;
    collector->collect("MaximumBipartiteGroupsMatcher - after", ss.str());
  }
  
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
  
  auto iter = std::partition(edges->begin(),
                             edges->end(),
                             [V](const auto& edge){return edge->to() == V-1 and
                                                   std::round(edge->flow()) == 1;});
  
  std::transform(edges->begin(),
                 iter,
		 std::back_inserter(passing_jets),
		 [&nodeToJet](const auto& edge){return nodeToJet[edge->from()];});
	       
  
  return pass;
  
}


std::string MaximumBipartiteGroupsMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "MaimumBipartiteMatcherMT\n";
  ss << "ConditionsMT:\n";

  for(auto c : m_conditions){ ss << c.toString() << '\n';}
  ss << "FlowNetwork:\n";
  return ss.str();
}


ConditionsMT MaximumBipartiteGroupsMatcherMT::getConditions() const noexcept {
  return m_conditions;
}


std::optional<std::vector<std::vector<int>>>
MaximumBipartiteGroupsMatcherMT::conditionGroupMatches(const HypoJetGroupCIter& groups_b,
						       const HypoJetGroupCIter& groups_e,
						       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const 
{

  // return type: std::vector<int, std::vector<int>> outer vector is m_conditions index (conditions node - 1)
  // index numbers of the matching job groups. (node - m_conditions.size()).

  std::vector<std::vector<int>> result;
  // result.reserve(m_conditions.size());
  if(collector){collector->collect("MatcherMT", "start satisfsaction checks");}
     
  for(const auto& cond : m_conditions){
    std::vector<int> groups;
    // jetgroup node numbers follow immediately after the condition node
    // numbers jet each group is considered n times where n is the
    // number of satisfied groups if no early return.

    auto ijg{0};
    for (auto jg = groups_b; jg != groups_e; ++jg){
      if (cond.isSatisfied(*jg, collector)){
	if (collector){
	  collector->collect("MaximumBipartiteGroupsMatcherMT",
			     "Satisfied Condition " + std::to_string(ijg) + " " +
			     cond.toString());
	}
	groups.push_back(ijg);
	// add source-condition link if condition satisfied
      }
      ++ijg;
    }
    if(groups.empty()){
      if(collector){
	collector->collect("MaximumBipartiteGroupsMatcherMT",
			   "Unsatisfied Condition" + cond.toString());
      }
      return std::optional<std::vector<std::vector<int>>>();     
    }
      
    result.push_back(groups);
  }
  return std::make_optional<std::vector<std::vector<int>>>(result);
}

std::optional<std::vector<std::shared_ptr<FlowEdge>>>
MaximumBipartiteGroupsMatcherMT::make_flowEdges(const HypoJetGroupCIter& groups_b,
                                                const HypoJetGroupCIter& groups_e, 
                                                const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                                                int& V,
                                                std::map<int, pHypoJet>& nodeToJet) const{

  auto matches = conditionGroupMatches(groups_b, groups_e, collector);
  if(!matches.has_value()){
    // there is  a condition without any  matches.
    return std::optional<std::vector<std::shared_ptr<FlowEdge>>>();
  }

  // add edges from condition to matching job group
  auto icond{0};
  const auto jg_offset = m_conditions.size() + 1;
  
  std::vector<std::shared_ptr<FlowEdge>> edges(m_srcToCondEdges.begin(),
					       m_srcToCondEdges.end());
  
  for(const auto& jg_nodes : *matches){
    // icond is m_conditons index
    double cap = m_conditions[icond].capacity();  // icond becomes the condition node index

    auto  dest  = std::back_inserter(edges);
    std::transform(jg_nodes.begin(),
		   jg_nodes.end(),
		   dest, 
		   [icond, jg_offset, cap](const auto& g) {
		     return std::make_shared<FlowEdge>(icond+1, g + jg_offset, cap);});
    ++icond;
  }
    

  //contains jets: jet - node for jets found in matching groups (nb type order)
  std::map<pHypoJet, int> jets; 

  int jnode{m_conditions.size() + (groups_e-groups_b)};

  // now add the jetgroup - jet edges
  std::size_t sinkCapacity{0};
  constexpr double unitCapacity = 1.0;
  
  for(const auto& jgnodes : *matches){
    // jgnodes - vector of jet group indicies
    for (auto jgnode : jgnodes){
      // jgnode - index in jet group collection
      for(const auto& j : *(groups_b + jgnode)){

	auto iter = jets.find(j);
	if (iter == jets.end()){
	  jets[j] = ++jnode;
	  edges.push_back(std::make_shared<FlowEdge>(jgnode + jg_offset,
						     jnode,
						     unitCapacity));
	  if(collector){
	    std::stringstream ss;
	    ss << jgnode << "->" << jnode << '\n';
	    collector->collect("MatcherMT", ss.str());
	  }
	} else {
	  edges.push_back(std::make_shared<FlowEdge>(jgnode +jg_offset,
						     iter->second,
						     unitCapacity));
	  if(collector){
	    std::stringstream ss;
	    ss << jgnode + jg_offset<< "->" << iter->second << '\n';
	    collector->collect("MatcherMT", ss.str());
	  }
	}
      }
    }
  }

  auto sourceCapacity = 0.;
  for(const auto& c : m_conditions){sourceCapacity += c.capacity();}
  sinkCapacity = jets.size();
  if(sinkCapacity < sourceCapacity){
    
    // no hope of matching
    if(collector){
      std::string msg =
	"source capacity > sink capacity: " +
	std::to_string(sourceCapacity) + " " +
	std::to_string(sinkCapacity);
      
      collector->collect("MaximumBipartiteGroupsMatcherMT", msg);
    }
    return std::optional<std::vector<std::shared_ptr<FlowEdge>>>();
  }

  if(collector){
    collector->collect("MaximumBipartiteGroupsMatcherMT - fn before",
		       "Νο of matched jets in matched groups: " + std::to_string(jets.size())+ "\n");
  }
  
  V = jnode + 2;  // now we have all the nodes, can index the sink

  // finally add the jet - sink edges.

  for(const auto j: jets){
      edges.push_back(std::make_shared<FlowEdge>(j.second,
						 V-1,
						 unitCapacity));

      // fill the node to jet map - to be used to identify jets
      // contirbuting to hypo success.
      //
      // NB: relies on the having no duplicates  in the values
      nodeToJet[j.second] = j.first;
      if(collector){
        std::stringstream ss;
        ss << j.first << "->" << V-1 << '\n';
        collector->collect("MatcherMT", ss.str());
      }
  }

  return std::make_optional<std::vector<std::shared_ptr<FlowEdge>>>(edges);

}
