/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTruthClassifierFallback.h
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_MuonTruthClassifierFallback_H
#define DERIVATIONFRAMEWORK_MuonTruthClassifierFallback_H

#include <string>
#include <vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthPileupEventContainer.h"

namespace DerivationFramework {
    class MuonTruthClassifierFallback : public AthAlgTool, public IAugmentationTool {
    public:
        /** Constructor with parameters */
        MuonTruthClassifierFallback(const std::string& t, const std::string& n, const IInterface* p);

        /** Destructor */
        ~MuonTruthClassifierFallback() = default;

        // Athena algtool's Hooks
        StatusCode initialize() override;

        virtual StatusCode addBranches() const override;

    private:
        SG::ReadHandleKey<xAOD::IParticleContainer> m_containerKey{this, "ContainerKey", "", "Key of the container to be decorated"};
        SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthSGKey{this, "TruthEvents", "Key of the truth event container"};
        SG::ReadHandleKey<xAOD::TruthPileupEventContainer> m_truthPileupSGKey{this, "TruthPileupContainerKey", "TruthPileupEvents",
                                                                              "Key of the pile-up event container"};
        SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthMuonSGKey{this, "TruthMuonContainerKey", "MuonTruthParticles", ""};

        /// Key properties are overwirrten during initialize
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_Truth_dR_Key{this, "dRDecoration", ""};
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_Truth_type_Key{this, "typeDecoration", ""};
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_Truth_origin_Key{this, "originDecoration", ""};
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_Truth_PU_dR_Key{this, "dRDecorationPU", ""};
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_Truth_PU_type_Key{this, "typeDecorationPU", ""};
        SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_Truth_PU_origin_Key{this, "originDecorationPU", ""};

        Gaudi::Property<float> m_minPt{this, "MinPt", 2500};

        ToolHandle<IMCTruthClassifier> m_mcTruthClassifier{this, "MCTruthClassifierTool", "", "Handle of the MC truth classifier"};
    };
}  // namespace DerivationFramework
#endif  //
