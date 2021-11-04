/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetDRTrackAssocTool.h

#ifndef JETMOMENTTOOLS_JETDRTRACKASSOCTOOL_H
#define JETMOMENTTOOLS_JETDRTRACKASSOCTOOL_H

/// Teng Jian Khoo
/// Oct 2021
///
/// Tool to associate tracks to jets with deltaR
/// Only intended as a stopgap solution to ghost association
///

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "JetInterface/IJetDecorator.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKeyArray.h"

#include "xAODTracking/TrackParticleContainer.h"

#include <vector>
#include <string>


class JetDRTrackAssocTool : public asg::AsgTool,
                            virtual public IJetDecorator {
  ASG_TOOL_CLASS(JetDRTrackAssocTool,IJetDecorator)

public:

  // Constructor from tool name
  JetDRTrackAssocTool(const std::string& name);

  // Initialization.
  StatusCode initialize() override;

  // Calls getTrackMoments and puts the results in the jet
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

private:

  // Configurable parameters
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for input jet container"};  
  Gaudi::Property<float> m_dRcut{this, "DeltaRCut", 0.4, "DeltaR cut for jet-track association"};  
  
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackContainer_key{this, "TracksToMatch", "", "Name of track collection for matching"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_decorDRTracks{this, "DRTrack", "", "SG key for DRTracks decoration (not to be configured manually!)"};

  //const std::vector<const xAOD::TrackParticle*> getMatchedTracks(const xAOD::Jet&, const xAOD::TrackParticleContainer&, float) const;
  StatusCode fillTrackToJetMap(std::map<const xAOD::TrackParticle*, const xAOD::Jet*>&, const xAOD::TrackParticleContainer&, const xAOD::JetContainer&, float) const;

};

#endif

