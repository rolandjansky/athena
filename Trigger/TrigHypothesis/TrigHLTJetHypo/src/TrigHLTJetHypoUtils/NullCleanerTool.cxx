/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     NullCleanerTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/NullCleanerTool.h"
#include <sstream>

NullCleanerTool::NullCleanerTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent) :
  base_class(type, name, parent){ 
}
  

bool NullCleanerTool::select(const pHypoJet&) const {return true;}    

std::string NullCleanerTool::toString() const noexcept {
  std::stringstream ss;
  ss << getName() << '\n';
  return ss.str();
}

std::string NullCleanerTool::getName() const noexcept  {
  return "NullCleanerTool";}

