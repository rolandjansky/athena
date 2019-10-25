/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./UnifiedFlowNetworkBuilder.h"
#include <map>
#include <algorithm>
#include <sstream>

#include <iostream>
#include <assert.h>



void
recordJetGroup(std::size_t ind,
	       const HypoJetVector& jg,
	       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector){
  
  std::stringstream ss0;
  ss0  << "UnifiedFlowNetworkBuilder jet group "
       << ind << " [" << jg.size() <<"]:";
  
  std::stringstream ss1;
  for(auto ip : jg){
    const void* address = static_cast<const void*>(ip);
    ss1  << "\n "  << address << " eta " << ip->eta()
	 << " e " << ip->e()
	 << " et " << ip->et();
  }
  ss1 << '\n';
  collector -> collect(ss0.str(), ss1.str());
}
		    

class DepthComparison{
public:
  DepthComparison(const Tree& t) : m_tree(t){}
  bool operator () (const std::size_t& lhs, const std::size_t rhs){
    return m_tree.depth(rhs) > m_tree.depth(lhs);
  }
private:
  Tree m_tree;
};

 
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
                                           std::map<int, pHypoJet>& nodeToJet) const {
  /*
    Build a flow network corresponding to a hypo tree. 
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
       <<" support one set of shared nodes, but received "
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
                                           std::map<int, pHypoJet>&, // nodeToJet,
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

                         sink
                     /     /       \
		  cond  cond      cond        final condition layer
		          ......              build uo combined jgs, conditions

		   leaf_0 leaf_1 ... leaf_n
                         / \|   /             incoming jg-leaf matches

		   jg_0   jg_1....jg_m

                    \     |    /
                         source

   Each of the incoming job groups are tested against the leaf conditions.
   Edges represnt matchs. The jgs that satisfy a condition are noted.

   After precessing the leaf nodes, the jgs that satisfied those nodes are used
   to construct a job group to be tested agains the next generation of 
   conditions, and edges are produced if the constructed jgs satisfy this
   generation of conditions. The procedure repeates until the last generation
   of Conditions is processed. The set of edges is returned.
   */


  // cur_jg will be used as a flow network node identifier. 0 is the source, start at 1
  std::size_t cur_jg = 1;

  std::vector<std::shared_ptr<FlowEdge>> edges;
  // makeSourceToLeafEdges(edges, leaves);
 

  // flow network node number for contributing JetGroups. We only
  // assign a node to a jet group if the group satisfies a condition.
   
  // cond id to vector of inidices pointing into the inJetGroup vector
  // The inJetGroup vector contains jet groups that satisfied the leaf nodes.
  CondInd2JetGroupsInds satisfiedBy;
  JetGroupInd2ElemInds jg2elemjgs;
 // jet grp id, jet grp
  std::map<std::size_t, HypoJetVector> indJetGroups{}; 
  
  findInitialJobGroups(edges,
		       leaves,
		       groups_b,
		       groups_e,
		       satisfiedBy,
		       indJetGroups,
		       jg2elemjgs,
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
		 satisfiedBy,
		 indJetGroups,
		 jg2elemjgs,
		 cur_jg,
		 collector);

  //sink index follows last jet group index
  std::size_t sink = cur_jg;
  V = sink;
  ++V;
  for(const auto e : edges){std::cout << "after propagate: "<<  *e <<'\n';}

  for(const auto& i :satisfiedBy[1]){
    edges.push_back(std::make_shared<FlowEdge>(i, sink, jg2elemjgs[i].size())); 
  }
  // makeSinkEdges(satisfiedBy[1], sink);
  for(const auto e : edges){std::cout << "final: "<<  *e <<'\n';}
  std::cout<<"after propagate, sink: " << sink << " V: " << V << '\n';
  
  return std::make_optional<std::vector<std::shared_ptr<FlowEdge>>>(edges);
}

/*
void
UnifiedFlowNetworkBuilder::makeSourceToLeafEdges(std::vector<std::shared_ptr<FlowEdge>>& edges,
						 const std::vector<int>& leaves) const {
  for(const auto& i : leaves){
    edges.push_back(std::make_shared<FlowEdge>(0,
					       i,
					       m_conditions[i]->capacity()));
  }
}
*/

void UnifiedFlowNetworkBuilder::findInitialJobGroups(std::vector<std::shared_ptr<FlowEdge>>& edges,
						     const std::vector<int>& leaves,
						     const HypoJetGroupCIter& groups_b,
						     const HypoJetGroupCIter& groups_e,
						     CondInd2JetGroupsInds& satisfiedBy,
						     std::map<std::size_t, HypoJetVector>& indJetGroups,
						     JetGroupInd2ElemInds& jg2elemjgs,
						     std::size_t& cur_jg,
						     const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  

  /*
    Will now test the incoming jet groups against the leaf conditions.
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

    // if a jet group satisfies a condition, note the fact, and store it by index
    bool jg_used{false};
    for(const auto& leaf: leaves){
      
      if (m_conditions[leaf]->isSatisfied(jg, collector)){
	if(!jg_used){
	  // do the following once if jet group satisfies any condition...
	  jg_used= true;
	  jg2elemjgs.emplace(cur_jg, std::vector<std::size_t>{cur_jg});
	  indJetGroups.emplace(cur_jg, jg);
	  edges.push_back(std::make_shared<FlowEdge>(0,
						     cur_jg,
						     jg.size()));
	  if(collector){recordJetGroup(cur_jg, jg, collector);}
	  ++cur_jg;
	}
	// do the following for each satisfied condition ...
	satisfiedBy[leaf].push_back(cur_jg-1);
      }
    }
  }
}



void UnifiedFlowNetworkBuilder::propagateEdges(std::vector<std::shared_ptr<FlowEdge>>& edges,
					       CondInd2JetGroupsInds& satisfiedBy,
					       const std::map<std::size_t, HypoJetVector>& indJetGroups,
					       JetGroupInd2ElemInds& jg2elemjgs,  
					       std::size_t& cur_jg,
					       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const{

  
  // construct jet groups according from jet groups that pass child
  // conditions.
  // This method controls which nodes to process. It checks whether all sibling nodes are processed.
  // if so, processing of the parent is delegated to propagate_()
  
  //The parent of the next condition to be processed
  // is found, and from the parent the condition's siblings are found,

  typedef std::priority_queue<std::size_t,
			      std::vector<std::size_t>,
			      DepthComparison> DepthQueue;

  auto comparator = DepthComparison(m_tree);
  DepthQueue to_process(comparator); // conditions to be processed.

  // keep track if a condition's sibling has been processed.
  std::vector<bool> checked(m_conditions.size(), false);

  // initialise the queue with satisfied leaf conditions indices.
  for(const auto& item : satisfiedBy){to_process.push(item.first);}

  std::cout << "initialised processing queue with: ";
  std::for_each(satisfiedBy.begin(),
		satisfiedBy.end(),
		[](const auto& item){std::cout<<item.first << " ";});
  std::cout << '\n';

  while(!to_process.empty()){
    auto k = to_process.top();
    to_process.pop();
    std::cout << "popping " << k << '\n';
    if(checked[k]){
      std::cout << "node " << k << "checked, skipping\n";
      continue;
    }
    std::cout << "setting node for " << k<< " and siblings checked\n";

    // block siblings from reprocessing parent
    auto siblings = m_tree.getSiblings(k);
    for(const auto& s : siblings){
      checked[s] = true;
    }
    std::cout << "processing " << k << '\n';
    
    // find the siblings indices of the condition being processed.
    // bool siblings_complete{true};
    // for(const auto& s : siblings){
    //  if(!checked[s]){
    //	to_process.push(s);
    //	std::cout << "pushed " << s << '\n';
    //	siblings_complete = false;
    //	break;
    //      }
    //    }

    //    if(!siblings_complete){continue;}


    // check if combinations of groups satisfying children satisfy their parent
    if (k != 0){

      auto par_satisfied =  propagate_(edges,
				       k,
				       siblings,
				       satisfiedBy,
				       jg2elemjgs,
				       indJetGroups,
				       cur_jg,
				       collector);
      if(par_satisfied){
	std::size_t par =  m_tree.getParent(k);
	std::cout << "pushed " << par << '\n';
	to_process.push(par);
      }
    }
  }
}


bool UnifiedFlowNetworkBuilder::propagate_(std::vector<std::shared_ptr<FlowEdge>>& edges,
					   std::size_t child,
					   const std::vector<std::size_t>& siblings,
					   CondInd2JetGroupsInds& satisfiedBy,
					   JetGroupInd2ElemInds& jg2elemjgs,  
					   const std::map<std::size_t, HypoJetVector>& indJetGroups,
					   std::size_t& cur_jg,
					   const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const{

  // all combinations of the jet groups passing the sibings are
  // constructed. One by one these combinations are tested for
  // parent satisfaction. Edges are contructed between satisfying job groups and the parent.
  // if any such edge is constructed, the calling rroutine is notified so it
  // can scheduling processing the parent as a child.

  std::cout << "propagate_ 0\n";
  std::cout << "siblings: ";
  for(const auto& i :siblings){ std::cout << i << " ";}
  std::cout <<'\n';
  std::cout << "satisfiedBy:\n";
  for(const auto& p : satisfiedBy){
    std:: cout << p.first << ": ";
    for(const auto& i :p.second){ std::cout << i << " ";}
    std::cout << '\n';
  }
  std::cout << "jg2elemjgs:\n";
  for(const auto& p : jg2elemjgs){
    std:: cout << p.first << ": ";
    for(const auto& i :p.second){ std::cout << i << " ";}
    std::cout << '\n';
  }
  std::size_t par =  m_tree.getParent(child);
  if(par == 0){return false;}
  
  std::cout << "child: " << child << " parent: " <<par << '\n';
      
  bool par_satisfied{false};  
      
  // calculate the external product of the jet groups
  // eg if condition c1 is satisfied by jg11 and jg12, while its only
  // sibling c2 is satisfied by jg21, the external jet groups are
  // jg11jg21, jg12jg21. Each of these  are flattened.
  
  auto jg_product = JetGroupProduct(siblings, satisfiedBy);
  // Note: while not siblings have been processed, next() will fail due to empty group vectors
  auto next = jg_product.next();

  // step through the job groups found by combining ghe child groups
  // check ecach combination to see if it satisfies the parent. If so
  // add an edge from the contributing children, and from the new job group to the parent.

  while (next.has_value()){
    
    auto jg_indices = *next;
    std::vector<std::size_t> elem_jgs;
    HypoJetVector jg;
 
    for(const auto& ind : jg_indices){
      elem_jgs.insert(elem_jgs.end(),
		      jg2elemjgs[ind].begin(),
		      jg2elemjgs[ind].end());
    }

    std::set<std::size_t> unique_indices(elem_jgs.begin(),
					 elem_jgs.end());
    if(unique_indices.size() != elem_jgs.size()){
      next = jg_product.next();
      continue;
    }

    for(const auto& i : elem_jgs){
      jg.push_back(indJetGroups.at(i)[0]);  // why [0]? assume elemental jg has size 1
    }
    std::cout<< "propagate_ parent testing condition " << par << " jet group indices ";
    for(const auto& i : elem_jgs){std::cout<< i << " ";}
    std::cout << " size jet vec " << jg.size() << '\n';
        
    std::cout << "propagate_ 100\n";

    if (m_conditions[par]->isSatisfied(jg, collector)){// par is a tree ind.
      par_satisfied = true;
      std::cout<< "propagate_ parent condition " << par << " satisfied, by jet group "
	       <<cur_jg<< "with elements: ";
      for(const auto& i : elem_jgs){std::cout<< i << " ";}
      std::cout << '\n';
      satisfiedBy[par].push_back(cur_jg);
      jg2elemjgs[cur_jg] = elem_jgs;
      if(collector){recordJetGroup(cur_jg, jg, collector);}
      
      for(const auto& i : jg_indices){
	edges.push_back(std::make_shared<FlowEdge>(i,
						   cur_jg,
						   jg2elemjgs[i].size()));
	std::cout<< "propagate_ adding edge " << *(edges.back())<<'\n';
      }
      ++cur_jg;
    } else {
      std::cout<< "propagate_ parent condition " << par << " NOT satisfied, no update\n";
    }
    
    next = jg_product.next();
  }
  std::cout << "propagate_ 9999\n";
  return par_satisfied;
}

/*
void UnifiedFlowNetworkBuilder::makeSinkEdges(std::vector<std::shared_ptr<FlowEdge>>& edges,
					      std::size_t sink) const {
  for(const auto& i : m_tree.firstGeneration()){
    edges.push_back(std::make_shared<FlowEdge>(i,
					       sink,
					       m_conditions[i]->capacity()));
  }
}
*/

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


