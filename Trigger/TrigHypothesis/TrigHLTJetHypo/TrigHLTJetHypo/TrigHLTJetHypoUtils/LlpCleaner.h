/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_LLPCLEANER_H
#define TRIGHLTJETHYPO_LLPCLEANER_H


// ********************************************************************
//
// NAME:     LlpCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h" //ICleaner
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

class LlpCleaner: public ICleaner{
  /* select jets that satisfy the long-lived particle cleaning cuts */
 public:
  LlpCleaner(float fSampMaxLlpThreshold,
             float negELlpThreshold,
             float hecfLlpThreshold,
             float hecqLlpThreshold,
             float avLarQFLlpThreshold);
  
  ~LlpCleaner() {}

  bool operator()(const pHypoJet& jet) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  float m_fSampMaxLlpThreshold;
  float m_negELlpThreshold;
  float m_hecfLlpThreshold;
  float m_hecqLlpThreshold;
  float m_avLarQFLlpThreshold;
};

#endif
