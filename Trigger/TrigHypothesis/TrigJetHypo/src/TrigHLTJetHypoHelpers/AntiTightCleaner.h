/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_ANTITIGHTCLEANER_H
#define TRIGJETHYPO_ANTITIGHTCLEANER_H


// ********************************************************************
//
// NAME:     AntiTightCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "./TightCleaner.h"
#include "xAODJet/Jet.h"

class AntiTightCleaner: public TightCleaner{
  /* select jets that fail the tight cleaning cuts */
public:
  AntiTightCleaner(float fSampMaxTightThreshold,
                   float etaTightThreshold,
                   float emfLowTightThreshold,
                   float emfHighTightThreshold,
                   float hecfTightThreshold);

  bool operator()(const xAOD::Jet* jet) const override;
  std::string getName() const noexcept override;
};

#endif
