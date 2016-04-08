/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TIGHTCLEANER_H
#define TRIGHLTJETHYPO_TIGHTCLEANER_H


// ********************************************************************
//
// NAME:     TightCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

class TightCleaner: public ICleaner{
  /* select jets that satisfy the tight cleaning cuts */
 public:
  TightCleaner(float fSampMaxTightThreshold,
               float etaTightThreshold,
               float emfLowTightThreshold,
               float emfHighTightThreshold,
               float hecfTightThreshold);
  
  ~TightCleaner() {}

  bool operator()(const pHypoJet& jet) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  float m_fSampMaxTightThreshold;
  float m_etaTightThreshold;
  float m_emfLowTightThreshold;
  float m_emfHighTightThreshold;
  float m_hecfTightThreshold;
};

#endif
