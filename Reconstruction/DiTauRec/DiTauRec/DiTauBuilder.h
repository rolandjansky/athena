/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_DITAUBUILDER_H
#define DITAUREC_DITAUBUILDER_H 1

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODJet/JetContainer.h"

#include "DiTauToolBase.h"
#include "GaudiKernel/ToolHandle.h"


class DiTauBuilder: public ::AthReentrantAlgorithm { 
	public: 
		DiTauBuilder( const std::string& name, ISvcLocator* pSvcLocator );
		virtual ~DiTauBuilder(); 

		virtual StatusCode  initialize() override;
		virtual StatusCode  execute(const EventContext&) const override;
		virtual StatusCode  finalize() override;

	private: 
                // ditau output container name
                SG::WriteHandleKey<xAOD::DiTauJetContainer> m_diTauContainerName
                { this, "DiTauContainer", "DiTauJets", "" };
                // name for seed jet collection name
                SG::ReadHandleKey<xAOD::JetContainer> m_seedJetName
                { this, "SeedJetName", "AntiKt10LCTopoJets", "" };
		float m_minPt;  // minimal jet seed pt
		float m_maxEta;  // maximal jet seed eta
		float m_Rjet;   // jet radius
		float m_Rsubjet;  // subjet radius
		float m_Rcore;  // core subjet radius
		ToolHandleArray<DiTauToolBase> m_tools;

}; 

#endif //> !DITAUREC_DITAUBUILDER_H
