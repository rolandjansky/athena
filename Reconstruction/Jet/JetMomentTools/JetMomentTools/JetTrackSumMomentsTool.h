/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetTrackSumMomentsTool.h 

#ifndef JETMOMENTTOOLS_JETTRACKSUMMOMENTSTOOL_H
#define JETMOMENTTOOLS_JETTRACKSUMMOMENTSTOOL_H


/// James Frost \n
/// January 2015
///
/// Tool to calculate moments based on the vector track sum
///
/// Moments to calculate:
///   TrkSumMass
///   TrkSumPt
/// 
/// applying selection to the ghost-associated track collection


#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetDecorator.h"
#include "JetInterface/IJetTrackSelector.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"

#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h" 
#include "xAODTracking/VertexContainer.h" 

#include <vector>
#include <string>


class JetTrackSumMomentsTool : public asg::AsgTool,
                               virtual public IJetDecorator {
  ASG_TOOL_CLASS(JetTrackSumMomentsTool,IJetDecorator)
    
public:

  // Constructor from tool name
  JetTrackSumMomentsTool(const std::string& name);

  // Initialization.
  virtual StatusCode initialize() override;

  // Inherited methods to modify a jet
  // Calls moment and puts the results in the jet
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

  // Local method to return the vector track sums
  std::pair<float,float>
  getJetTrackSums(const xAOD::Vertex*, 
                        const std::vector<const xAOD::TrackParticle*>&, 
                        const jet::TrackVertexAssociation*) const;

  // Local method to return the HS vertex - that of type PriVtx
  const xAOD::Vertex* findHSVertex(const xAOD::VertexContainer*&) const;

private:

  Gaudi::Property<std::string> m_assocTracksName{this, "AssociatedTracks", "", "SG key for associated tracks container"};
  Gaudi::Property<bool> m_requireTrackPV{this, "RequireTrackPV", true, "Require track to be from the primary vertex?"};
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};
  ToolHandle<IJetTrackSelector> m_htsel{this, "TrackSelector", "", "Track selector"};


  SG::ReadHandleKey< xAOD::VertexContainer> m_vertexContainer_key{this, "VertexContainer", "", "Vertex container key"};
  SG::ReadHandleKey<jet::TrackVertexAssociation> m_tva_key{this, "TrackVertexAssociation", "", "Track vertex association key"};
  
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_trackSumPtKey{this, "SumPtName", "SumPt", "SG key for output track SumPt decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_trackSumMassKey{this, "SumMassName", "SumMass", "SG key for output track SumMass decoration"};
};

#endif
