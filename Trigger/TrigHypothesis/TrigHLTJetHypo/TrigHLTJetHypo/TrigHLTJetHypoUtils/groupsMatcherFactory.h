/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_GROUPSMATCHERFACTORY_H
#define TRIGHLTJETHYPO_GROUPSMATCHERFACTORY_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IGroupsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include <memory>

std::unique_ptr<IGroupsMatcher> groupsMatcherFactory(const Conditions&);

#endif
