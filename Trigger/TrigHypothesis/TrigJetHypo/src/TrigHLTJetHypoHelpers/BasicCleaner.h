/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_BASICCLEANER_H
#define TRIGJETHYPO_BASICCLEANER_H


// ********************************************************************
//
// NAME:     BasicCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "xAODJet/Jet.h"

class BasicCleaner: public ICleaner{
  /* select jets that satisfy the run 1 cleaning cuts */
 public:
  BasicCleaner(float n90Threshold,
               float presamplerThreshold,
               float negativeEThreshold);
  
  bool operator()(const xAOD::Jet* jet) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  float m_n90Threshold;
  float m_presamplerThreshold;
  float m_negativeEThreshold;
};

#endif
