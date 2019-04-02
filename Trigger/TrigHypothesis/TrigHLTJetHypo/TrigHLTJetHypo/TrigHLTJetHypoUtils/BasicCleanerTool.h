/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_BASICCLEANERTOOL_H
#define TRIGHLTJETHYPO_BASICCLEANERTOOL_H


// ********************************************************************
//
// NAME:     BasicCleanerTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "AthenaBaseComps/AthAlgTool.h"


class BasicCleanerTool:  public extends<AthAlgTool, ICleanerTool> {
  /* select jets that satisfy the run 1 cleaning cuts */
 public:
  BasicCleanerTool(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);
  
  ~BasicCleanerTool() {}


  bool select(const pHypoJet& jet) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;

 private:
  Gaudi::Property<float> m_n90Threshold{
    this, "n90Threshold", 2.0, "n90Threshold"};

  Gaudi::Property<float> m_presamplerThreshold{
    this, "presamplerThreshold", 0.9, "presamplerThreshold"};

  
  Gaudi::Property<float> m_negativeEThreshold{
    this, "negativeEThreshold", -60e3, "negativeEThreshold"};


  Gaudi::Property<float>
    m_qmeanThreshold {this, "qmeanCleaningThreshold", 0.8, ""};
  
  Gaudi::Property<float>
    m_hecQThreshold {this, "HECQCleaningThreshold", 0.5, ""};
  
  Gaudi::Property<float>
    m_hecFThreshold {this, "HECfCleaningThreshold", 0.5, ""};
  
  Gaudi::Property<float>
    m_larQThreshol {this, "LArQCleaningThreshold", 0.8, ""};
  
  Gaudi::Property<float>
    m_emFThreshold {this, "EMfCleaningThreshold", 0.95, ""};
 
};

#endif
