/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRECTOOLS_CHARGEDHADRONSUBTRACTIONTOOL_H
#define JETRECTOOLS_CHARGEDHADRONSUBTRACTIONTOOL_H

////////////////////////////////////////////
/// \class ChargedHadronSubtractionTool
///
/// Removes charged PFO not associated to the PV
///
/// \author John Stupak and Jennifer Roloff
//////////////////////////////////////////////////

#include <string>
#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "xAODTracking/VertexContainer.h" 
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/FlowElementContainer.h"

#include "AsgDataHandles/ReadHandleKey.h"


class ChargedHadronSubtractionTool : public JetConstituentModifierBase{
  ASG_TOOL_CLASS(ChargedHadronSubtractionTool, IJetConstituentModifier)

  public:
  
  ChargedHadronSubtractionTool(const std::string& name);

  // Check that the configuration is reasonable
  StatusCode initialize();

  private:
  // Implement the correction
  StatusCode process_impl(xAOD::IParticleContainer* cont) const; 
  // Type-specific operation
  StatusCode matchToPrimaryVertex(xAOD::PFOContainer& cont) const;
  StatusCode matchToPrimaryVertex(xAOD::FlowElementContainer& cont) const;

  const xAOD::Vertex* getPrimaryVertex() const;


  // properties -----------------
  
  Gaudi::Property<bool> m_useTrackToVertexTool = {this, "UseTrackToVertexTool", false, "True if we will use the track to vertex tool"};
  Gaudi::Property<bool> m_ignoreVertex = {this, "IgnoreVertex", false, "Dummy option for cosmics - accept everything"};
  Gaudi::Property<float> m_z0sinThetaCutValue = {this, "Z0sinThetaCutValue", 2.0, "True if we will use the track to vertex tool"};
	
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key = {this, "VertexContainerKey", "PrimaryVertices", "key for the primary vertex container"};
  SG::ReadHandleKey<jet::TrackVertexAssociation> m_trkVtxAssoc_key = {this, "TrackVertexAssociation", "JetTrackVtxAssoc", "key for the TrackVertexAssociation object"};
  


};

#endif
