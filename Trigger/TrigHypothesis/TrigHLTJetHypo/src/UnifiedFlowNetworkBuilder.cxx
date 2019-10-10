/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./UnifiedFlowNetworkBuilder.h"
#include "./JetGroupProduct.h"
#include <map>
#include <algorithm>
#include <sstream>

#include <iostream>
#include <assert.h>

UnifiedFlowNetworkBuilder::UnifiedFlowNetworkBuilder(ConditionsMT conditions,
						     const Tree& tree,
						     const std::vector<std::vector<int>>& sharedNodes):
  FlowNetworkBuilderBase(std::move(conditions)),
  m_tree(tree),
  m_sharedNodes(sharedNodes){
  }



std::optional<std::vector<std::shared_ptr<FlowEdge>>>
UnifiedFlowNetworkBuilder::make_flowEdges(const HypoJetGroupCIter& groups_b,
                                           const HypoJetGroupCIter& groups_e, 
                                           const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                                           int& V,
                                           std::map<int, pHypoJet>& nodeToJet) const{
  /*
    Build a flow network correposnding to a hypo tree. 
    The nodes pf the flow network represent jet groups or conditions.
    Edges have a node representing a jet group at one end and a node 
    representing a condition at the other. The edge exists if the jet group
    satisfies the condition.

    The structure of the network is

                         source
                     /     /       \
		  fn_0  fn_1 ... fn_n
                    \     |       ?
                         sink

   each of the fn_i are flow networks for which the leaf conditions see the
   incoming jet groups. Having i > 1 corresponds to having and AND node in the
   hypo tree. Another way of expressing this is that the leaf nodes of the
   fn_i flow networks share the incomming jet groups.
   */

  // TEMPORARY WHILE DEVELOPING: Allow only one set of shared jets.
  if(m_sharedNodes.size() != 1){
    std::stringstream ss;
    ss << "UnifiedFlowNetworkBuilder::make_leaves: currently "
       <<" support one set of shared nodes, but recieved "
       << m_sharedNodes.size();
    throw std::runtime_error(ss.str());
  }

  std::vector<std::vector<std::shared_ptr<FlowEdge>>> edges_container;
  for(const auto& leaves : m_sharedNodes){
    auto edges = (make_flowEdges_(groups_b,
				  groups_e, 
				  collector,
				  V,
				  nodeToJet,
				  leaves));

    if(!edges.has_value()){
      // early return if unsatisfied conditions
      std::cout << "Early return 1 \n";
      return edges;   // be careful if OR is implemented
    }
      
    edges_container.push_back(*edges);

  }
  
  // TEMPORARY while developing: one set of shared nodes, do not fit
  // into larger network.
  return std::make_optional<std::vector<std::shared_ptr<FlowEdge>>>(edges_container[0]);
}

std::optional<std::vector<std::shared_ptr<FlowEdge>>>
UnifiedFlowNetworkBuilder::make_flowEdges_(const HypoJetGroupCIter& groups_b,
                                           const HypoJetGroupCIter& groups_e, 
                                           const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                                           int& V,
                                           std::map<int, pHypoJet>& nodeToJet,
					   const std::vector<int>& leaves
					   ) const{
    /*
    Build a flow network correposnding to a hypo tree with no shared nodes.
    The nodes pf the flow network represent jet groups or conditions.
    Edges have a node representing a jet group at one end and a node 
    representing a condition at the other. The edge exists if the jet group
    satisfies the condition.

    Conditions occur in  a tree structure (allows for representation of,
    e.g., dijet -> jet, jet).

    The structure of the network is

                         source
                     /     /       \
		  cond  cond      cond        final condition layer
		          ......              build uo combined jgs, conditions

		   leaf_0 leaf_1 ... leaf_n
                         / \|   /             oncoming jg-leaf matches

		   jg_0   jg_1....jg_m

                    \     |    /
                         sink

   Each of the incoming job groups are tested against the leaf conditions.
   Edges represnt matchs. The jgs that satisfy a condition are noted.

   After precessing the leaf nodes, the jgs that satisfied those nodes are used
   to construct a job group to be tested agains the next generation of 
   conditions, and edges are produced if the constructed jgs satisfy this
   generation of conditions. The procedure repeates until the last generation
   of Conditions is processed. The set of edges is returned.
   */


  std::size_t cur_jg = m_tree.size();  // (first node - 1)  for jets

  std::vector<std::shared_ptr<FlowEdge>> edges;
  makeSourceToLeafEdges(edges, leaves);
 

  // create map to keep track of which jet groups
  // satisfy condition (by node id)
  std::map<std::size_t, HypoJetGroupVector> satisfiedBy;
  std::set<std::size_t> toSink;  // nodes to connect to the sink, when known

  // flow network node number for contributing JetGroups. We only
  // assign a node to a jet group if the group satisfies a condition.
  
  IDJetGroup jetgroups;

  makeLeafJobGroupEdges(edges,
			leaves,
			groups_b,
			groups_e,
			nodeToJet,
			satisfiedBy,
			jetgroups,
			toSink,
			cur_jg,
			collector);

  for(const auto e : edges){std::cout << "after making jg-leaf edges 1: "<<  *e <<'\n';}

  // require all leaf nodes to be satisfied.
  if (satisfiedBy.size() != leaves.size()){
    std::cout << "Early return 0 " << satisfiedBy.size() << " " << leaves.size() <<'\n';
    return std::optional<std::vector<std::shared_ptr<FlowEdge>>>();
  }

  // now know which job groups match which leaf nodes. Propagate combined
  // job groups to parents of condition being processed
  propagateEdges(edges,
		 leaves,
		 satisfiedBy,
		 cur_jg,
		 jetgroups,
		 toSink,
		 V,
		 collector);

  for(const auto e : edges){std::cout << "all edges: "<<  *e <<'\n';}

  return std::make_optional<std::vector<std::shared_ptr<FlowEdge>>>(edges);
}

void
UnifiedFlowNetworkBuilder::makeSourceToLeafEdges(std::vector<std::shared_ptr<FlowEdge>>& edges,
						 const std::vector<int>& leaves) const {
  for(const auto& i : leaves){
    edges.push_back(std::make_shared<FlowEdge>(0,
					       i,
					       m_conditions[i]->capacity()));
  }
}

void UnifiedFlowNetworkBuilder::makeLeafJobGroupEdges(std::vector<std::shared_ptr<FlowEdge>>& edges,
						      const std::vector<int>& leaves,
						      const HypoJetGroupCIter& groups_b,
						      const HypoJetGroupCIter& groups_e,
						      std::map<int, pHypoJet>& nodeToJet,
						      std::map<std::size_t, HypoJetGroupVector>& satisfiedBy,
						      IDJetGroup& jetgroups,
						      std::set<std::size_t>& toSink, 
						      std::size_t& cur_jg,
						      const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const{
  

  /*
    Will now test the incomming jet groups against the leaf conditions.
    keep track of input jet groups to be connected toi the sink node.
    
    we do not know the sink node ID yet. This is because
    we will form internediate jet groups according to which groups
    are accepted by previous generation conditions. 
  */

  for(auto iter = groups_b; iter != groups_e; ++iter){
    auto jg = *iter;
    
    if(jg.size() != 1){
      
      // input jet groups of size 1 (for now....). This used
      // to establish which jets participated in a passing hypo.
      
      // return std::optional<std::vector<std::shared_ptr<FlowEdge>>>();
      // TEMPORARY while  developing
      throw std::runtime_error("Edge making failed");

    }
    
    bool jg_is_used{false};
    for(const auto& leaf: leaves){
      
      if (m_conditions[leaf]->isSatisfied(jg, collector)){
	jg_is_used = true;
	satisfiedBy[leaf].push_back(jg);  //leaf: tree index
	toSink.insert(cur_jg);
	jetgroups.insert(IDJetGroup::value_type(cur_jg, jg));
	auto edge = std::make_shared<FlowEdge>(leaf, cur_jg, jg.size());
	edges.push_back(edge);
	nodeToJet[cur_jg] = jg[0];
      }

    }
    if(jg_is_used){++cur_jg;}
  }
}



void UnifiedFlowNetworkBuilder::propagateEdges(std::vector<std::shared_ptr<FlowEdge>>& edges,
					       const std::vector<int>& leaves,
					       std::map<std::size_t, HypoJetGroupVector>& satisfiedBy,
					       std::size_t& cur_jg,
					       IDJetGroup& jetgroups,
					       std::set<std::size_t>& toSink, 					    
					       int& V,
					       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const{

  
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

  // initialise the queue with satisfied leaf conditions indices.
  for(const auto& l : leaves){to_process.push(l);}

  std::cout << "initialised processing queue with: ";
  std::copy(leaves.begin(), leaves.end(),  std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
  
  while(!to_process.empty()){
    auto k = to_process.front();
    std::cout << "processing " << k << '\n';
    if (checked[k]){
      // condition taken into account when process one of its siblings.
      std::cout << "popping " << k << '\n';
      to_process.pop();
      continue;
    }

    // find the siblings indices of the condition being processed.
    auto siblings = m_tree.getSiblings(k);

    std::cout << "siblings of "<<k<<": ";
    for(const auto& s : siblings){std::cout << s << " ";}
    std::cout << '\n';

    // mark the siblings as having been processed.
    for(const auto& s : siblings){checked[s] = true;}
    std::cout << "checked[]: ";
    for(const auto& c : checked){std::cout <<std::boolalpha<< c << " ";}
    std::cout << '\n';
    
    // get the passing jet group(s) for each satisfied sibling
    std::vector<HypoJetGroupVector> sibling_jgroups;

    
    for(const auto& s : siblings){
      sibling_jgroups.push_back(satisfiedBy[s]);  //s is a tree index
    }
    std::cout << "sent 100 length sibling_jgroups: " << sibling_jgroups.size() << '\n';;
    
    // std::cout<< collector->toString();
    
    // calculate the external product of the jet groups
    // eg if condition c1 is satisfied by jg11 and jg12, while its only
    // sibling c2 is satisfied by jg21, the external jet groups are
    // jg11jg21, jg12jg21. Each of these  are flattened.
    
    auto jg_product = JetGroupProduct(sibling_jgroups);
    std::cout << "sent 150\n";
    auto next = jg_product.next();
    std::cout << "sent 155\n";
    auto par = m_tree.getParent(k);
    std::cout << "sent 200\n";

    while (next.has_value()){
      auto jg = *next;
      if (m_conditions[par]->isSatisfied(jg, collector)){// par is a tree ind.
	std::cout << "parent " << par << " satisfied\n";
	++cur_jg;
	satisfiedBy[par].push_back(jg);
	
	edges.push_back(std::make_shared<FlowEdge>(par, cur_jg, jg.size()));
	for(const auto& s : siblings){
	  edges.push_back(std::make_shared<FlowEdge>(cur_jg,
						     s,
						     m_conditions[s-1]->capacity()));}
	jetgroups.insert(std::map<int, HypoJetVector>::value_type(cur_jg, jg));
      }      
      
      next = jg_product.next();
    }
    if(par != 0){to_process.push(par);}
    std::cout<< "pushing onto q: " << par << '\n';
  
    std::cout<< "popping from  q: " <<to_process.front() << '\n';
    to_process.pop();
  }
  
   for(const auto e : edges){std::cout << "source to leaf edges 3: "<<  *e <<'\n';}
   std::cout<< collector->toString();
   assert(false);

  auto sink = ++cur_jg;
  for(const auto& n : toSink){
    edges.push_back(std::make_shared<FlowEdge>(n, sink, 1));
  }
  V = ++ sink;
}


std::string UnifiedFlowNetworkBuilder::toString() const {
  std::stringstream ss;
  ss << "UnifiedFlowNetworkBuilder:\n";
  ss << "  treeVector: " << m_tree << '\n';;
  ss << "  shared node sets [" << m_sharedNodes.size() << "]:\n";
  for(const auto& snodelist : m_sharedNodes){
    for(const auto el : snodelist){
      ss << el << " ";
    }
    ss << '\n';
  }
  ss << FlowNetworkBuilderBase::toString();

  return ss.str();
}


