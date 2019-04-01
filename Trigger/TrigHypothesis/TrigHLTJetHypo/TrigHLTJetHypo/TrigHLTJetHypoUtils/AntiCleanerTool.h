/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ANTICLEANERTOOL_H
#define TRIGHLTJETHYPO_ANTICLEANERTOOL_H


// ********************************************************************
//
// NAME:     AntiBaiscCleanerTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

class AntiCleanerTool: public extends<AthAlgTool, ICleanerTool> {
  /* select jets that fail the run 1 cleaning cuts */
public:
  AntiCleanerTool(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);
  
  ~AntiCleanerTool(){}
  
  bool select(const pHypoJet&) const override;
  std::string getName() const noexcept override;
  virtual std::string toString() const noexcept;

 private:
  ToolHandle<ICleanerTool> m_cleaner {this, "cleaner", {}, "cleaner to invert"};
  
};

#endif
