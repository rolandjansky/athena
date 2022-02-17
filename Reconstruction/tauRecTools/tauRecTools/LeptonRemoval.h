/*
	Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_LEPTONREMOVAL_H
#define TAURECTOOLS_LEPTONREMOVAL_H

#include "tauRecTools/TauRecToolBase.h"
#include "xAODTau/TauJet.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "AsgDataHandles/ReadHandleKey.h"

class LeptonRemoval : public TauRecToolBase 
{
	public:

		ASG_TOOL_CLASS2( LeptonRemoval, TauRecToolBase, ITauToolBase )

		LeptonRemoval(const std::string& type);
		~LeptonRemoval();

		virtual StatusCode initialize() override;
		virtual StatusCode finalize() override;
		virtual StatusCode execute(xAOD::TauJet&) const override;
		virtual StatusCode eventInitialize() override;
		virtual StatusCode eventFinalize() override { return StatusCode::SUCCESS; }

	private:
		bool 		m_do_muon_trk_rm 		= false;
		bool 		m_do_elec_trk_rm 		= false;
		bool 		m_do_muon_cls_rm 		= false;
		bool 		m_do_elec_cls_rm 		= false;
		double 		m_lep_removal_cone_size = 0.6;
		std::string m_str_min_elec_id_wp 	= "Medium";   
		std::string m_str_min_muon_id_wp 	= "Medium";   
		std::string m_str_elec_id_wp_prefix = "DFCommonElectronsLH";
		
		const std::map<std::string, uint> m_map_muon_id_wp = {{"Tight", 0}, {"Medium", 1}, {"Loose", 2}, {"VeryLoose",3}};
		SG::ReadHandleKey<xAOD::MuonContainer>     m_muon_input_container{this, "Key_MuonInputContainer", "Muons", "input xAOD muons"};
		SG::ReadHandleKey<xAOD::ElectronContainer> m_elec_input_container{this, "Key_ElecInputContainer", "Electrons", "input xAOD electrons"};

		//helpers
		std::vector<const xAOD::CaloCluster*> 	getOrignalTopoClusters (const xAOD::CaloCluster		*cluster) const;
		const xAOD::TrackParticle* 				getOrignalTrackParticle(const xAOD::TrackParticle 	*trk	) const;
};



#endif// TAURECTOOLS_LEPTONREMOVAL_H
