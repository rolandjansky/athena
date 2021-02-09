/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupProductFactory.h"
#include "./JetGroupProduct.h"
#include "./JetGroupUnion.h"

std::unique_ptr<IJetGroupProduct>
makeJetGroupProduct(const std::vector<std::size_t>& siblings,
		    const CondInd2JetGroupsInds& satisfiedBy,
		    const std::vector<std::size_t>& condMult,
		    const JetGroupInd2ElemInds& jg2elemjgs,
		    std::size_t parentCapacity,
		    const Collector& collector) {

  std::unique_ptr<IJetGroupProduct> jpg (nullptr);
  
  // If the parent condition has capacity 0, it is an AcceptAll
  // condition. If there is only one child, merely return the set
  // of elemental conditions passing the child. This takes care
  // of, dor example 10j40 where there is one child (with multiplicity = 1
  //
  // This approach does _not_ work for multiple children
  // eg j80_j60 where both consditions pass the only one jet.
  // attempting to the same strategy here fails as the single set
  // will trigger a pass, whereas two are needed.
  if (parentCapacity == 0 and siblings.size() == 1) {
    std::unique_ptr<IJetGroupProduct> jpg (nullptr);
    jpg.reset(new JetGroupUnion(siblings,
				satisfiedBy,
				jg2elemjgs,
				collector));
    return jpg;
  }

  // Otherwise need a group product that will step
  // through the external products of the sets
  jpg.reset(new JetGroupProduct(siblings, satisfiedBy, condMult));
  return jpg;
}
		    
