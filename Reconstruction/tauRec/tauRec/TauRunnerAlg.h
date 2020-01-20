/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAURUNNERALG_H
#define TAUREC_TAURUNNERALG_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "tauRecTools/ITauToolBase.h"

#include "tauRecTools/TauEventData.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODParticleEvent/ParticleAuxContainer.h"

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
        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

	// template for deep copy function
	template<class T, class U, class V> StatusCode deepCopy(T*& containerOut, U*& containerStoreOut, const V* dummyContainerType,
								const T*& oldContainer);
 private:
       
	ToolHandleArray<ITauToolBase>  m_tools{this, "TauRunnerTools", {}, "Tools building taus"};

	TauEventData m_data;

	SG::ReadHandleKey<xAOD::TauJetContainer> m_tauInputContainer{this,"Key_tauInputContainer","tmp_TauJets","input temp tau key"};
	SG::ReadHandleKey<xAOD::CaloClusterContainer> m_pi0ClusterInputContainer{this,"Key_Pi0ClusterInputContainer", "TauPi0SubtractedClusters", "input pi0 cluster"};

	SG::WriteHandleKey<xAOD::TauJetContainer> m_tauOutputContainer{this,"Key_tauOutputContainer","TauJets","output tau data key"};	

	SG::WriteHandleKey<xAOD::PFOContainer> m_neutralPFOOutputContainer{this,"Key_neutralPFOOutputContainer", "TauNeutralParticleFlowObjects", "tau neutral pfo out key"};
	SG::WriteHandleKey<xAOD::CaloClusterContainer> m_pi0ClusterOutputContainer{this,"Key_pi0ClusterOutputContainer", "TauPi0Clusters", "tau pi0cluster output"};
	SG::WriteHandleKey<xAOD::PFOContainer> m_hadronicPFOOutputContainer{this,"Key_hadronicPFOOutputContainer", "TauHadronicParticleFlowObjects", "tau hadronic pfo out key"};
	SG::WriteHandleKey<xAOD::VertexContainer> m_vertexOutputContainer{this,"Key_vertexOutputContainer", "TauSecondaryVertices", "input vertex container key"};
	SG::WriteHandleKey<xAOD::PFOContainer> m_chargedPFOOutputContainer{this,"Key_chargedPFOOutputContainer", "TauChargedParticleFlowObjects", "tau charged pfo out key"};
	SG::WriteHandleKey<xAOD::ParticleContainer> m_pi0Container{this,"Key_pi0Container", "finalTauPi0s", "tau final pi0s output"};
	
};

// Function to perform deep copy on container
template<class T, class U, class V> StatusCode TauRunnerAlg::deepCopy(T*& container, U*& containerStore, const V* /*dummyContainerElementType*/,
								      const T*& oldContainer){

  // The new container should be null, check here
  if(container==0 && containerStore==0){
    container = new T();
    containerStore = new U();
    container->setStore(containerStore);
  }
  else{
    ATH_MSG_FATAL("Proviced non-null containters, not initializing please provide null containers: ");
    return StatusCode::FAILURE;
  }
  for( const V* v : *oldContainer ){
    V* newV = new V();
    // Put objects into new container
    container->push_back(newV);
    // Copy across aux store
    *newV = *v;
  }

  return StatusCode::SUCCESS;
}

#endif // TAUREC_TAURUNNERALG_H
