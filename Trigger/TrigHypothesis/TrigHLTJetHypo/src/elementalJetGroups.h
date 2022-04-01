/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ELEMENTALJETGROUPS_H
#define TRIGHLTJETHYPO_ELEMENTALJETGROUPS_H

#include <map>
#include <vector>

using JetGroupInd2ElemInds = std::map<int, std::vector<std::size_t>>;

std::vector<std::size_t>
elementalJetGroups(const std::vector<std::size_t>& non_elemental,
		   const JetGroupInd2ElemInds& jg2elemjgs);


#endif
