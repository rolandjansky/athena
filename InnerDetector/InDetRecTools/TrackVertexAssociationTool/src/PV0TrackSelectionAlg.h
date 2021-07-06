/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Author: Bill Balunas
// This algorithm creates a filtered copy of a TrackParticleContainer,
// retaining only tracks which are associated to the primary vertex (as
// defined by the provided TrackVertexAssociationTool). The output container
// is a view container; it simply points back to the original TrackParticles.

#ifndef TRACKVERTEXASSOCIATIONTOOL_PV0TRACKSELECTIONALG_H
#define TRACKVERTEXASSOCIATIONTOOL_PV0TRACKSELECTIONALG_H 1

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthContainers/ConstDataVector.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ShallowCopyDecorDeps.h"

#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

class PV0TrackSelectionAlg
  : public AthReentrantAlgorithm
{ 

  public: 

    PV0TrackSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator );

    StatusCode initialize() override;
    StatusCode execute(const EventContext&) const override;
    StatusCode finalize() override;

  private: 

    PV0TrackSelectionAlg();

    ToolHandle<CP::ITrackVertexAssociationTool> m_TVATool{this, "TVATool", "", "Track-Vertex Association tool"};
    
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inTrackKey{this, "InputTrackContainer", "InDetTrackParticles", "Input track particle container"};
    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey{this, "VertexContainer", "PrimaryVertices", "Input vertex container"};
    
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_outTrackKey{this, "OutputTrackContainer", "PV0InDetTrackParticles", "Output track particle container"};

    SG::ShallowCopyDecorDeps<xAOD::TrackParticleContainer> m_decorDeps { this, "DecorDeps", {}, "List of decorations to propagate through the view container" };

}; 

#endif
