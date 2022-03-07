// this is a -*- C++ -*- file
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////
/// \class JetTrackSelectionAlg
///
/// Algorithm tasked to create a a filtered collection of tracks using a InDetTrackSelectionTool.
/// The algorithm writes out a container in VIEW_ELEMENTS mode.
///

#ifndef JetTrackSelectionAlg_H
#define JetTrackSelectionAlg_H

#include "AnaAlgorithm/AnaReentrantAlgorithm.h"
#include "AsgTools/ToolHandle.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/WriteHandleKey.h"
#include "StoreGate/ShallowCopyDecorDeps.h"

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "AthContainers/ConstDataVector.h"

class IJetExecuteTool;

class JetTrackSelectionAlg : public EL::AnaReentrantAlgorithm { 

public:

  JetTrackSelectionAlg(const std::string & n, ISvcLocator* l) : EL::AnaReentrantAlgorithm(n,l) {}
  using EL::AnaReentrantAlgorithm::AnaReentrantAlgorithm;

  /// Athena algorithm's Hooks
  StatusCode  initialize() override;
  StatusCode  execute(const EventContext& ctx) const override;

private:
  ToolHandle<InDet::IInDetTrackSelectionTool> m_selector {this, "TrackSelector", "", "track selection tool"};
  
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_input = {this, "InputContainer", "", "The input tracks"};
  SG::WriteHandleKey<xAOD::TrackParticleContainer> m_output= {this, "OutputContainer", "JetSelectedTracks", "The output filtered tracks"};

  // this is to tell the scheduler to which decorations will be accessed from the ouput container : 
  SG::ShallowCopyDecorDeps<xAOD::TrackParticleContainer> m_decorDeps { this, "DecorDeps", {}, "List of decorations to propagate through the view container" };
};

#endif
