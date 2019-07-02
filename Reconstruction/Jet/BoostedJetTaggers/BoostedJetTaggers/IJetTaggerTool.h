// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJETTAGGERTOOL_H
#define IJETTAGGERTOOL_H

#include "PATInterfaces/CorrectionCode.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "xAODJet/JetContainer.h"

class IJetTaggerTool : public virtual IJetSelectorTool {

  ASG_TOOL_INTERFACE( CP::IJetTaggerTool )

  public:

  virtual ~IJetTaggerTool() {}
  
  virtual Root::TAccept tag(const xAOD::Jet& jet) const = 0;
  
  virtual StatusCode decorateTruthLabel(const xAOD::Jet& jet, std::string decorName="FatjetTruthLabel", double dR_truthJet=0.75, double dR_truthPart=0.75, double mLowTop=140, double mHighTop=-1, double mLowW=50, double mHighW=100, double mLowZ=60, double mHighZ=110) const = 0;

};

#endif
