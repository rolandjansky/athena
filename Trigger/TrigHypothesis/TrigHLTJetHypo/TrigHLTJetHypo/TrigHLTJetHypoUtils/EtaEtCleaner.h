/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ETAETCLEANER_H
#define TRIGHLTJETHYPO_ETAETCLEANER_H


// ********************************************************************
//
// NAME:     EtaEtCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

class EtaEtCleaner: public ICleaner{
  /* select jets that fail the run 1 cleaning cuts */
public:
  EtaEtCleaner(float etaMin,
               float etaMax,
               float etMin,
               float etMax);

  ~EtaEtCleaner(){}
  
  bool operator()(const pHypoJet&) const override;
  std::string getName() const noexcept override;

 private:
  float m_etaMin;
  float m_etaMax;
  float m_etMin;
  float m_etMax;
};

#endif
