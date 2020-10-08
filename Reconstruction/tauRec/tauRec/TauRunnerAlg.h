/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAURUNNERALG_H
#define TAUREC_TAURUNNERALG_H

#include "tauRecTools/ITauToolBase.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODParticleEvent/ParticleAuxContainer.h"

#include "GaudiKernel/ToolHandle.h"

/**
 * @brief       Main class for tau candidate processing.
 */


class TauRunnerAlg: public AthAlgorithm
{
    public:
        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauRunnerAlg( const std::string &name, ISvcLocator *pSvcLocator );
        ~TauRunnerAlg();

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode initialize() override;
        virtual StatusCode execute() override;
        virtual StatusCode finalize() override;

 private:
       
	ToolHandleArray<ITauToolBase>  m_tools{this, "Tools", {}, "Tools building taus"};

	SG::ReadHandleKey<xAOD::TauJetContainer> m_tauInputContainer{this,"Key_tauInputContainer","tmp_TauJets","input temp tau key"};
	SG::ReadHandleKey<xAOD::CaloClusterContainer> m_pi0ClusterInputContainer{this,"Key_Pi0ClusterInputContainer", "TauInitialPi0Clusters", "input pi0 cluster"};

	SG::WriteHandleKey<xAOD::TauJetContainer> m_tauOutputContainer{this,"Key_tauOutputContainer","TauJets","output tau data key"};
	SG::WriteHandleKey<xAOD::PFOContainer> m_neutralPFOOutputContainer{this,"Key_neutralPFOOutputContainer", "TauNeutralParticleFlowObjects", "tau neutral pfo out key"};
	SG::WriteHandleKey<xAOD::CaloClusterContainer> m_pi0ClusterOutputContainer{this,"Key_pi0ClusterOutputContainer", "TauPi0Clusters", "tau pi0cluster output"};
	SG::WriteHandleKey<xAOD::PFOContainer> m_hadronicPFOOutputContainer{this,"Key_hadronicPFOOutputContainer", "TauHadronicParticleFlowObjects", "tau hadronic pfo out key"};
	SG::WriteHandleKey<xAOD::VertexContainer> m_vertexOutputContainer{this,"Key_vertexOutputContainer", "TauSecondaryVertices", "input vertex container key"};
	SG::WriteHandleKey<xAOD::PFOContainer> m_chargedPFOOutputContainer{this,"Key_chargedPFOOutputContainer", "TauChargedParticleFlowObjects", "tau charged pfo out key"};
	SG::WriteHandleKey<xAOD::ParticleContainer> m_pi0Container{this,"Key_pi0Container", "TauFinalPi0s", "tau final pi0s output"};
	
};

#endif // TAUREC_TAURUNNERALG_H
