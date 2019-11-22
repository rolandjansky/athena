/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_GROUPSMATCHERFACTORYMT_H
#define TRIGHLTJETHYPO_GROUPSMATCHERFACTORYMT_H

#include "./IGroupsMatcherMT.h"
#include "./ConditionsDefsMT.h"
#include <memory>

std::unique_ptr<IGroupsMatcherMT>
groupsMatcherFactoryMT_MaxBipartite(ConditionsMT&&);

std::unique_ptr<IGroupsMatcherMT>
groupsMatcherFactoryMT_Partitions(ConditionsMT&&);


std::unique_ptr<IGroupsMatcherMT>
groupsMatcherFactoryMT_FastReduction(ConditionsMT&&,
				     const std::vector<std::size_t>& treeVec,
				     const std::vector<std::vector<int>>&);
#endif
