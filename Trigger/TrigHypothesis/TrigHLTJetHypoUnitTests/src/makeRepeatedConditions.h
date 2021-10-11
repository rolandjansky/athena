/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_MAKEREPEATEDCONDITIONS_H
#define TRIGHLTJETHYPOUNITTESTS_MAKEREPEATEDCONDITIONS_H

#include "TrigHLTJetHypo/../src/RepeatedConditionsDefs.h"

#include <utility>
#include <vector>

using EtaPair = std::pair<double, double>;
using EtaPairs = std::vector<EtaPair>;

ConditionPtrs makeRepeatedConditions(const EtaPairs& etas);
ConditionPtrs makeRepeatedConditions(const EtaPair& eta);

#endif
