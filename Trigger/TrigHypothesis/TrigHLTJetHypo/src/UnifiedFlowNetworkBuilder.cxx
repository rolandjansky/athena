/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./UnifiedFlowNetworkBuilder.h"
#include "./JetGroupProduct.h"
#include <map>
#include <algorithm>
#include <sstream>

#include <ostream>


UnifiedFlowNetworkBuilder::UnifiedFlowNetworkBuilder(ConditionsMT conditions,
						     const Tree& tree):
  FlowNetworkBuilderBase(std::move(conditions)), m_tree(tree){
}

std::optional<std::vector<std::shared_ptr<FlowEdge>>>
UnifiedFlowNetworkBuilder::make_flowEdges(const HypoJetGroupCIter& groups_b,
                                           const HypoJetGroupCIter& groups_e, 
                                           const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                                           int& V,
                                           std::map<int, pHypoJet>& nodeToJet) const{

  std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 0\n";
  std::size_t cur_jg = m_tree.size();  // (first node - 1)  for jets

  // container for edges to be constructed.
  std::vector<std::shared_ptr<FlowEdge>> edges;
  std::vector<std::shared_ptr<FlowEdge>> leafToJetGroupInEdges;  


  // Flow network to be build as source -> conditiosnANDjetGroups ->sink.
  // conditionsANDJEt jet groups are alternating layers of conditions
  // and jet groups. The first layer is made up of conditions, and the last
  // is made up of jet groups. The last set of conditions are called "leaves"
  // the first set is called "first generation". Intermediate layer
  // jet groups are calculated according to the jet groups accepted by
  // the previous layer of conditions.
  //
  // At this point, connect the sink to the  first generation conditions.
  std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 100\n";
  for(const auto& i : m_tree.firstGeneration()){
    std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 101 " << i <<'\n';
  }
  for(const auto& i : m_tree.firstGeneration()){
    edges.push_back(std::make_shared<FlowEdge>(0,
					       i,
					       //tree->conditions: -1
					       m_conditions[i-1]->capacity()));
  }
  std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 200\n";

  // create map to keep track of which jet groups
  // satisfy condition (by node id)
  std::map<std::size_t, HypoJetGroupVector> satisfiedBy;

  auto leaves = m_tree.leaves();  // indices into Conditions vector

  // first edges - single jets - single jet conditions
  // keep track of input jet groups to be connected toi the sink node.
  // we do not knoiw the sink node ID yet.
  std::set<std::size_t> toSink;

  // node number for contributing JetGroups
  using IDJetGroup = std::map<std::size_t, HypoJetVector>;
  IDJetGroup jetgroups;
  
  std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 300\n";
  for(auto iter = groups_b; iter != groups_e; ++iter){
    auto jg = *iter;

    if(jg.size() != 1){
      
      // input jet groups of size 1 (for now....). This used
      // to establish which jets participated in a passing hypo.
      return std::optional<std::vector<std::shared_ptr<FlowEdge>>>();
    }
    std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 400\n";

    bool jg_is_used{false};
    for(const auto& leaf: leaves){
      std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 410\n";
      if (m_conditions[leaf-1]->isSatisfied(jg, collector)){
	jg_is_used = true;
	std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 420\n";
	satisfiedBy[leaf-1].push_back(jg);  //leaf-1: conditions index
	std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 430\n";
	toSink.insert(cur_jg);
	std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 440\n";
	jetgroups.insert(IDJetGroup::value_type(cur_jg, jg));
	std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 450\n";
	auto edge = std::make_shared<FlowEdge>(leaf, cur_jg, jg.size());
	std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 460\n";
	edges.push_back(edge);
	std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 470\n";
	nodeToJet[cur_jg] = jg[0];
	std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 480\n";
      }
    }
    if(jg_is_used){++cur_jg;}
  }
  std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 500\n";
  for(const auto& e: edges){
    std::cerr<< *e << '\n';
  }
  std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 510\n";

  // construct jet groups according from jet groups that pass child
  // conditions. The parent of the next condition to be processed
  // is found, and from the parent the condition's siblings are found,
  // all combinations of the jet groups passing the sibings are
  // constructed. One by one these combinations are tested for
  // parent satisfaction. If the parent is satisfied, it is placed in the
  // queue for later processing.
  std::queue<std::size_t> to_process;  // conditions to be processed.

  // keep track if a condition's sibling has been processed.
  std::vector<bool> checked(m_conditions.size(), false);

  // initialise the queue with satisfied leaf conditions.
  for(const auto& l : leaves){to_process.push(l);}

  std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 600\n";
  while(!to_process.empty()){
    auto k = to_process.front();
    if (checked[k]){
      // condition taken into account when process one of its siblings.
      to_process.pop();
      std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 650  skipping " << k <<"\n";
      continue;
    }
    std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges \n";

    // find the siblings of the condition being processed.
    auto siblings = m_tree.getSiblings(k);

    // mark the siblings as having been processed.
    for(const auto& s : siblings){checked[s] = true;}

    // get the passing jet group(s) for each satisfied sibling
    std::vector<HypoJetGroupVector> sibling_jgroups;

    
    std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 800\n";
    for(const auto& s : siblings){
      sibling_jgroups.push_back(satisfiedBy[s]);
    }
    std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 900\n";

    // calculate the external product of the jet groups
    // eg if condition c1 is satisfied by jg11 and jg12, while its only
    // sibling c2 is satisfied by jg21, the external jet groups are
    // jg11jg21, jg12jg21. Each of these  are flattened.
    auto jg_product = JetGroupProduct(sibling_jgroups);
    auto next = jg_product.next();
    auto par = m_tree.getParent(k);

    std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 1000\n";
    while (next.has_value()){
      auto jg = *next;
      std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 950 \n";
      if (m_conditions[par]->isSatisfied(jg, collector)){
	++cur_jg;
      std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 960 cur_jg " << cur_jg <<"\n";
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
  std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 1100\n";
  auto sink = ++cur_jg;
  for(const auto& n : toSink){
    edges.push_back(std::make_shared<FlowEdge>(n, sink, 1));
  }
  V = ++ sink;
  std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 1200\n";

  for(const auto& e : edges){
    std::cout << "UnifiedFlowNetworkBuilder Edge " << *e  << '\n';
  }
  std::cerr<<"UnifiedFlowNetworkBuilder::make_flowEdges 1300\n";
  return std::make_optional<std::vector<std::shared_ptr<FlowEdge>>>(edges);
}

std::string UnifiedFlowNetworkBuilder::toString() const {
  std::stringstream ss;
  ss << "UnifiedFlowNetworkBuilder:\n";
  ss << "  treeVector: " << m_tree << '\n';;
  ss << FlowNetworkBuilderBase::toString();
  return ss.str();
}


