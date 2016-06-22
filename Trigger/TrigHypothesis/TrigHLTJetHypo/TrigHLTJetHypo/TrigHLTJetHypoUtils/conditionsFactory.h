/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHLTHYPO_CONDITIONSFACTORY_H
#define TRIGJETHLTHYPO_CONDITIONSFACTORY_H

// ********************************************************************
//
// NAME:     conditionsFactory.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

Conditions conditionsFactory(const std::vector<double>& etasMin,
			     const std::vector<double>& etasMax,
			     const std::vector<double>& thresholds);

Conditions conditionsFactory(const std::vector<double>& etasMin,
			     const std::vector<double>& etasMax,
			     const std::vector<double>& ystarsMin,
			     const std::vector<double>& ystarsMax,
			     const std::vector<double>& massesMin,
			     const std::vector<double>& massesMax);


#endif
