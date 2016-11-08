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
  EtaEtCleaner(double etaMin,
               double etaMax,
               double etMin,
               double etMax);

  ~EtaEtCleaner(){}
  
  bool operator()(const pHypoJet&) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  double m_etaMin;
  double m_etaMax;
  double m_etMin;
  double m_etMax;
};

#endif
