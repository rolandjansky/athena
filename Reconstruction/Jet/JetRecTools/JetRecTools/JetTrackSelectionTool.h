// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRECTOOLS_JETTRACKSELECTIONTOOL_H
#define JETRECTOOLS_JETTRACKSELECTIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/ConstDataVector.h"

class JetTrackSelectionTool
  : public asg::AsgTool,
    virtual public IJetExecuteTool {
  ASG_TOOL_CLASS(JetTrackSelectionTool, IJetExecuteTool)
  
  public:
  JetTrackSelectionTool(const std::string &name);

  virtual StatusCode initialize() ;
  
  virtual int execute() const ;

  void selectTracks(const xAOD::TrackParticleContainer& inCont, ConstDataVector<xAOD::TrackParticleContainer>& outCont) const;
  void selectTracks(const xAOD::TrackParticleContainer& inCont, std::vector<const xAOD::TrackParticle*> outCont) const;
  
protected:
  template<class T>
  void selectionLoop(const xAOD::TrackParticleContainer& inCont, T& outCont) const;

  ToolHandle<Trk::ITrackSelectorTool> m_selector;
  float m_ptMin;
  float m_etaMin, m_etaMax;

  std::string m_inputContainer;
  std::string m_outputContainer;

};

#endif
