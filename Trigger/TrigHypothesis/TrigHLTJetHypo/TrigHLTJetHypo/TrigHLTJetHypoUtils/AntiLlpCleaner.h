/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ANTILLPCLEANER_H
#define TRIGHLTJETHYPO_ANTILLPCLEANER_H


// ********************************************************************
//
// NAME:     AntiLlpCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "./LlpCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

class AntiLlpCleaner: public LlpCleaner{
  /* select jets that fail the long-lived particle cleaning cuts */
public:
  AntiLlpCleaner(float fSampMaxLlpThreshold,
                 float negELlpThreshold,
                 float hecfLlpThreshold,
                 float hecqLlpThreshold,
                 float avLarQFLlpThreshold);

  ~AntiLlpCleaner(){}

  bool operator()(const pHypoJet& jet) const override;
  std::string getName() const noexcept override;
};

#endif
