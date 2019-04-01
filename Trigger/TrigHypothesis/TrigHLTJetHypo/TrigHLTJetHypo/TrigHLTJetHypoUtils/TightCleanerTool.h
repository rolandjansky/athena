/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TIGHTCLEANERTOOL_H
#define TRIGHLTJETHYPO_TIGHTCLEANERTOOL_H


// ********************************************************************
//
// NAME:     TightCleanerTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TightCleanerTool:  public extends<AthAlgTool, ICleanerTool> {

  /* select jets that satisfy the tight cleaning cuts */
 public:
  DeclareInterfaceID(ICleanerTool, 1, 0);

  TightCleanerTool(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);
  
  
  ~TightCleanerTool() {}

  bool select(const pHypoJet& jet) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  Gaudi::Property<float>  m_fSampMaxTightThreshold{
    this, "fSampMaxTightThreshold", 0.8, "fSampMaxTightThreshold"};

  Gaudi::Property<float>  m_etaTightThreshold{
    this, "etaTightThreshold", 2.0, "etaTightThreshold"};

  Gaudi::Property<float>  m_emfLowTightThreshold{
    this, "emfLowTightThreshold", 0.10, "emfLowTightThreshold"};

  Gaudi::Property<float>  m_emfHighTightThreshold{
    this, "emfHighTightThreshold", 0.99, "emfHighTightThreshold"};

  Gaudi::Property<float>  m_hecfTightThreshold{
    this, "hecfTightThreshold", 0.85, "hecfTightThreshold"};
};

#endif
