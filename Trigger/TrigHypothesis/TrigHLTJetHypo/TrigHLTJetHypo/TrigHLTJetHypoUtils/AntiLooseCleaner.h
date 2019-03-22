/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ANTILOOSECLEANER_H
#define TRIGHLTJETHYPO_ANTILOOSECLEANER_H


// ********************************************************************
//
// NAME:     AntiLooseCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "./LooseCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

class AntiLooseCleaner: public LooseCleaner{
  /* select jets that fail the loose cleaning cuts */
public:
  AntiLooseCleaner(float fSampMaxLooseThreshold,
                   float etaLooseThreshold,
                   float emfLowLooseThreshold,
                   float emfHighLooseThreshold,
                   float hecfLooseThreshold);

  ~AntiLooseCleaner(){}

  bool operator()(const pHypoJet& jet) const override;
  std::string getName() const noexcept override;

};

#endif
