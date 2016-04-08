/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AntiLooseCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AntiLooseCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

AntiLooseCleaner::AntiLooseCleaner(float fSampMaxLooseThreshold,
                                   float etaLooseThreshold,
                                   float emfLowLooseThreshold,
                                   float emfHighLooseThreshold,
                                   float hecfLooseThreshold):
  LooseCleaner(fSampMaxLooseThreshold, 
               etaLooseThreshold, 
               emfLowLooseThreshold, 
               emfHighLooseThreshold, 
               hecfLooseThreshold){}

bool AntiLooseCleaner::operator()(const pHypoJet& jet) const{
  return !LooseCleaner::operator()(jet);
}


std::string  
AntiLooseCleaner::getName() const noexcept {return "AntiLooseCleaner";}



