/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "JetInterface/IJetTrackSelector.h"
#include "JetRec/JetModifierBase.h"
#include "JetEDM/TrackVertexAssociation.h"

#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h" 
#include "xAODTracking/VertexContainer.h" 

#include <vector>
#include <string>


class JetTrackSumMomentsTool : public JetModifierBase {
  ASG_TOOL_CLASS(JetTrackSumMomentsTool,IJetModifier)
    
public:

  // Constructor from tool name
  JetTrackSumMomentsTool(const std::string& name);

  // Initialization.
  StatusCode initialize();

  // Inherited methods to modify a jet
  // Calls moment and puts the results in the jet
  virtual int modifyJet(xAOD::Jet& jet) const;

  // Local method to return the vector track sums
  std::pair<float,float>
  getJetTrackSums(const xAOD::Vertex*, 
                        const std::vector<const xAOD::TrackParticle*>&, 
                        const jet::TrackVertexAssociation*) const;

  // Local method to return the HS vertex - that of type PriVtx
  const xAOD::Vertex* findHSVertex(const xAOD::VertexContainer*&) const;

private:

  std::string m_assocTracksName;
  bool m_requireTrackPV;
  ToolHandle<IJetTrackSelector> m_htsel;


  SG::ReadHandleKey< xAOD::VertexContainer> m_vertexContainer_key;
  SG::ReadHandleKey<jet::TrackVertexAssociation> m_tva_key;
};

#endif
