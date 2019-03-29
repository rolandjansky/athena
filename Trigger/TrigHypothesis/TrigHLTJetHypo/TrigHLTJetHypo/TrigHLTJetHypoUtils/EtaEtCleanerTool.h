/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ETAETCLEANERTOOL_H
#define TRIGHLTJETHYPO_ETAETCLEANERTOOL_H

// ********************************************************************
//
// NAME:     EtaEtCleanerTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "AthenaBaseComps/AthAlgTool.h"

class EtaEtCleanerTool:  public extends<AthAlgTool, ICleanerTool>{
public:

  EtaEtCleanerTool(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);
  
  ~EtaEtCleanerTool(){}
  
  bool select(const pHypoJet&) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:

  Gaudi::Property<double> m_etaMin{this, "etaMin", {},"etaMin"};
  Gaudi::Property<double> m_etaMax{this, "etaMax", {},"etaMax"};
  Gaudi::Property<double> m_etMin{this, "etMin", {},"etMin"};
  Gaudi::Property<double> m_etMax{this, "etMax", {},"etMax"};

};

#endif
