/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ANTITIGHTCLEANER_H
#define TRIGHLTJETHYPO_ANTITIGHTCLEANER_H


// ********************************************************************
//
// NAME:     AntiTightCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "./TightCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

class AntiTightCleaner: public TightCleaner{
  /* select jets that fail the tight cleaning cuts */
public:
  AntiTightCleaner(float fSampMaxTightThreshold,
                   float etaTightThreshold,
                   float emfLowTightThreshold,
                   float emfHighTightThreshold,
                   float hecfTightThreshold);

  ~AntiTightCleaner() {}

  bool operator()(const pHypoJet& jet) const override;
  std::string getName() const noexcept override;
};

#endif
