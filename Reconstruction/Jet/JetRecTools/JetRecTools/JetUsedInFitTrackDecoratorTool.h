/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetUsedInFitTrackDecoratorTool.h

/// class JetUsedInFitTrackDecoratorTool
/// Decorates tracks with vectors of AMVF vertices and annealing weights
/// --> Needed for Run 3 TTVA

#ifndef JETRECTOOLS_JETUSEDINFITDECORATORTOOL_H
#define JETRECTOOLS_JETUSEDINFITDECORATORTOOL_H

// FrameWork inlcudes:
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Tool includes:
#include "JetInterface/IJetExecuteTool.h"
#include "InDetRecToolInterfaces/IInDetUsedInFitTrackDecoratorTool.h"

class JetUsedInFitTrackDecoratorTool
  : public asg::AsgTool,
    virtual public IJetExecuteTool
{

  ASG_TOOL_CLASS(JetUsedInFitTrackDecoratorTool, IJetExecuteTool)

public:

  // Constructor:
  JetUsedInFitTrackDecoratorTool(const std::string& name);

  // Inherited interface:
  StatusCode initialize();
  int execute() const;
  
protected:

  // Decoration tool:
  ToolHandle<InDet::IInDetUsedInFitTrackDecoratorTool> m_decoTool;

};

#endif
