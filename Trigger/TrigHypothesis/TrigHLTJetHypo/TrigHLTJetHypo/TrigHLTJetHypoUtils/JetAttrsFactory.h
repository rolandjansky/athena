/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHLTHYPO_JETATTRSFACTORY_H
#define TRIGJETHLTHYPO_JETATTRSFACTORY_H

// ********************************************************************
//
// NAME:     JetAttrsFactory.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  A. Steinhebel
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

Conditions JetAttrsSort(std::string var, double limitMin, double limitMax);


#endif

