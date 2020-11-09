/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetCustomVertexFractionTool.h

#ifndef JETMOMENTTOOLS_JETCUSTOMVERTEXFRACTIONTOOL_H
#define JETMOMENTTOOLS_JETCUSTOMVERTEXFRACTIONTOOL_H

/// Steven Schramm \n
/// February 2014
///
/// Tool to calculate the jet vertex fraction (JVF)
/// JVF is a vector<float> with respect to all vertices
///
/// Calculation requires three main types of information
///     1. Vertex container for the event (from evtStore), which the JVF vector is aligned with
///     2. Tracks associated to the input jet (in the jet aux store)
///     3. Track vertex association object (from evtStore)
///
/// Using this information, the procedure can be broken into three main steps:
///     1. Retrieve necessary information
///     2. Helper method to create the full vector given the information
///     3. Helper method to create the JVF value for one vertex given the information
///
/// Properties:
///  VertexContainer - name of the vertex container
///  AssociatedTracks - name for attribute holding the list of associated tracks
///  TrackSelector - tool to select tracks (none ==> no selection)
///  JVFName - name for the JVF array attribute (default is "JVF")


#include "AsgTools/ToolHandle.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "JetInterface/IJetTrackSelector.h"
#include "JetRec/JetModifierBase.h"
#include "JetMomentTools/JetVertexFractionTool.h"

#include <vector>
#include <string>

//class JetCustomVertexFractionTool : public JetModifierBase {
class JetCustomVertexFractionTool : public JetVertexFractionTool {
  ASG_TOOL_CLASS(JetCustomVertexFractionTool,IJetModifier)

public:  // methods

  // Constructor from tool name
  JetCustomVertexFractionTool(const std::string& name);

  // Inherited methods to modify a jet
  // Computes JVF for all jets from track sum information
  // Also adds JVF corrected to be insensitive to NPV
  int modify(xAOD::JetContainer& jetCont) const override;

  // Local method to return the primary and pileup track pT sums
  // this method also allows the standard jvf to be calculated
  const xAOD::Vertex* associatedVertex(const xAOD::TrackParticle* track) const;

private:  // data

  std::vector<float> 
  getJetVertexTrackSums(const xAOD::Vertex*,
                        const std::vector<const xAOD::TrackParticle*>&) const;

  // Local method to count the number of pileup tracks in the event
  int getPileupTrackCount(const xAOD::Vertex*,const xAOD::TrackParticleContainer*&) const; 

  // Configurable parameters
  float m_z0cutHighEta;
  float m_z0cutLowEta;
  float m_z0etaDiv;

protected:
  bool m_isTrigger;  
};


#endif

