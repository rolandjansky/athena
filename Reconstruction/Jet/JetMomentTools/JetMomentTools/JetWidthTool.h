/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetWidthTool.h

#ifndef JetMomentTools_JetWidthTool_H
#define JetMomentTools_JetWidthTool_H

/// Steven Schramm and David Adams \n
/// February 2014
///
/// Tool to calculate the jet width.

#include "JetRec/JetModifierBase.h"
#include "AsgTools/ToolHandle.h"

#include "PFlowUtils/IWeightPFOTool.h"

class JetWidthTool : public JetModifierBase {
  ASG_TOOL_CLASS(JetWidthTool, IJetModifier)
    
public:

  // Constructor from tool name.
  JetWidthTool(std::string myname);

  // Inherited method to modify a jet.
  // Calls width and puts the result on the jet.
  virtual int modifyJet(xAOD::Jet& jet) const;

  // Local method to calculate and return the width.
  //double width(const xAOD::Jet& jet) const;
  double width(const xAOD::Jet& jet, double& widthEta, double& widthPhi) const;

private:
};

#endif
