/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_LOOSECLEANERTOOL_H
#define TRIGHLTJETHYPO_LOOSECLEANERTOOL_H


// ********************************************************************
//
// NAME:     LooseCleanerTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "AthenaBaseComps/AthAlgTool.h"


class LooseCleanerTool: public extends<AthAlgTool, ICleanerTool>{
  /* select jets that satisfy the loose cleaning cuts */
 public:

  LooseCleanerTool(const std::string& type,
               const std::string& name,
               const IInterface* parent);
  
  ~LooseCleanerTool() {}

  bool select(const pHypoJet& jet) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  Gaudi::Property<float>  m_fSampMaxLooseThreshold{
    this, "fSampMaxLooseThreshold", 0.80, "fSampMaxLooseThreshold"};
  
  Gaudi::Property<float>  m_etaLooseThreshold{
    this, "etaLooseThreshold", 2.0, "etaLooseThreshold"};
  
  Gaudi::Property<float>  m_emfLowLooseThreshold{
    this, "emfLowLooseThreshold", 0.10, "emfLowLooseThreshold"};
  
  Gaudi::Property<float>  m_emfHighLooseThreshold{
    this, "emfHighLooseThreshold", 0.99, "emfHighLooseThreshold"};
  
  Gaudi::Property<float>  m_hecfLooseThreshold{
    this, "hecfLooseThreshold", 0.85, "hecfLooseThreshold"};

};

#endif
