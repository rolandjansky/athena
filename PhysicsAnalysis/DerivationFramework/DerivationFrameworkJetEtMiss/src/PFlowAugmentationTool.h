////////////////////-*- C++ -*-////////////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PFlowAugmentationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_PFLOWAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_PFLOWAUGMENTATIONTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/WriteDecorHandleKey.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include <xAODPFlow/PFOContainer.h>
#include <PFlowUtils/IWeightPFOTool.h>

namespace DerivationFramework {

  class PFlowAugmentationTool : public AthAlgTool, public IAugmentationTool {
  public: 
    PFlowAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);

    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;

  private:

    float m_z0sinthcut;

    ToolHandle<CP::IWeightPFOTool> m_weightPFOTool;    /// Retrieval tool
    bool m_useChargedWeights; //If true, them we make use of the charged PFO weighting scheme

    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key{this, "VertexContainer", "PrimaryVertices", "Input vertex container"};
    SG::ReadHandleKey<xAOD::PFOContainer> m_pfoContainer_key{this, "JetETMissChargedParticleFlowObjects", "JetETMissChargedParticleFlowObjects", "Input charged PFO"};

    SG::WriteDecorHandleKey<xAOD::PFOContainer> m_PVmatchedKey {this, "PVmatchedKey", "JetETMissChargedParticleFlowObjects.DFCommonPFlow_PVMatched", "Boolean indicating if PFO was matched to PV "};
    SG::WriteDecorHandleKey<xAOD::PFOContainer> m_corrP4_ptKey {this, "m_corrP4_ptKey", "JetETMissChargedParticleFlowObjects.DFCommonPFlow_CaloCorrectedPt", "Decoration for weighted charged PFO pt"};
    SG::WriteDecorHandleKey<xAOD::PFOContainer> m_z0Key {this, "m_z0Key", "JetETMissChargedParticleFlowObjects.DFCommonPFlow_z0", "Decoration for track z0"};
    SG::WriteDecorHandleKey<xAOD::PFOContainer> m_vzKey{this, "m_vzKey","JetETMissChargedParticleFlowObjects.DFCommonPFlow_vz", "Decoration for track vz"};
    SG::WriteDecorHandleKey<xAOD::PFOContainer> m_d0Key{this, "m_d0Key","JetETMissChargedParticleFlowObjects.DFCommonPFlow_d0", "Decoration for track d0"};
    SG::WriteDecorHandleKey<xAOD::PFOContainer> m_thetaKey{this, "m_thetaKey","JetETMissChargedParticleFlowObjects.DFCommonPFlow_theta", "Decoration for track theta"};
    SG::WriteDecorHandleKey<xAOD::PFOContainer> m_envWeightKey{this, "m_envWeightKey","JetETMissChargedParticleFlowObjects.DFCommonPFlow_envWeight", "Decoration for weight for dense environments"};
    
  }; 
}

#endif // DERIVATIONFRAMEWORK_PFLOWAUGMENTATIONTOOL_H
