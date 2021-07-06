/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETGROUPPRODUCTFACTORY_H
#define TRIGHLTJETHYPO_JETGROUPPRODUCTFACTORY_H

#include "./JetGroupProduct.h"

// produce an object that will make jet groups
// for the parent Condition  depend on that Conditions capacity

using JetGroupInd2ElemInds = std::map<int, std::vector<std::size_t>>;

std::unique_ptr<IJetGroupProduct>
makeJetGroupProduct(const std::vector<std::size_t>& siblings,
		    const CondInd2JetGroupsInds& satisfiedBy,
		    const std::vector<std::size_t>& condMult,
		    const JetGroupInd2ElemInds& jg2elemjgs,
		    std::size_t parCapacity,
		    const Collector& collector);

#endif
