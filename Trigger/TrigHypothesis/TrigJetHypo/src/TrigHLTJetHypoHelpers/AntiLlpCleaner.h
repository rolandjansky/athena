/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_ANTILLPCLEANER_H
#define TRIGJETHYPO_ANTILLPCLEANER_H


// ********************************************************************
//
// NAME:     AntiLlpCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "./LlpCleaner.h"
#include "xAODJet/Jet.h"

class AntiLlpCleaner: public LlpCleaner{
  /* select jets that fail the long-lived particle cleaning cuts */
public:
  AntiLlpCleaner(float fSampMaxLlpThreshold,
                 float negELlpThreshold,
                 float hecfLlpThreshold,
                 float hecqLlpThreshold,
                 float avLarQFLlpThreshold);

  ~AntiLlpCleaner(){}

  bool operator()(const xAOD::Jet* jet) const override;
  std::string getName() const noexcept override;
};

#endif
