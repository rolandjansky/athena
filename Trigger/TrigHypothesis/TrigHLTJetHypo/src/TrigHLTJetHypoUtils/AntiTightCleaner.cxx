/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AntiTightCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AntiTightCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

AntiTightCleaner::AntiTightCleaner(float fSampMaxTightThreshold,
                                   float etaTightThreshold,
                                   float emfLowTightThreshold,
                                   float emfHighTightThreshold,
                                   float hecfTightThreshold):
  TightCleaner(fSampMaxTightThreshold, 
               etaTightThreshold, 
               emfLowTightThreshold, 
               emfHighTightThreshold, 
               hecfTightThreshold){}

bool AntiTightCleaner::operator()(const pHypoJet& jet) const{
  return TightCleaner::operator()(jet);
}

std::string 
AntiTightCleaner::getName() const noexcept {return "AntiTightCleaner";}




