/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_TIGHTCLEANER_H
#define TRIGJETHYPO_TIGHTCLEANER_H


// ********************************************************************
//
// NAME:     TightCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h" //ICleaner
#include "xAODJet/Jet.h"

class TightCleaner: public ICleaner{
  /* select jets that satisfy the tight cleaning cuts */
 public:
  TightCleaner(float fSampMaxTightThreshold,
               float etaTightThreshold,
               float emfLowTightThreshold,
               float emfHighTightThreshold,
               float hecfTightThreshold);
  
  bool operator()(const xAOD::Jet* jet) const override;

 private:
  float m_fSampMaxTightThreshold;
  float m_etaTightThreshold;
  float m_emfLowTightThreshold;
  float m_emfHighTightThreshold;
  float m_hecfTightThreshold;
};

#endif
