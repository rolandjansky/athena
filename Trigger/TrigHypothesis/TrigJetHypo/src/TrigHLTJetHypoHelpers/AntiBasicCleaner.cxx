/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AntiBasicCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "./AntiBasicCleaner.h"
#include "xAODJet/Jet.h"  //ICleaner

#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h"

AntiBasicCleaner::AntiBasicCleaner(float n90Threshold,
                                   float presamplerThreshold,
                                   float negativeEThreshold):
  BasicCleaner(n90Threshold, presamplerThreshold, negativeEThreshold){}

bool AntiBasicCleaner::operator()(const xAOD::Jet* jet) const{
  return !BasicCleaner::operator()(jet);
}





