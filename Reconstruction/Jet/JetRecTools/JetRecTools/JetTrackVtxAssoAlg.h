// this is a -*- C++ -*- file
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////
/// \class JetTrackVtxAssoAlg
/// \brief creates a jet::TrackVertexAssociation object
///
/// Creates a jet::TrackVertexAssociation map.
/// The tool expects a ITrackVertexAssociationTool which it uses to create the map
/// from a  TrackParticleContainer and a VertexContainer. The map is recorded in the evtStore.
///
/// Properties :
///  - "TrackParticleContainer" : name of TrackParticleContainer
///  - "VertexContainer" : name of VertexContainer
///  - "TrackVertexAssociation" : name of the output jet::TrackVertexAssociation
///  - "TrackVertexAssoTool" : A ITrackVertexAssociationTool instance
///
///  If the TrackVertexAssoTool is left blank, the old custom method is used to build the code
/// using cuts defined by these properties 
///  - "MaxTransverseDistance",
///  - "MaxLongitudinalDistance"
///  - "MaxZ0SinTheta"
/// This old part of the code may disappear in the future.
////////////////////////////////////////////////////////////////////

#ifndef JetTrackVtxAssoAlg_H
#define JetTrackVtxAssoAlg_H

#include "AnaAlgorithm/AnaReentrantAlgorithm.h"
#include "AsgTools/ToolHandle.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/WriteHandleKey.h"

#include "JetEDM/TrackVertexAssociation.h"

#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"


#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h" 


class JetTrackVtxAssoAlg : public EL::AnaReentrantAlgorithm { 

public:

  JetTrackVtxAssoAlg(const std::string & n, ISvcLocator* l) : EL::AnaReentrantAlgorithm(n,l) {}

  /// Athena algorithm's Hooks
  StatusCode  initialize() override;
  StatusCode  execute(const EventContext& ctx) const override;

private:

  std::unique_ptr<jet::TrackVertexAssociation>
  buildTrackVertexAssociation_withTool(const xAOD::TrackParticleContainer*, 
                                       const xAOD::VertexContainer*) const;
  // Constuct a TrackVertexAssociation using the older custom method
  // jet::TrackVertexAssociation* 
  std::unique_ptr<jet::TrackVertexAssociation>
  buildTrackVertexAssociation_custom(const xAOD::TrackParticleContainer*, 
                                     const xAOD::VertexContainer*) const;

  std::unique_ptr<jet::TrackVertexAssociation>
  makeTrackVertexAssociation(const xAOD::TrackParticleContainer*, 
                             const xAOD::VertexContainer*,
                             bool useCustom) const;

  
  ToolHandle<CP::ITrackVertexAssociationTool> m_tvaTool {this, "TrackVertexAssoTool", "", "CP::ITrackVertexAssociationTool instance" };

  // old Configurable parameters

  Gaudi::Property<float> m_transDistMax= {this, "MaxTransverseDistance",10e6, ""};
  Gaudi::Property<float> m_longDistMax = {this , "MaxLongitudinalDistance",10e6 , ""};
  Gaudi::Property<float> m_maxZ0SinTheta = {this, "MaxZ0SinTheta", 10e6, ""};

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackContainer_key = {this, "TrackParticleContainer", "" , "" };
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key = {this, "VertexContainer", "" , "" };
  SG::WriteHandleKey<jet::TrackVertexAssociation> m_tva_key = {this, "TrackVertexAssociation", "" , "Output name of jet::TrackVertexAssociation" };
  
  
};

#endif
