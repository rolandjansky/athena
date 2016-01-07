/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AntiLlpCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "./AntiLlpCleaner.h"
#include "xAODJet/Jet.h"


AntiLlpCleaner::AntiLlpCleaner(float fSampMaxLlpThreshold,
                               float negELlpThreshold,
                               float hecfLlpThreshold,
                               float hecqLlpThreshold,
                               float avLarQFLlpThreshold):
  LlpCleaner(fSampMaxLlpThreshold,
             negELlpThreshold,
             hecfLlpThreshold,
             hecqLlpThreshold,
             avLarQFLlpThreshold){}

bool AntiLlpCleaner::operator()(const xAOD::Jet* jet) const{
  return !LlpCleaner::operator()(jet);
}

std::string
AntiLlpCleaner::getName() const noexcept {return "AntiLlpCleaner";}



