/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     NullCleanerObject.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/NullCleanerObject.h"

pairHypoJetIter NullCleanerObject::operator()(const pairHypoJetIter& iters) const {
  return iters;
}


std::string NullCleanerObject::toString() const noexcept {
  return "Null Cleaner";
}

std::string NullCleanerObject::getName() const noexcept  {return "Null Cleaner object";}
