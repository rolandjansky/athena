/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SimpleJetTrackSelectionTool.h

/// class SimpleJetTrackSelectionTool
///
/// Returns all tracks.

#ifndef JETRECTOOLS_SimpleJetTrackSelectionTool_H
#define JETRECTOOLS_SimpleJetTrackSelectionTool_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"
#include "JetInterface/IJetTrackSelector.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "AsgTools/ToolHandle.h"
#include "AthContainers/ConstDataVector.h"

class SimpleJetTrackSelectionTool
  : public asg::AsgTool,
    virtual public IJetExecuteTool,
    virtual public IJetTrackSelector {
  ASG_TOOL_CLASS2(SimpleJetTrackSelectionTool, IJetExecuteTool, IJetTrackSelector)
  
public:
  SimpleJetTrackSelectionTool(const std::string &name);

  // Inherited interface.
  StatusCode initialize() ;
  int execute() const ;
  int keep(const xAOD::TrackParticle& trk) const;

  void selectTracks(const xAOD::TrackParticleContainer& inCont, ConstDataVector<xAOD::TrackParticleContainer>& outCont) const;
  void selectTracks(const xAOD::TrackParticleContainer& inCont, std::vector<const xAOD::TrackParticle*> outCont) const;
  
protected:
  template<class T>
  void selectionLoop(const xAOD::TrackParticleContainer& inCont, T& outCont) const;
  float m_ptmin;

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inputContainer_key;
  SG::WriteHandleKey<ConstDataVector< xAOD::TrackParticleContainer>> 
    m_outputContainer_key;


};

#endif
