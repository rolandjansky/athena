/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetVertexFractionTool.h

#ifndef JETMOMENTTOOLS_JETVERTEXFRACTIONTOOL_H
#define JETMOMENTTOOLS_JETVERTEXFRACTIONTOOL_H

/// Steven Schramm \n
/// February 2014
///
/// Updated for Run 3 by Bill Balunas
/// March 2020
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
#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "JetInterface/IJetTrackSelector.h"
#include "JetInterface/IJetDecorator.h"
#include "JetEDM/TrackVertexAssociation.h"

#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"

#include <vector>
#include <string>

class JetVertexFractionTool : public asg::AsgTool,
                              virtual public IJetDecorator{
  ASG_TOOL_CLASS(JetVertexFractionTool,IJetDecorator)

public:  // methods

  // Constructor from tool name
  JetVertexFractionTool(const std::string& name);

  // Initialization.
  virtual StatusCode initialize() override;

  // Inherited method to decorate a jet
  // Computes JVF for all jets from track sum information
  // Also adds JVF corrected to be insensitive to NPV
  virtual StatusCode decorate(const xAOD::JetContainer& jetCont) const override;

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
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for input jet container"};
  Gaudi::Property<std::string> m_assocTracksName{this, "AssociatedTracks", "", "Name of associated track container"};
  Gaudi::Property<float> m_kcorrJVF{this, "K_JVFCorrScale", 0.01, "Value of k for JVFCorr calculation"};
  Gaudi::Property<float> m_PUtrkptcut{this, "PUTrkPtCut", 30000., "Pileup track pt cut (MeV)"};

  ToolHandle<IJetTrackSelector> m_htsel{this, "TrackSelector", "", "Track selector tool"};

  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key{this, "VertexContainer", "", "Input vertex container"};
  SG::ReadHandleKey<jet::TrackVertexAssociation> m_tva_key{this, "TrackVertexAssociation", "", "Input track-vertex association"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksCont_key{this, "TrackParticleContainer", "", "Input track container"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_sumPtTrkKey{this, "SumPtTrkName", "SumPtTrkPt500", "SG key for input SumPtTrk decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_jvfKey{this, "JVFName", "JVF", "SG key for output JVF decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_jvfCorrKey{this, "JVFCorrName", "JVFCorr", "SG key for output JVFCorr decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_maxJvfVtxKey{this, "HighestJVFVertexName", "HighestJVFVtx", "SG key for output HighestJVFVertex decoration"};

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
  Gaudi::Property<bool> m_isTrigger{this, "IsTrigger", false, "Is this in the trigger?"};

};


#endif

