/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupProductFactory.h"
#include "./JetGroupProduct.h"
#include "./EmptyJetGroupProduct.h"
#include "./JetGroupUnion.h"
#include "./JetGroupReducer.h"
#include "./JetGroupSingleClique.h"

#include <algorithm>
#include <numeric>  // std::accumulate

std::size_t max_jet (const std::vector<std::size_t>& siblings,
		     const CondInd2JetGroupsInds& satisfiedBy) {
  std::vector<std::size_t> inds;
  for (const auto& s : siblings) {
    inds.insert(inds.end(),
		satisfiedBy.at(s).cbegin(),
		satisfiedBy.at(s).cend());
  }
  
  auto iter = std::max_element(inds.begin(), inds.end());
  if (iter == inds.end()) {return 0;}
  return *iter;
}
		       

bool willPassSimpleTree(const std::vector<std::size_t>& siblings,
			const CondInd2JetGroupsInds& satisfiedBy,
			const std::vector<std::size_t>& condMult){

  for (const auto& s : siblings) {
    if (satisfiedBy.at(s).empty()) {return false;}
  }
  
  std::vector<std::vector<std::size_t>> ind_stack;

  for (const auto& s : siblings) {
    auto repeat = condMult.at(s);
    for (std::size_t i = 0; i != repeat; ++i) {
      auto c_inds = satisfiedBy.at(s);
      ind_stack.emplace_back(c_inds);
    }
  }


  std::sort (ind_stack.begin(), ind_stack.end(),
	     [](const auto& v0, const auto& v1) {
	       return v0.size() < v1.size();
	     });

  auto max_ind  = max_jet(siblings, satisfiedBy);

  std::vector<bool> mask(max_ind+1, false);  //why +1? : what if only jet = 0

  for (const auto& v : ind_stack){
    bool found{false};
    for (const auto& e : v) {
      if (!mask.at(e)){  // looking for a false
	mask.at(e) = true;  // found one, not blocked
	found = true;
	break;
      }
    }
    if (!found) {return false;}
  }
  return true;
}

std::unique_ptr<IJetGroupProduct>
makeJetGroupProduct(const std::vector<std::size_t>& siblings,
		    const CondInd2JetGroupsInds& satisfiedBy,
		    const std::vector<std::size_t>& condMult,
		    const std::vector<unsigned int>& condCap,
		    const std::vector<int>& condClique,
		    const JetGroupInd2ElemInds& jg2elemjgs,
		    std::size_t parentCap,
		    bool simpleTree,
		    const Collector&) {


  // simpleTree - flags with the tree has an AceptAll root (mult 1, cap 0)
  // with all other nodes being children.
  // leaves - one entry per sibling
  
  // satisfiedBy map. key:Condition val: vector of satisfying jets,
  // includes the root node. size = nConditions

  // condMult - multiplicity, all conditions, size = nConditions
  // condCap - capacity, all conditions, size = nConditions
  // condClique - clique index, size = nConditions


  // jg2elemjgs - jet group index to elementary jet groups
  // not needed for simpleTrees as all jet groups are elementary jet groups
  // for simple trees.

  std::unique_ptr<IJetGroupProduct> jgp (nullptr);

  if (siblings.empty()) {
    jgp.reset(new EmptyJetGroupProduct);
    return jgp;
  }

  		  
  for(const auto& s : siblings) {
    if (satisfiedBy.at(s).empty()) {
      jgp.reset(new EmptyJetGroupProduct);
      return jgp;    
    }
  }
  
  
  std::map<int, std::vector<std::size_t>> clique_map;
  std::for_each(siblings.begin(),
		siblings.end(),
		[&clique_map, &condClique](std::size_t s){
		  auto clique = condClique[s];
		  if (clique != -1) {
		    clique_map[clique].push_back(s);}});
	
  

  auto n_required = std::accumulate(siblings.cbegin(),
				    siblings.cend(),
				    0u,
				    [&condMult, &condCap](const auto& sofar,
							  const auto& ind){
				      return sofar +
					condMult.at(ind)*condCap.at(ind);});


  bool cap1{true};
  for(const auto& s: siblings) {
    if (condCap.at(s) != 1) {
      cap1 = false;
      break;
    }
  }
  
  if (simpleTree and cap1) {
    if (clique_map.size() == 1){
      jgp.reset(new JetGroupSingleClique(satisfiedBy.at(1),  // first child
					 n_required));
      return jgp;
    }


    // check whether all Conditions have at least
    // n_required jets. If so, a JetGroupUnion
    // can be used to provide the single
    // relevant jet group.
    bool enough{true};
    for (const auto& s : siblings) {
      if ((satisfiedBy.at(s)).size() < n_required) {
	enough = false;
	break;
      }
    }
    
    if (enough) {
      jgp.reset(new JetGroupUnion(siblings,
				  satisfiedBy));
      return jgp;
    }

    if (willPassSimpleTree(siblings, satisfiedBy, condMult)) {
      jgp.reset(new JetGroupUnion(siblings,
				  satisfiedBy));
      return jgp;
    }

    jgp.reset(new EmptyJetGroupProduct);
    return jgp;    
  }

  // tree is non-simple or there are siblings are not all capacity 1.
  // Special case: parent node is AcceptAll, one child.
  // Examples:
  // Dijet scenario (non-simple tree, root node has one child).
  // HT scenario (simple tree, child node has cap != 1)
  if (parentCap == 0 and siblings.size() == 1) {

    jgp.reset(new JetGroupReducer(siblings,
				  satisfiedBy,
				  jg2elemjgs));
    return jgp;
  }
  
  
  // Otherwise need a group product that will step
  // through the external products of the sets
  // Note - if a tree contained an AcceptAll node internally,
  // for which all children had capactity = 1, a JetGroupReducer
  // could be used. This would be a very unusual case, and is
  // not treated here.
  
  jgp.reset(new JetGroupProduct(siblings, satisfiedBy, condMult,
				jg2elemjgs));
  return jgp;
}
		    
