/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupProductFactory.h"
#include "./JetGroupProduct.h"
#include "./EmptyJetGroupProduct.h"
#include "./JetGroupUnion.h"

#include <algorithm>
#include <numeric>  // std::accumulate


std::unique_ptr<IJetGroupProduct>
makeJetGroupProduct(const std::vector<std::size_t>& siblings,
		    const std::vector<bool>& leaves,
		    const CondInd2JetGroupsInds& satisfiedBy,
		    const std::vector<std::size_t>& condMult,
		    const std::vector<unsigned int>& condCap,
		    const std::vector<int>& condClique,
		    const JetGroupInd2ElemInds& jg2elemjgs,
		    std::size_t parentCapacity,
		    const Collector&) {


  // If the parent condition has capacity 0, it is an AcceptAll
  // condition. If there is only one child, merely return the set
  // of elemental conditions passing the child. This takes care
  // of, dor example 10j40 where there is one child (with multiplicity = 1
  //
  // This approach does _not_ work for multiple children
  // eg j80_j60 where both consditions pass the only one jet.
  // attempting to the same strategy here fails as the single set
  // will trigger a pass, whereas two are needed.


  std::unique_ptr<IJetGroupProduct> jpg (nullptr);

  if (siblings.empty()) {
    jpg.reset(new EmptyJetGroupProduct);
    return jpg;
  }

  		  
  for(const auto& s : siblings) {
    if (satisfiedBy.at(s).empty()) {
      jpg.reset(new EmptyJetGroupProduct);
      return jpg;    
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
				      
  if (parentCapacity == 0 and clique_map.size() == 1){
    std::unique_ptr<IJetGroupProduct> jpg (nullptr);
    jpg.reset(new JetGroupUnion(siblings,
				leaves,
				satisfiedBy,
				jg2elemjgs,
				n_required));
    
    return jpg;
  }

  
  // Otherwise need a group product that will step
  // through the external products of the sets
  jpg.reset(new JetGroupProduct(siblings, satisfiedBy, condMult));
  return jpg;
}
		    
