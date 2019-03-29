/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_LLPCLEANERTOOL_H
#define TRIGHLTJETHYPO_LLPCLEANERTOOL_H



// ********************************************************************
//
// NAME:     LlpCleanerTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "AthenaBaseComps/AthAlgTool.h"


class LlpCleanerTool: public extends<AthAlgTool, ICleanerTool> {
  /* select jets that satisfy the long-lived particle cleaning cuts */
 public:

  LlpCleanerTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);
  ~LlpCleanerTool() {}

  bool select(const pHypoJet& jet) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  Gaudi::Property<float>  m_fSampMaxLlpThreshold{
    this, "fSampMaxLlpThreshold", 0.85,"fSampMaxLlpThreshold"};

  Gaudi::Property<float>  m_negELlpThreshold{
    this, "negELlpThreshold", 10e3,"negELlpThreshold"};

  Gaudi::Property<float>  m_hecfLlpThreshold{
    this, "hecfLlpThreshold", 0.5,"hecfLlpThreshold"};

  Gaudi::Property<float>  m_hecqLlpThreshold{
    this, "hecqLlpThreshold", 0.5,"hecqLlpThreshold"};

  Gaudi::Property<float>  m_avLarQFLlpThreshold{
    this, "avLarQFLlpThreshold", 0.8*65535,"avLarQFLlpThreshold"};

};

#endif
