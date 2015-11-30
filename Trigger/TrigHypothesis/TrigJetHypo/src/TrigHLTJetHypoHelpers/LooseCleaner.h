/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_LOOSECLEANER_H
#define TRIGJETHYPO_LOOSECLEANER_H


// ********************************************************************
//
// NAME:     LooseCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "xAODJet/Jet.h"

class LooseCleaner: public ICleaner{
  /* select jets that satisfy the loose cleaning cuts */
 public:
  LooseCleaner(float fSampMaxLooseThreshold,
               float etaLooseThreshold,
               float emfLowLooseThreshold,
               float emfHighLooseThreshold,
               float hecfLooseThreshold);
  
  bool operator()(const xAOD::Jet* jet) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  float m_fSampMaxLooseThreshold;
  float m_etaLooseThreshold;
  float m_emfLowLooseThreshold;
  float m_emfHighLooseThreshold;
  float m_hecfLooseThreshold;
};

#endif
