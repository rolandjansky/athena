/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetTrackMomentsTool.h

#ifndef JETMOMENTTOOLS_JETTRACKMOMENTSTOOL_H
#define JETMOMENTTOOLS_JETTRACKMOMENTSTOOL_H

/// Steven Schramm \n
/// February 2014
///
/// Tool to calculate general track-based jet moments
///
/// Moments to calculate:
///     NumTrkPtXXX
///     SumPtTrkPtXXX
///     TrackWidthPtXXX
/// where the track pT threhsold is XXX MeV.

#include "AsgTools/ToolHandle.h"
#include "JetInterface/IJetTrackSelector.h"
#include "JetRec/JetModifierBase.h"
#include "JetEDM/TrackVertexAssociation.h"

#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/Vertex.h" 
#include "xAODTracking/VertexContainer.h" 

#include <vector>
#include <string>


class JetTrackMomentsTool : public JetModifierBase {
  ASG_TOOL_CLASS(JetTrackMomentsTool,IJetModifier)
    
public:

  // Constructor from tool name
  JetTrackMomentsTool(const std::string& name);

  // Initialization.
  StatusCode initialize();

  // Inherited methods to modify a jet
  // Calls getTrackMoments and puts the results in the jet
  virtual int modifyJet(xAOD::Jet& jet) const;

private:

  // Configurable parameters
  std::string m_assocTracksName;
  std::vector<float> m_minTrackPt;
  ToolHandle<IJetTrackSelector> m_htsel;
        
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

  SG::ReadHandleKey< xAOD::VertexContainer> m_vertexContainer_key;
  SG::ReadHandleKey<jet::TrackVertexAssociation> m_tva_key;

};

#endif

