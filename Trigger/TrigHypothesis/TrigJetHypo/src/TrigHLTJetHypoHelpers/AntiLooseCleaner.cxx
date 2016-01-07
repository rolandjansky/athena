/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AntiLooseCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "./AntiLooseCleaner.h"
#include "xAODJet/Jet.h"  //ICleaner

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

bool AntiLooseCleaner::operator()(const xAOD::Jet* jet) const{
  return !LooseCleaner::operator()(jet);
}


std::string  
AntiLooseCleaner::getName() const noexcept {return "AntiLooseCleaner";}



