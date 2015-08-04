/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_ANTIBASICCLEANER_H
#define TRIGJETHYPO_ANTIBASICCLEANER_H


// ********************************************************************
//
// NAME:     AntiBaiscCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h" // ICleaner
#include "./BasicCleaner.h"
#include "xAODJet/Jet.h"

class AntiBasicCleaner: public BasicCleaner{
  /* select jets that fail the run 1 cleaning cuts */
public:
  AntiBasicCleaner(float n90Threshold,
                   float presamplerThreshold,
                   float negativeEThreshold);
  
  bool operator()(const xAOD::Jet* jet) const override;
};

#endif
