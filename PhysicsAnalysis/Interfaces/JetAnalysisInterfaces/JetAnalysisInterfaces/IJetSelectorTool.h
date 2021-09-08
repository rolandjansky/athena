// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetAnalysisInterfaces_IJetSelectorTool_H_
#define JetAnalysisInterfaces_IJetSelectorTool_H_

#include "AsgTools/IAsgTool.h"

#include "xAODJet/Jet.h"

#include "PATCore/TAccept.h"

class IJetSelectorTool :  virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IJetSelectorTool)

  public:

  virtual Root::TAccept& tag(const xAOD::Jet& jet) const = 0;

};

#endif
