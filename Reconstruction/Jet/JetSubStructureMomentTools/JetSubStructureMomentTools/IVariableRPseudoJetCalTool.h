/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_ivariablerpseudojetcaltool_header
#define jetsubstructuremomenttools_ivariablerpseudojetcaltool_header

#include "AsgTools/IAsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetSubStructureUtils/IPseudoJetCalibrationTool.h"

namespace fastjet{
  class ClusterSequence;
  class PseudoJet;
}
class IVariableRPseudoJetCalTool : public virtual asg::IAsgTool, public virtual IPseudoJetCalibrationTool {
  public:
    ASG_TOOL_INTERFACE( IVariableRPseudoJetCalTool ) //declares the interface to athena
};

#endif 
