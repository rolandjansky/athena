/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./FlowNetworkBuilderBase.h"
#include <map>
#include <algorithm>
#include <sstream>


FlowNetworkBuilderBase::FlowNetworkBuilderBase(ConditionsMT conditions):
  m_conditions(std::move(conditions)){
}


std::optional<std::unique_ptr<FlowNetwork>> 
FlowNetworkBuilderBase::create(const HypoJetGroupCIter& groups_b,
			       const HypoJetGroupCIter& groups_e,
			       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
			       std::map<int, pHypoJet>& nodeToJet) const {

  int V{0};

  auto edges = make_flowEdges(groups_b, groups_e, collector, V, nodeToJet);
  if(!edges.has_value()){return std::optional<std::unique_ptr<FlowNetwork>>();}

  auto G = std::make_unique<FlowNetwork>(V);

  for(const auto & edge: (*edges)){G->addEdge(edge);}

  return std::make_optional<std::unique_ptr<FlowNetwork>>(std::move(G));

}


std::optional<std::vector<std::vector<int>>>
FlowNetworkBuilderBase::conditionGroupMatches(const HypoJetGroupCIter& groups_b,
					      const HypoJetGroupCIter& groups_e,
					      const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const 
{

  // return type: std::vector<int, std::vector<int>>
  // Outer vector: m_conditions index (conditions node - 1)
  // Inner vector: index numbers of the matching job groups.
  //               (node - m_conditions.size()).

  std::vector<std::vector<int>> result;

  if(collector){collector->collect("FlowNetworkBuilder", "start satisfsaction checks");}
     
  for(const auto& cond : m_conditions){
    std::vector<int> groups;
    // jetgroup node numbers follow immediately after the condition node
    // numbers jet each group is considered n times where n is the
    // number of satisfied groups if no early return.

    auto ijg{0};
    for (auto jg = groups_b; jg != groups_e; ++jg){
      if (cond->isSatisfied(*jg, collector)){
	if (collector){
	  collector->collect("FlowNetworkBuilder",
			     "Satisfied Condition jet node "
			     + std::to_string(ijg) + " " + cond->toString());
	}
	groups.push_back(ijg);
	// add source-condition link if condition satisfied
      }
      ++ijg;
    }
    if(groups.empty()){
      if(collector){
        collector->collect("FlowNetworkBuilder",
                           "Unsatisfied Condition" + cond->toString());
      }
      return std::optional<std::vector<std::vector<int>>>();     
    }
      
    result.push_back(groups);
  }
  return std::make_optional<std::vector<std::vector<int>>>(result);
}

std::vector<std::shared_ptr<FlowEdge>>
FlowNetworkBuilderBase::getSourceToConditionsEdges() const {
  std::vector<std::shared_ptr<FlowEdge>> initialEdges;
  auto icond{0};
  for(const auto& cond : m_conditions){
    initialEdges.push_back(std::make_shared<FlowEdge>(0,
						      ++icond,
						      cond->capacity()));
  }
  return initialEdges;
}


std::string FlowNetworkBuilderBase::toString() const {
  std::stringstream ss;
  ss << "FlowNetworkBuilderBase Conditions ["
     << m_conditions.size() << "]: \n";
  for(const auto& c : m_conditions){
    ss << c->toString() + '\n';
  }
  return ss.str();
}
