/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DescendingEt.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DescendingEt.h"
#include <TLorentzVector.h>

bool DescendingEt::operator()(pHypoJet l, pHypoJet r)  const {
  return l->p4().Et() > r->p4().Et();
}

