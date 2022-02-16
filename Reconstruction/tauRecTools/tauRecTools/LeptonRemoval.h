/*
	Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_LeptonRemoval_H
#define TAURECTOOLS_LeptonRemoval_H

#include "tauRecTools/TauRecToolBase.h"
#include "xAODTau/TauJet.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"

namespace tauRecTools
{
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
			bool m_do_muon_trk_rm;
			bool m_do_elec_trk_rm;
			bool m_do_muon_cls_rm;
			bool m_do_elec_cls_rm;
			std::string m_str_min_elec_id_wp;   
			std::string m_str_min_muon_id_wp;   
			std::string m_str_elec_id_wp_prefix;
			const std::map<std::string, uint> m_map_muon_id_wp = {{"Tight", 0}, {"Medium", 1}, {"Loose", 2}, {"VeryLoose",3}};
			SG::ReadHandleKey<xAOD::MuonContainer>     m_muon_input_container{this, "Key_MuonInputContainer", "Muons", "input xAOD muons"};
			SG::ReadHandleKey<xAOD::ElectronContainer> m_elec_input_container{this, "Key_ElecInputContainer", "Electrons", "input xAOD electrons"};
			template <typename T1, typename T2> bool trackMatch(T1 trk1, T2 trk2) const;
			template <typename T1, typename T2> bool clusterMatch(T1 cls1, T2 cls2) const;
	};
}

#endif// TAURECTOOLS_LeptonRemoval_H
