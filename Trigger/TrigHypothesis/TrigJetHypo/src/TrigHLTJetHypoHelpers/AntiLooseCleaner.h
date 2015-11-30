/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_ANTILOOSECLEANER_H
#define TRIGJETHYPO_ANTILOOSECLEANER_H


// ********************************************************************
//
// NAME:     AntiLooseCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "./LooseCleaner.h"
#include "xAODJet/Jet.h"

class AntiLooseCleaner: public LooseCleaner{
  /* select jets that fail the loose cleaning cuts */
public:
  AntiLooseCleaner(float fSampMaxLooseThreshold,
                   float etaLooseThreshold,
                   float emfLowLooseThreshold,
                   float emfHighLooseThreshold,
                   float hecfLooseThreshold);

  bool operator()(const xAOD::Jet* jet) const override;
  std::string getName() const noexcept override;

};

#endif
