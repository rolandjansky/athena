/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AntiTightCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "./AntiTightCleaner.h"
#include "xAODJet/Jet.h"  //ICleaner

#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h"

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

bool AntiTightCleaner::operator()(const xAOD::Jet* jet) const{
  return TightCleaner::operator()(jet);
}





