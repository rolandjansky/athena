/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetVertexFractionTool.h

#ifndef JETMOMENTTOOLS_JETVERTEXFRACTIONTOOL_H
#define JETMOMENTTOOLS_JETVERTEXFRACTIONTOOL_H

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
///  TrackVertexAssociation - name for the container holding the track-vertex associations
///  TrackSelector - tool to select tracks (none ==> no selection)
///  JVFName - name for the JVF array attribute (default is "JVF")


#include "AsgTools/ToolHandle.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "JetInterface/IJetTrackSelector.h"
#include "JetRec/JetModifierBase.h"
#include "JetEDM/TrackVertexAssociation.h"

#include <vector>
#include <string>

class JetVertexFractionTool : public JetModifierBase {
  ASG_TOOL_CLASS(JetVertexFractionTool,IJetModifier)

public:  // methods

  // Constructor from tool name
  JetVertexFractionTool(const std::string& name);

  // Initialization.
  StatusCode initialize();

  // Inherited methods to modify a jet
  // Calls getJetVertexFraction and puts the result in the jet
  int modifyJet(xAOD::Jet& jet) const;

  // Inherited methods to modify a jet
  // Computes JVF for all jets from track sum information
  // Also adds JVF corrected to be insensitive to NPV
  int modify(xAOD::JetContainer& jetCont) const;

  // Local method to calculate and return the JVF vector
  const std::vector<float> getJetVertexFraction(const xAOD::VertexContainer*,
                                                const std::vector<const xAOD::TrackParticle*>&,
                                                const jet::TrackVertexAssociation*) const;

  // Local method to calculate the JVF for a given vertex
  float getJetVertexFraction(const xAOD::Vertex*,
                             const std::vector<const xAOD::TrackParticle*>&,
                             const jet::TrackVertexAssociation*) const;

  // Local method to calculate the JVF for a given vertex
  float getCorrJetVertexFraction(const xAOD::Vertex*,
				 const std::vector<const xAOD::TrackParticle*>&,
				 const xAOD::TrackParticleContainer*&,
				 const jet::TrackVertexAssociation*) const;

  // Local method to determine the highest JVF vertex and get an ElementLink to it
  ElementLink<xAOD::VertexContainer> getMaxJetVertexFraction(const xAOD::VertexContainer*,
                                                                   const std::vector<float>&) const;


private:  // data

  // Configurable parameters
  std::string m_verticesName;
  std::string m_assocTracksName;
  std::string m_tvaName;
  std::string m_tracksName;
  std::string m_sumPtTrkName;
  ToolHandle<IJetTrackSelector> m_htsel;
  std::string m_jvfname;
  float m_kcorrJVF;
  float m_PUtrkptcut;

private:  // methods

  std::vector<float> getEmptyJetVertexFraction(const xAOD::VertexContainer*) const;

  // Local method to count the number of pileup tracks in the event
  int getPileupTrackCount(const xAOD::Vertex*, 
  const xAOD::TrackParticleContainer*&, 
  const jet::TrackVertexAssociation*) const; 

  // Local method to return the HS vertex - that of type PriVtx
  const xAOD::Vertex* findHSVertex(const xAOD::VertexContainer*&) const;

  // Local method to return the primary and pileup track pT sums
  // this method also allows the standard jvf to be calculated
  std::pair<float,float>
  getJetVertexTrackSums(const xAOD::Vertex*,
                        const std::vector<const xAOD::TrackParticle*>&, 
                        const jet::TrackVertexAssociation*) const;

protected:
  bool m_isTrigger;  
};


#endif

