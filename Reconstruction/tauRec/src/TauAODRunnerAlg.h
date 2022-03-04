/*
	Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUAODRUNNERALG_H
#define TAUREC_TAUAODRUNNERALG_H

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
#include "xAODTau/TauTrackContainer.h"
#include "GaudiKernel/ToolHandle.h"

/**
 * 	@brief	The implementation of the TauAODRunnerAlg, which is meant to run at AOD level.
 *	This algorithm read in the AOD tauJets and TauTracks containers, as well as other tau 
 *	related containers, and make deep copies. The tools scheduled for this algorithm were 
 *	divided into two categories, modification tools and official tools after the 
 *	modification. The algorithm do not proceed to the official tools if the tau object was 
 *	not modified by the modification tools. The example python scheduling scripts can be 
 *	found in the TauRec package.
 */

class TauAODRunnerAlg: public AthAlgorithm {
	public:
		TauAODRunnerAlg(const std::string &name, ISvcLocator *);
		~TauAODRunnerAlg(){};
		virtual StatusCode initialize() override;
		virtual StatusCode execute() override;
	private:
		//Tool handle array
		ToolHandleArray<ITauToolBase>  m_modificationTools{this, "modificationTools", {}, "Tools for modifying the taus"};
		ToolHandleArray<ITauToolBase>  m_officialTools	  {this, "officialTools",     {}, "Official Reconstruction tools for taus after the modifications"};
		//Read and write keys
		SG::ReadHandleKey<xAOD::TauJetContainer>        m_tauContainer              {this,"Key_tauContainer",                   "TauJets",                      "input tau key"};
		SG::ReadHandleKey<xAOD::CaloClusterContainer>   m_pi0ClusterInputContainer  {this,"Key_Pi0ClusterInputContainer",       "TauPi0Clusters",               "input pi0 cluster"};
		SG::ReadHandleKey<xAOD::TauTrackContainer>      m_tauTrackInputContainer    {this,"Key_TauTrackInputContainer",         "TauTracks",                    "input tau track cluster"};
		SG::WriteHandleKey<xAOD::TauJetContainer>       m_tauOutContainer           {this, "Key_tauOutputContainer",            "TauJets_AODReco",              "output tau key"};
		SG::WriteHandleKey<xAOD::ParticleContainer>     m_pi0Container              {this, "Key_pi0OutputContainer",            "TauFinalPi0s_AODReco",         "output tau final pi0 key"};
		SG::WriteHandleKey<xAOD::PFOContainer>          m_neutralPFOOutputContainer {this, "Key_neutralPFOOutputContainer",     "TauNeutralPFOs_AODReco",       "output tau neutral pfo key"};
		SG::WriteHandleKey<xAOD::PFOContainer>          m_chargedPFOOutputContainer {this, "Key_chargedPFOOutputContainer",     "TauChargedPFOs_AODReco",       "output tau charged pfo key"};
		SG::WriteHandleKey<xAOD::PFOContainer>          m_hadronicPFOOutputContainer{this, "Key_hadronicPFOOutputContainer",    "TauHadronicPFOs_AODReco",      "output tau hadronic pfo key"};
		SG::WriteHandleKey<xAOD::TauTrackContainer>     m_tauTrackOutputContainer   {this, "Key_tauTrackOutputContainer",       "TauTracks_AODReco",            "output tau track key"};
		SG::WriteHandleKey<xAOD::VertexContainer>       m_vertexOutputContainer     {this, "Key_vertexOutputContainer",         "TauSecondaryVertices_AODReco", "output vertex container key"};
		//helper
		bool isTauModified(const xAOD::TauJet* newtau);
};

#endif // TAUREC_TAUAODRUNNERALG_H
