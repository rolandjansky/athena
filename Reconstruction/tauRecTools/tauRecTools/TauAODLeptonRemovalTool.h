/*
	Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 *	@brief the tool meant to run at AOD level, to remove the lepton found inside the TauJet.
 */

#ifndef TAURECTOOLS_TAUAODLEPTONREMOVALTOOL_H
#define TAURECTOOLS_TAUAODLEPTONREMOVALTOOL_H

#include "tauRecTools/TauRecToolBase.h"
#include "xAODTau/TauJet.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/ReadHandleKey.h"

class TauAODLeptonRemovalTool : public TauRecToolBase 
{
	public:
		ASG_TOOL_CLASS2( TauAODLeptonRemovalTool, TauRecToolBase, ITauToolBase )
		TauAODLeptonRemovalTool(const std::string& type);
		~TauAODLeptonRemovalTool() {};
		virtual StatusCode initialize() override;
		virtual StatusCode execute(xAOD::TauJet&) const override;
	private:
		bool 						m_doMuonTrkRm 		= false;
		bool 						m_doElecTrkRm 		= false;
		bool 						m_doMuonClsRm 		= false;
		bool 						m_doElecClsRm 		= false;
		std::string 					m_strMinElecIdWp 	= "Medium";   
		std::string 					m_strMinMuonIdWp 	= "Medium";   
		std::string 					m_strElecIdWpPrefix 	= "DFCommonElectronsLH";
		double 						m_lepRemovalConeSize 	= 0.6;
		const std::map<std::string, uint> 		m_mapMuonIdWp 		= {{"Tight", 0}, {"Medium", 1}, {"Loose", 2}, {"VeryLoose",3}};
		std::string 					m_elecWpStr;
		uint 						m_muonWpUi;
		SG::ReadHandleKey<xAOD::MuonContainer>     	m_muonInputContainer{this, "Key_MuonInputContainer", "Muons",	  "input xAOD muons"};
		SG::ReadHandleKey<xAOD::ElectronContainer> 	m_elecInputContainer{this, "Key_ElecInputContainer", "Electrons", "input xAOD electrons"};
		//helpers
		std::vector<const xAOD::CaloCluster*> 						getOrignalTopoClusters (const xAOD::CaloCluster	  *cluster) const;
		const xAOD::TrackParticle* 							getOrignalTrackParticle(const xAOD::TrackParticle *trk	)   const;
		std::vector<std::pair<const xAOD::TrackParticle*, const xAOD::Electron*>> 	getElecAndTrk(const xAOD::TauJet& tau,	const xAOD::ElectronContainer& elec_cont) const;
		std::vector<std::pair<const xAOD::CaloCluster*,   const xAOD::Electron*>> 	getElecAndCls(const xAOD::TauJet& tau,	const xAOD::ElectronContainer& elec_cont) const;
		std::vector<std::pair<const xAOD::TrackParticle*,     const xAOD::Muon*>> 	getMuonAndTrk(const xAOD::TauJet& tau,	const xAOD::MuonContainer& muon_cont) const;
		std::vector<std::pair<const xAOD::CaloCluster*,       const xAOD::Muon*>> 	getMuonAndCls(const xAOD::TauJet& tau,	const xAOD::MuonContainer& muon_cont) const;
		template<typename Tlep, typename Tlinks> std::vector<Tlep> 			removeTrks(Tlinks& tau_trk_links, 	std::vector<std::pair<const xAOD::TrackParticle*, Tlep>>& removings) const;
		template<typename Tlep, typename Tlinks> std::vector<Tlep> 			removeClss(Tlinks& tau_cls_links, 	std::vector<std::pair<const xAOD::CaloCluster*, Tlep>>& clusters_and_leps) const;
};

#endif// TAURECTOOLS_TAUAODLEPTONREMOVALTOOL_H
