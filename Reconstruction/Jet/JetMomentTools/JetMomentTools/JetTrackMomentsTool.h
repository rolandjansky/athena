/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetTrackMomentsTool.h

#ifndef JETMOMENTTOOLS_JETTRACKMOMENTSTOOL_H
#define JETMOMENTTOOLS_JETTRACKMOMENTSTOOL_H

/// Steven Schramm \n
/// February 2014
///
/// Updated for Run 3 by Bill Balunas
/// March 2020
///
/// Tool to calculate general track-based jet moments
///
/// Moments to calculate:
///     NumTrkPtXXX
///     SumPtTrkPtXXX
///     TrackWidthPtXXX
/// where the track pT threshold is XXX MeV.

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "JetInterface/IJetTrackSelector.h"
#include "JetInterface/IJetDecorator.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKeyArray.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include <vector>
#include <string>


class JetTrackMomentsTool : public asg::AsgTool,
                            virtual public IJetDecorator {
  ASG_TOOL_CLASS(JetTrackMomentsTool,IJetDecorator)

public:

  // Constructor from tool name
  JetTrackMomentsTool(const std::string& name);

  // Initialization.
  StatusCode initialize() override;

  // Calls getTrackMoments and puts the results in the jet
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

private:

  // Configurable parameters
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for input jet container"};
  Gaudi::Property<std::string> m_assocTracksName{this, "AssociatedTracks", "", "Name of associated tracks collection"};
  Gaudi::Property<std::vector<float> > m_minTrackPt{this, "TrackMinPtCuts", {}, "Vector of track pt cuts"};
  Gaudi::Property<bool> m_doPFlowMoments{this, "DoPFlowMoments", false, "Calculate PFlow Object track moments?"};

  ToolHandle<IJetTrackSelector> m_htsel{this, "TrackSelector", "", "Track selector tool"};

  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key{this, "VertexContainer", "", "SG key for vertex container"};
  SG::ReadHandleKey<jet::TrackVertexAssociation> m_tva_key{this, "TrackVertexAssociation", "", "SG key for track-vertex association"};

  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_keysNumTrk{this, "NumTrkDecorKeys", {}, "SG keys for NumTrk decoration (not to be configured manually!)"};
  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_keysSumPtTrk{this, "SumPtTrkDecorKeys", {}, "SG keys for SumPtTrk decoration (not to be configured manually!)"};
  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_keysTrkWidth{this, "TrackWidthDecorKeys", {}, "SG keys for TrackWidth decoration (not to be configured manually!)"};
  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_keysNumCPFO{this, "NumChargedPFODecorKeys", {}, "SG keys for NumChargedPFO decoration (not to be configured manually!)"};
  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_keysSumPtCPFO{this, "SumPtChargedPFODecorKeys", {}, "SG keys for SumPtChargedPFO decoration (not to be configured manually!)"};
  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_keysCPFOWidth{this, "ChargedPFOWidthDecorKeys", {}, "SG keys for ChargedPFO Width decoration (not to be configured manually!)"};

  // Private struct to make it unambiguous what each value is (rather than a vector)
  // Doubles for calculation for now - will be written as float in the aux store
  struct TrackMomentStruct { int numTrk; double sumPtTrk; double trackWidth; };

  // Local method to calculate NumTrk, SumPtTrk, and TrackWidth for all vertices
  const std::vector<TrackMomentStruct>
  getTrackMoments(const xAOD::Jet& jet, const xAOD::VertexContainer* vertices,
                  const float minTrackPt, const std::vector<const xAOD::TrackParticle*>& tracks,
                  const jet::TrackVertexAssociation* tva) const;

  // Local method to calculate NumTrk, SumPtTrk, and TrackWidth for one vertex
  TrackMomentStruct
  getTrackMoments(const xAOD::Jet&, const xAOD::Vertex* vertex, const float minTrackPt,
                  const std::vector<const xAOD::TrackParticle*>& tracks,
                  const jet::TrackVertexAssociation* tva) const;

  // Parse the float to get a moment base name
  const std::string getMomentBaseName(const float minTrackPt) const;

};

#endif

