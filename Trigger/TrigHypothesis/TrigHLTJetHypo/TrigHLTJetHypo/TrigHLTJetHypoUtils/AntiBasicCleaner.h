/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ANTIBASICCLEANER_H
#define TRIGHLTJETHYPO_ANTIBASICCLEANER_H


// ********************************************************************
//
// NAME:     AntiBaiscCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "./BasicCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

class AntiBasicCleaner: public BasicCleaner{
  /* select jets that fail the run 1 cleaning cuts */
public:
  AntiBasicCleaner(float n90Threshold,
                   float presamplerThreshold,
                   float negativeEThreshold);
  ~AntiBasicCleaner(){}
  
  bool operator()(const pHypoJet&) const override;
  std::string getName() const noexcept override;
};

#endif
