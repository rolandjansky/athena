// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetTrackSelectionTool/.h

/* class JetTrackSelectionTool2
 
  Simplified version of the tool which wraps the selector 
  but does not define input/output containers.
 */

#ifndef JETRECTOOLS_JETTRACKSELECTIONTOOL2_H
#define JETRECTOOLS_JETTRACKSELECTIONTOOL2_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"
#include "JetInterface/IJetTrackSelector.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "AsgTools/ToolHandle.h"
#include "AthContainers/ConstDataVector.h"

class JetTrackSelectionTool2 : public asg::AsgTool,
  virtual public IJetTrackSelector {
  ASG_TOOL_CLASS(JetTrackSelectionTool2,IJetTrackSelector)
  
public:

  JetTrackSelectionTool2(const std::string &name);

  // Inherited interface.
  StatusCode initialize() ;
  int keep(const xAOD::TrackParticle& trk) const;

protected:

  ToolHandle<InDet::IInDetTrackSelectionTool> m_hidselector;

};

#endif
