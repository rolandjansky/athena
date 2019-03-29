/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AntiCleanerTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AntiCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

AntiCleanerTool::AntiCleanerTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent) :
  base_class(type, name, parent){ 
}


bool AntiCleanerTool::select(const pHypoJet& jet) const{
  return !(m_cleaner->select(jet));
}

std::string
AntiCleanerTool::getName() const noexcept {return "AntiCleanerTool";}



std::string AntiCleanerTool::toString() const noexcept {
  std::stringstream ss;
  ss << "AntiCleanerTool: inverting: "
     << m_cleaner->toString()
     <<'\n';

  return ss.str();
}

