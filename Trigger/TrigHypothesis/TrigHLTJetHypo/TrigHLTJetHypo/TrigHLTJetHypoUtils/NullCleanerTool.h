/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_NULLCLEANERTOOL_H
#define TRIGHLTJETHYPO_NULLCLEANERTOOL_H


// ********************************************************************
//
// NAME:     NullCleanerTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace AOD{
  class Jet;
}

class NullCleanerTool:  public extends<AthAlgTool, ICleanerTool> {
  /* apply no cleaning cuts */
public:
  
  NullCleanerTool(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);
  
  ~NullCleanerTool(){}

  bool select(const pHypoJet&) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;
};


#endif
