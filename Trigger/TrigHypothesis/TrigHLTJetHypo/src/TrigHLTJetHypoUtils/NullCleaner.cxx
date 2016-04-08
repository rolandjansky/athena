/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     NullCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/NullCleaner.h"
#include <sstream>

bool NullCleaner::operator()(const pHypoJet&) const {return true;}    

std::string NullCleaner::toString() const noexcept {
  std::stringstream ss;
  ss << getName() << '\n';
  return ss.str();
}

std::string NullCleaner::getName() const noexcept  {return "NullCleaner";}
