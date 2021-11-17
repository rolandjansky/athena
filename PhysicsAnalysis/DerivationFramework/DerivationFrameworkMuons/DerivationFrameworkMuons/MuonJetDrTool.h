/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonJetDrTool.h
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_MuonJetDrTool_H
#define DERIVATIONFRAMEWORK_MuonJetDrTool_H

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"

namespace DerivationFramework {
    class MuonJetDrTool : public AthAlgTool, public IAugmentationTool {
    public:
        /** Constructor with parameters */
        MuonJetDrTool(const std::string& t, const std::string& n, const IInterface* p);

        /** Destructor */
        ~MuonJetDrTool() = default;

        virtual StatusCode addBranches() const override;
        virtual StatusCode initialize() override;

    private:
        SG::ReadHandleKey<xAOD::MuonContainer> m_muonSGKey{this, "ContainerKey", "Muons"};
        SG::ReadHandleKey<xAOD::JetContainer> m_jetSGKey{this, "JetContainerKey", "AntiKt4EMTopoJets"};
        SG::WriteDecorHandleKey<xAOD::MuonContainer> m_jetDR_SGKey{this, "dRDecoration", "DFCommonJetDr"};

        Gaudi::Property<float> m_jetMinPt{this, "JetMinPt", 20.e3, "Minimal pt cut of the jets to be considered"};
    };
}  // namespace DerivationFramework
#endif  //
