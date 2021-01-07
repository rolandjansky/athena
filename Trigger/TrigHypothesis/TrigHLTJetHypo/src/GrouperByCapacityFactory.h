/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_GROUPERBYCAPACITYFACTORY_H
#define TRIGHLTJETHYPO_GROUPERBYCAPACITYFACTORY_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include <memory>

std::unique_ptr<IJetGrouper> grouperByCapacityFactory(unsigned int capacity,
						      const HypoJetCIter& b,
						      const HypoJetCIter& e);

#endif
