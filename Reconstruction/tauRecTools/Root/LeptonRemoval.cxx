/*
	Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/LeptonRemoval.h"

using namespace tauRecTools;

//________________________________________
LeptonRemoval::LeptonRemoval(const std::string& name):
	TauRecToolBase(name)
{
    declareProperty("doMuonTrkRm",      m_do_muon_trk_rm        = 0);
    declareProperty("doElecTrkRm",      m_do_elec_trk_rm        = 0);
    declareProperty("doMuonClsRm",      m_do_muon_cls_rm        = 0);
    declareProperty("doElecClsRm",      m_do_elec_cls_rm        = 0);
	declareProperty("elecIDWP",         m_str_min_elec_id_wp    = "Medium");
    declareProperty("muonIDWP",         m_str_min_muon_id_wp    = "Medium");
    declareProperty("eleIDWPPrefix",    m_str_elec_id_wp_prefix = "DFCommonElectronsLH");
}

//________________________________________
LeptonRemoval::~LeptonRemoval(){}

//________________________________________
StatusCode LeptonRemoval::initialize(){
	ATH_CHECK(m_elec_input_container.initialize());
    ATH_CHECK(m_muon_input_container.initialize());
	return StatusCode::SUCCESS;
}

//________________________________________
StatusCode LeptonRemoval::finalize(){
	return StatusCode::SUCCESS;
}

StatusCode LeptonRemoval::eventInitialize(){
	ATH_MSG_VERBOSE( "RUNNING:" << this->name());
	return StatusCode::SUCCESS;
}
//________________________________________
StatusCode LeptonRemoval::execute(xAOD::TauJet& tau) const {
    const auto elec_wp_str = m_str_elec_id_wp_prefix + m_str_min_elec_id_wp;
    const auto muon_wp_ui  = m_map_muon_id_wp.at(m_str_min_muon_id_wp);
	// Read in elec and muon container
	SG::ReadHandle<xAOD::ElectronContainer> elec_input_handle(m_elec_input_container);
    SG::ReadHandle<xAOD::MuonContainer> muon_input_handle(m_muon_input_container);
	if (bool fail_elec = !elec_input_handle.isValid(), fail_muon = !muon_input_handle.isValid(); fail_elec || fail_muon)
	{
		ATH_MSG_ERROR(
                (fail_elec ? "Could not retrieve Electron container with key " + elec_input_handle.key() : "") + 
                (fail_muon ? "\tCould not retrieve Muon container with key " + muon_input_handle.key() : "")
        );
        ATH_MSG_ERROR("Could not retrieve HiveDataObj with key " << muon_input_handle.key());
		return StatusCode::FAILURE;
	}
	auto elec_container = elec_input_handle.cptr();
	auto muon_container = muon_input_handle.cptr();

    std::vector<const xAOD::TrackParticle*> elec_tracks;
    std::vector<const xAOD::CaloCluster*>   elec_clusters;
    std::vector<const xAOD::TrackParticle*> muon_tracks;
    std::vector<const xAOD::CaloCluster*>   muon_clusters;
    auto tau_p4 = tau.p4();
    if (m_do_elec_cls_rm || m_do_elec_trk_rm)
    {
        //get elec tracks and clusters
        std::for_each(elec_container->cbegin(), elec_container->cend(),
            [&](const xAOD::Electron* elec) -> void
            {
                if(tau_p4.DeltaR(elec->p4()) < 0.6 && elec->passSelection(elec_wp_str))
                {
                    if (m_do_elec_trk_rm)
                    {
                        auto elec_ID_tracks_links = elec->trackParticleLinks(); 
                        for (auto elec_ID_track_link : elec_ID_tracks_links)
                        {
                            if (elec_ID_track_link.isValid()) elec_tracks.push_back(std::move(*elec_ID_track_link));
                        }
                    }
                    if (m_do_elec_cls_rm)
                    {
                        auto elec_cluster_links = elec->caloClusterLinks(); 
                        for (auto elec_cluster_link : elec_cluster_links)
                        {
                            if (elec_cluster_link.isValid()) elec_clusters.push_back(std::move(*elec_cluster_link));
                        }
                    }
                }
            }
        );
    }
	
    //get Muon tracks and clusters
    if (m_do_muon_cls_rm || m_do_muon_trk_rm)
    {
        std::for_each(muon_container->cbegin(), muon_container->cend(),
            [&](const xAOD::Muon* muon) -> void
            {
                if(tau_p4.DeltaR(muon->p4()) < 0.6 && muon->quality() <= muon_wp_ui)
                {
                    if (m_do_muon_trk_rm)
                    {
                        if(auto muon_ID_tracks_link = muon->inDetTrackParticleLink();  muon_ID_tracks_link.isValid())
                            muon_tracks.push_back(std::move(*muon_ID_tracks_link));
                    }
                    if (m_do_muon_cls_rm)
                    {
                        if(auto muon_cluster_link = muon->clusterLink();  muon_cluster_link.isValid())
                        {
                            auto muon_cluster = std::move(*muon_cluster_link);
                            auto muon_e = muon->e();
                            auto loss_e = muon->floatParameter(xAOD::Muon::EnergyLoss);
                            auto cls_e = muon_cluster->e();
                            auto loss_diff = ((cls_e - loss_e) / (cls_e + loss_e));
                            if (muon_e > cls_e && loss_diff < 0.1 && loss_diff > -0.3) 
                                muon_clusters.push_back(std::move(muon_cluster));
                        }
                    }
                }
            }
        );
    }

	if(elec_tracks.empty() && elec_clusters.empty() && muon_tracks.empty() && muon_clusters.empty()) return StatusCode::SUCCESS;

	// get tau track links 
	auto tau_track_links = tau.allTauTrackLinksNonConst();
    auto tau_cluster_links = tau.clusterLinks();
	tau.clearTauTrackLinks();
    tau.clearClusterLinks();
    tau_track_links.erase(
        std::remove_if(tau_track_links.begin(), tau_track_links.end(),
            [&](ElementLink<xAOD::TauTrackContainer> tau_trk_link) -> bool
            {
                bool match = false;
                if(tau_trk_link.isValid())
                {
                    auto tau_trk = (*tau_trk_link)->track();
                    if (m_do_muon_trk_rm)
                    {
                        if(auto where = std::find_if(muon_tracks.cbegin(), muon_tracks.cend(), 
                            [&](auto mu_trk){ return trackMatch(mu_trk, tau_trk); }); 
                            where != muon_tracks.cend()) 
                        {
                            ATH_MSG_DEBUG( "muon_track with pt " << tau_trk->pt()/1000 << " GeV removed");
                            match = true;
                        }
                    }
                    if (m_do_elec_trk_rm)
                    {
                        if(auto where = std::find_if(elec_tracks.cbegin(), elec_tracks.cend(), 
                            [&](auto elec_trk){ return trackMatch(elec_trk, tau_trk); }); 
                            where != elec_tracks.cend()) 
                        {
                            ATH_MSG_DEBUG( "elec_track with pt " << tau_trk->pt()/1000 << " GeV removed");
                            match = true;
                        }
                    }
                }
                return match;
            }
        ), 
        tau_track_links.end()
    );
    tau_cluster_links.erase(
        std::remove_if(tau_cluster_links.begin(), tau_cluster_links.end(),
            [&](auto tau_cls_link) -> bool
            {
                bool match = false;
                if(tau_cls_link.isValid())
                {
                    auto tau_cls = (*tau_cls_link);
                    if (m_do_muon_cls_rm)
                    {
                        if(auto where = std::find_if(muon_clusters.cbegin(), muon_clusters.cend(), 
                            [&](auto mu_cls){ return clusterMatch(mu_cls, tau_cls); }); 
                            where != muon_clusters.cend()) 
                        {
                            ATH_MSG_DEBUG( "muon_cluster with pt " << tau_cls->pt()/1000 << " GeV removed");
                            match = true;
                        }
                    }
                    if (m_do_elec_cls_rm)
                    {
                        if(auto where = std::find_if(elec_clusters.cbegin(), elec_clusters.cend(), 
                            [&](auto elec_cls){ return clusterMatch(elec_cls, tau_cls); }); 
                            where != elec_clusters.cend()) 
                        {
                            ATH_MSG_DEBUG( "elec_clusters with pt " << tau_cls->pt()/1000 << " GeV removed");
                            match = true;
                        }
                    }
                }
                return match;
            }
        ), 
        tau_cluster_links.end()
    );
    tau.setClusterLinks(tau_cluster_links);
	tau.setAllTauTrackLinks(tau_track_links);
	return StatusCode::SUCCESS;
}

//Helpers
template <typename T1, typename T2>
bool LeptonRemoval::trackMatch(T1 trk1, T2 trk2) const
{
    auto diff_d0     = std::abs(trk1->d0()      - trk2->d0());
    auto diff_z0     = std::abs(trk1->z0()      - trk2->z0());
    auto diff_phi0   = std::abs(trk1->phi0()    - trk2->phi0());
    auto diff_theta  = std::abs(trk1->theta()   - trk2->theta());
    auto diff_qOverP = std::abs(trk1->qOverP()  - trk2->qOverP());
    return diff_d0 + diff_z0 + diff_phi0 + diff_theta + diff_qOverP < 0.1;
}

template <typename T1, typename T2>
bool LeptonRemoval::clusterMatch(T1 cls1, T2 cls2) const
{
    auto ener_diff = std::abs((cls1->e()   - cls2->e())   / (cls1->e()   + cls2->e()));
    auto phi_diff  = std::abs((cls1->phi() - cls2->phi()) / (cls1->phi() + cls2->phi()));
    auto eta_diff  = std::abs((cls1->eta() - cls2->eta()) / (cls1->eta() + cls2->eta()));
    return ener_diff < 0.1 && phi_diff < 0.1 && eta_diff < 0.1;
}
