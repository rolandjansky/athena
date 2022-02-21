/*
	Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/LeptonRemoval.h"

//________________________________________
LeptonRemoval::LeptonRemoval(const std::string& name):
	TauRecToolBase(name)
{
    declareProperty("doMuonTrkRm",          m_do_muon_trk_rm        = false);
    declareProperty("doElecTrkRm",          m_do_elec_trk_rm        = false);
    declareProperty("doMuonClsRm",          m_do_muon_cls_rm        = false);
    declareProperty("doElecClsRm",          m_do_elec_cls_rm        = false);
	declareProperty("elecIDWP",             m_str_min_elec_id_wp    = "Medium");
    declareProperty("muonIDWP",             m_str_min_muon_id_wp    = "Medium");
    declareProperty("eleIDWPPrefix",        m_str_elec_id_wp_prefix = "DFCommonElectronsLH");
    declareProperty("lepRemovalConeSize",   m_lep_removal_cone_size = 0.6);
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
                if(tau_p4.DeltaR(elec->p4()) < m_lep_removal_cone_size && elec->passSelection(elec_wp_str))
                {
                    if (m_do_elec_trk_rm)
                    {
                        auto elec_ID_tracks_links = elec->trackParticleLinks();
                        for (auto elec_ID_tracks_link : elec_ID_tracks_links)
                        {
                            if (elec_ID_tracks_link.isValid())
                            {
                                if(auto orig_ele_trk = getOrignalTrackParticle(*elec_ID_tracks_link); orig_ele_trk)
                                {
                                    elec_tracks.push_back(orig_ele_trk);
                                }
                            }
                        }
                    }
                    if (m_do_elec_cls_rm)
                    {
                        auto elec_cluster_links = elec->caloClusterLinks(); 
                        for (auto elec_cluster_link : elec_cluster_links)
                        {
                            if (elec_cluster_link.isValid()) 
                            {
                                auto orig_elec_clusters = std::move(getOrignalTopoClusters(*elec_cluster_link));
                                elec_clusters.insert(elec_clusters.end(), orig_elec_clusters.begin(), orig_elec_clusters.end());
                            }
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
                if(tau_p4.DeltaR(muon->p4()) < m_lep_removal_cone_size && muon->quality() <= muon_wp_ui)
                {
                    if (m_do_muon_trk_rm)
                    {
                        if(auto muon_ID_tracks_link = muon->inDetTrackParticleLink();  muon_ID_tracks_link.isValid())
                        {
                            muon_tracks.push_back(std::move(*muon_ID_tracks_link));
                        }
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
                            {
                                auto orig_muon_clusters = std::move(getOrignalTopoClusters(muon_cluster));
                                muon_clusters.insert(muon_clusters.end(), orig_muon_clusters.begin(), orig_muon_clusters.end());
                            }
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
                            [&](auto mu_trk){ return tau_trk == mu_trk; }); 
                            where != muon_tracks.cend()) 
                        {
                            ATH_MSG_DEBUG( "muon_track with pt " << tau_trk->pt()/1000 << " GeV removed");
                            match = true;
                        }
                    }
                    if (m_do_elec_trk_rm)
                    {
                        if(auto where = std::find_if(elec_tracks.cbegin(), elec_tracks.cend(), 
                            [&](auto elec_trk){ return tau_trk == elec_trk; }); 
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
                    auto tau_cls = static_cast<const xAOD::CaloCluster*>(*tau_cls_link);
                    if (m_do_muon_cls_rm)
                    {
                        if(auto where = std::find_if(muon_clusters.cbegin(), muon_clusters.cend(), 
                            [&](auto mu_cls){ return tau_cls == mu_cls; }); 
                            where != muon_clusters.cend()) 
                        {
                            ATH_MSG_DEBUG( "muon_cluster with pt " << tau_cls->pt()/1000 << " GeV removed");
                            match = true;
                        }
                    }
                    if (m_do_elec_cls_rm)
                    {
                        if(auto where = std::find_if(elec_clusters.cbegin(), elec_clusters.cend(), 
                            [&](auto elec_cls){ return tau_cls == elec_cls; }); 
                            where != elec_clusters.cend()) 
                        {
                            ATH_MSG_DEBUG( "elec_cluster with pt " << tau_cls->pt()/1000 << " GeV removed");
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

//helpers
std::vector<const xAOD::CaloCluster*> LeptonRemoval::getOrignalTopoClusters(const xAOD::CaloCluster *cluster) const
{
    static const SG::AuxElement::Accessor<std::vector<ElementLink<xAOD::CaloClusterContainer>>> orig("constituentClusterLinks");
    std::vector< const xAOD::CaloCluster* > orig_cls;
    if(orig.isAvailable(*cluster))
    {
        auto links = orig(*cluster); 
        for (auto link : links)
        {
            if (link.dataID() != "CaloCalTopoClusters") 
                ATH_MSG_ERROR("the clusters in the lepton cannot be converted to CaloCalTopoClusters");
            if (link.isValid())
                orig_cls.push_back(*link);
        }
    }
    return orig_cls;
}

const xAOD::TrackParticle* LeptonRemoval::getOrignalTrackParticle(const xAOD::TrackParticle* trk) const
{
    static const SG::AuxElement::Accessor<ElementLink<xAOD::TrackParticleContainer>> orig ("originalTrackParticle");
    const xAOD::TrackParticle* orig_trk = nullptr;
    if(orig.isAvailable(*trk))
    {
        if (auto orig_link = orig(*trk); orig_link.isValid())
        {
            if (orig_link.dataID() != "InDetTrackParticles") 
                ATH_MSG_ERROR("the tracks in the lepton cannot be converted to InDetTrackParticles");
            orig_trk = *orig_link;
        }
    }
    return orig_trk;
}