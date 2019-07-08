/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_GROUPSMATCHERFACTORYMT_H
#define TRIGHLTJETHYPO_GROUPSMATCHERFACTORYMT_H

#include "./IGroupsMatcherMT.h"
#include "./ConditionsDefsMT.h"
#include <memory>

std::unique_ptr<IGroupsMatcherMT>
groupsMatcherFactoryMT_MaxBipartite(const ConditionsMT&);

std::unique_ptr<IGroupsMatcherMT>
groupsMatcherFactoryMT_Partitions(const ConditionsMT&);

#endif
