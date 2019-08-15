/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./UnifiedFlowNetworkBuilder.h"
#include "./JetGroupProduct.h"
#include <map>
#include <algorithm>
#include <sstream>


UnifiedFlowNetworkBuilder::UnifiedFlowNetworkBuilder(ConditionsMT conditions,
						     const TreeVec& treeVec):
  FlowNetworkBuilderBase(std::move(conditions)), m_tree(treeVec){
}

std::optional<std::vector<std::shared_ptr<FlowEdge>>>
UnifiedFlowNetworkBuilder::make_flowEdges(const HypoJetGroupCIter& groups_b,
                                           const HypoJetGroupCIter& groups_e, 
                                           const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                                           int& V,
                                           std::map<int, pHypoJet>& nodeToJet) const{


  std::size_t cur_jg = m_tree.size() - 1;  // (first node - 1)  for jets

  // container for edges to be constructed.
  std::vector<std::shared_ptr<FlowEdge>> edges;
  std::vector<std::shared_ptr<FlowEdge>> leafToJobGroupInEdges;  


  // Flow network to be build as source -> conditiosnANDjetGroups ->sink.
  // conditionsANDJEt jet groups are alternating layers of conditions
  // and jet groups. The first layer is made up of conditions, and the last
  // is made up of jet groups. The last set of conditions are called "leaves"
  // the first set is called "first generation". Intermediate layer
  // jet groups are calculated according to the jet groups accepted by
  // the previous layer of conditions.
  //
  // At this point, connect the sink to the  first generation conditions.
  for(const auto& i : m_tree.firstGeneration()){
    edges.push_back(std::make_shared<FlowEdge>(0,
					       i,
					       m_conditions[i]->capacity()));
  }

  // keep track of which jet groups satisfy condition (by node id)
  std::map<std::size_t, HypoJetGroupVector> satisfiedBy;

  auto leaves = m_tree.leaves();

  // first edges - single jets - single jet conditions
  // keep track of input jet groups to be connected toi the sink node.
  // we do not knoiw the sink node ID yet.
  std::vector<std::size_t> toSink;

  // node number for contributing JetGroups
  using IDJetGroup = std::map<std::size_t, HypoJetVector>;
  IDJetGroup jetgroups;
  
  for(auto iter = groups_b; iter != groups_e; ++iter){
    auto jg = *iter;

    if(jg.size() != 1){
      
      // input job groups of size 1 (for now....). This used
      // to establish which jets participated in a passing hypo.
      std::optional<std::vector<std::shared_ptr<FlowEdge>>>();
    }
    
    for(const auto& l: leaves){
      if (m_conditions[l]->isSatisfied(jg, collector)){
	++cur_jg;
	satisfiedBy[l].push_back(jg);
	toSink.push_back(cur_jg);
	jetgroups.insert(IDJetGroup::value_type(cur_jg, jg));
	auto edge = std::make_shared<FlowEdge>(l, cur_jg, jg.size());
	edges.push_back(edge);
	nodeToJet[cur_jg] = jg[0];
      }
    }
  }


  // construct job groups according from job groups that pass child
  // conditions. The parent of the next condition to be processed
  // is found, and from the parent the condition's siblings are found,
  // all combinations if the job groups passing the sibings are
  // constructed. One by one these combinations are tested for
  // parent satisfaction. If the parent is satisfied, it is placed in the
  // queue for later processing.
  std::queue<std::size_t> to_process;  // conditions to be processed.

  // keep track if a condition's sibling has been processed.
  std::vector<bool> checked(m_conditions.size(), false);

  // initialise the queue with satisfied leaf conditions.
  for(const auto& l : leaves){to_process.push(l);}

  while(!to_process.empty()){
    auto k = to_process.front();
    if (checked[k]){
      // condition taken into account when process one of its siblings.
      to_process.pop();
      continue;
    }

    // find the siblings of the condition being processed.
    auto siblings = m_tree.getSiblings(k);

    // mark the siblings as having been processed.
    for(const auto& s : siblings){checked[s] = true;}

    // get the passing job group(s) for each satisfied sibling
    std::vector<HypoJetGroupVector> sibling_jgroups;

    
    for(const auto& s : siblings){
      sibling_jgroups.push_back(satisfiedBy[s]);
    }

    // calculate the external product of the job groups
    // eg if condition c1 is satisfied by jg11 and jg12, while its only
    // sibling c2 is satisfied by jg21, the external job groups are
    // jg11jg21, jg12jg21. Each of these  are flattened.
    auto jg_product = JetGroupProduct(sibling_jgroups);
    auto next = jg_product.next();
    auto par = m_tree.getParent(k);

    while (next.has_value()){
      auto jg = *next;
      if (m_conditions[par]->isSatisfied(jg, collector)){
	++cur_jg;
	satisfiedBy[par].push_back(jg);

	edges.push_back(std::make_shared<FlowEdge>(par, cur_jg, jg.size()));
	for(const auto& s : siblings){
	  edges.push_back(std::make_shared<FlowEdge>(cur_jg,
						     s,
						     m_conditions[s]->capacity()));}
	jetgroups.insert(std::map<int, HypoJetVector>::value_type(cur_jg, jg));
      }      
      
      next = jg_product.next();
    }
    to_process.push(par);
  
    to_process.pop();
  }
  V = ++cur_jg;
  auto& sink = V;
  for(const auto& n : toSink){
    edges.push_back(std::make_shared<FlowEdge>(n, sink, 1));
  }

						
  return std::make_optional<std::vector<std::shared_ptr<FlowEdge>>>(edges);
}


