/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AntiBasicCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AntiBasicCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

AntiBasicCleaner::AntiBasicCleaner(float n90Threshold,
                                   float presamplerThreshold,
                                   float negativeEThreshold):
  BasicCleaner(n90Threshold, presamplerThreshold, negativeEThreshold){}

bool AntiBasicCleaner::operator()(const pHypoJet& jet) const{
  return !BasicCleaner::operator()(jet);
}

std::string
AntiBasicCleaner::getName() const noexcept {return "AntiBasicCleaner";}



