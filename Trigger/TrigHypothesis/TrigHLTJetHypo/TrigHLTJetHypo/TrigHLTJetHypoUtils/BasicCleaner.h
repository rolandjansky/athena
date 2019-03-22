/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_BASICCLEANER_H
#define TRIGHLTJETHYPO_BASICCLEANER_H


// ********************************************************************
//
// NAME:     BasicCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

class BasicCleaner: public ICleaner{
  /* select jets that satisfy the run 1 cleaning cuts */
 public:
  BasicCleaner(float n90Threshold,
               float presamplerThreshold,
               float negativeEThreshold);
  
  ~BasicCleaner() {}


  bool operator()(const pHypoJet& jet) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  float m_n90Threshold;
  float m_presamplerThreshold;
  float m_negativeEThreshold;
};

#endif
