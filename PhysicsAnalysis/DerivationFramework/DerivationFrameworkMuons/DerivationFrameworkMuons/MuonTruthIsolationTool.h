/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTruthIsolationTool.h
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_MuonTruthIsolationTool_H
#define DERIVATIONFRAMEWORK_MuonTruthIsolationTool_H

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"

namespace DerivationFramework {
    class MuonTruthIsolationTool : public AthAlgTool, public IAugmentationTool {
    public:
        /** Constructor with parameters */
        MuonTruthIsolationTool(const std::string& t, const std::string& n, const IInterface* p);

        /** Destructor */
        virtual ~MuonTruthIsolationTool() = default;

        // Athena algtool's Hooks
        StatusCode initialize() override;
        virtual StatusCode addBranches() const override;

    private:
        SG::ReadHandleKey<xAOD::IParticleContainer> m_partSGKey{this, "ContainerKey", "",
                                                                "Key of the container to run the truth isolation on "};
        SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthSGKey{this, "TruthContainerKey", "TruthEvents"};

        /// Decor handle keys. Never set them via the JO as they're overwritten
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_topoetcone20_Key{this, "Key_TopoEtCone20", ""};
        /// PtCone20
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_ptcone20_pt500_Key{this, "Key_PtCone20_pt500", ""};
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_ptcone20_Key{this, "Key_PtCone20", ""};
        /// PtVarCone20
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_ptvarcone20_pt500_Key{this, "Key_PtVarCone20_pt500", ""};
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_ptvarcone20_Key{this, "Key_PtVarCone20", ""};
        /// Pt varcone 30
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_ptvarcone30_pt500_Key{this, "Key_PtVarCone30_pt500", ""};
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_ptvarcone30_Key{this, "Key_PtVarCone30", ""};
    };
}  // namespace DerivationFramework
#endif  //
