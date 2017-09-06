/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetTrackSelectionTool.h

/// class JetTrackSelectionTool
///
/// Selects tracks using an InDet selection tool.

#ifndef JETRECTOOLS_JETTRACKSELECTIONTOOL_H
#define JETRECTOOLS_JETTRACKSELECTIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"
#include "JetInterface/IJetTrackSelector.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "AsgTools/ToolHandle.h"
#include "AthContainers/ConstDataVector.h"

class JetTrackSelectionTool
  : public asg::AsgTool,
    virtual public IJetExecuteTool,
    virtual public IJetTrackSelector {
  ASG_TOOL_CLASS2(JetTrackSelectionTool, IJetExecuteTool, IJetTrackSelector)
  
public:

  JetTrackSelectionTool(const std::string &name);

  // Inherited interface.
  StatusCode initialize() ;
  int execute() const ;
  int keep(const xAOD::TrackParticle& trk) const;

  void selectTracks(const xAOD::TrackParticleContainer& inCont, ConstDataVector<xAOD::TrackParticleContainer>& outCont) const;
  void selectTracks(const xAOD::TrackParticleContainer& inCont, std::vector<const xAOD::TrackParticle*> outCont) const;
  
protected:

  template<class T>
  void selectionLoop(const xAOD::TrackParticleContainer& inCont, T& outCont) const;

  ToolHandle<InDet::IInDetTrackSelectionTool> m_hidselector;
  
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inCont_key;
  SG::WriteHandleKey<ConstDataVector<xAOD::TrackParticleContainer>> 
    m_outCont_key;
};

#endif
