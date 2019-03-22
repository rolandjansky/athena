/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DESCENDINGET_H
#define TRIGHLTJETHYPO_DESCENDINGET_H

// ********************************************************************
//
// NAME:     DescendingEt.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

struct DescendingEt:
  public std::binary_function<pHypoJet, pHypoJet, bool> 
{
  bool operator()(pHypoJet l, pHypoJet r) const;
};

#endif
