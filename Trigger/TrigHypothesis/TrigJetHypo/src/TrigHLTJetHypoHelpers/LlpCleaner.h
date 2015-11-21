/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_LLPCLEANER_H
#define TRIGJETHYPO_LLPCLEANER_H


// ********************************************************************
//
// NAME:     LlpCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h" //ICleaner
#include "xAODJet/Jet.h"

class LlpCleaner: public ICleaner{
  /* select jets that satisfy the long-lived particle cleaning cuts */
 public:
  LlpCleaner(float fSampMaxLlpThreshold,
             float negELlpThreshold,
             float hecfLlpThreshold,
             float hecqLlpThreshold,
             float avLarQFLlpThreshold);
  
  bool operator()(const xAOD::Jet* jet) const override;

 private:
  float m_fSampMaxLlpThreshold;
  float m_negELlpThreshold;
  float m_hecfLlpThreshold;
  float m_hecqLlpThreshold;
  float m_avLarQFLlpThreshold;
};

#endif
