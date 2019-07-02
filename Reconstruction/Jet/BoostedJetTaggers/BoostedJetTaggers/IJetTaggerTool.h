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
  
  virtual StatusCode decorateTruthLabel(const xAOD::Jet& jet) const = 0;

};

#endif
