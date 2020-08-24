/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTCalo_L2CaloEMClustersMonitor.h"

template <typename T> struct clus_kin {
  double et;
  double eta;
  double phi;
  T parent;
};

HLTCalo_L2CaloEMClustersMonitor::HLTCalo_L2CaloEMClustersMonitor( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
  declareProperty("HLTContainer", m_HLT_cont_key = "HLT_FastCaloEMClusters");
  declareProperty("OFFContainer", m_OFF_cont_key = "egammaClusters");
  declareProperty("MonGroupName", m_mongroup_name = "TrigCaloMonitor");

  declareProperty("OFFTypes",  m_OFF_types);
  declareProperty("HLThighET", m_HLT_high_et = 10000.0);
  declareProperty("HLTMinET",  m_HLT_min_et  = -1.0);
  declareProperty("OFFMinET",  m_OFF_min_et  = -1.0);
  declareProperty("MaxDeltaR", m_max_delta_r = 0.04);
}


HLTCalo_L2CaloEMClustersMonitor::~HLTCalo_L2CaloEMClustersMonitor() {}


StatusCode HLTCalo_L2CaloEMClustersMonitor::initialize() {
  ATH_CHECK(m_HLT_cont_key.initialize());
  ATH_CHECK(m_OFF_cont_key.initialize());
  ATH_CHECK( m_bunchCrossingKey.initialize());
  return AthMonitorAlgorithm::initialize();
}


StatusCode HLTCalo_L2CaloEMClustersMonitor::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  // Get HLT cluster collections
  SG::ReadHandle<xAOD::TrigEMClusterContainer> hltCluster_readHandle(m_HLT_cont_key, ctx);
  if (! hltCluster_readHandle.isValid() ) {
	ATH_MSG_ERROR("evtStore() does not contain a cluster Collection with key " << m_HLT_cont_key);
	return StatusCode::FAILURE;
  }

  // Get OFF cluster collections
  SG::ReadHandle<xAOD::CaloClusterContainer> offCluster_readHandle(m_OFF_cont_key, ctx);
  if (! offCluster_readHandle.isValid() ) {
	ATH_MSG_ERROR("evtStore() does not contain a cluster Collection with key " << m_OFF_cont_key);
	return StatusCode::FAILURE;
  }

  // Bunch crossing
  int bcid = ctx.eventID().bunch_crossing_id();
  auto HLT_bc = Monitored::Scalar<int>("HLT_bc",-1);

  SG::ReadCondHandle<BunchCrossingCondData> bcidHdl(m_bunchCrossingKey,ctx);
  if (!bcidHdl.isValid()) {
    ATH_MSG_ERROR( "Unable to retrieve BunchCrossing conditions object" );
    return StatusCode::FAILURE;
  }
  const BunchCrossingCondData* bcData=*bcidHdl;
  HLT_bc = bcData->distanceFromFront(bcid, BunchCrossingCondData::BunchCrossings);



  /////////////////////////////////////
  // Cache expensive et, eta and phi //
  // calculations for the clusters   //
  /////////////////////////////////////

  // prepare HLT clusters
  std::vector<clus_kin<xAOD::TrigEMCluster*> > vec_hlt_clusters;
  for (const auto& hlt_cluster : *hltCluster_readHandle) {
	auto hlt_clus_et = hlt_cluster->et();
	if (hlt_clus_et < m_HLT_min_et) continue;
	vec_hlt_clusters.push_back({hlt_clus_et*0.001, hlt_cluster->eta(), hlt_cluster->phi(), (xAOD::TrigEMCluster*) hlt_cluster});
  }

  // prepare OFF clusters
  std::vector<clus_kin<xAOD::CaloCluster*> > vec_off_clusters;
  for (const auto& off_cluster : *offCluster_readHandle) {
	auto off_clus_et = off_cluster->et();
	if (off_clus_et < m_OFF_min_et) continue;

	bool OFF_type_match = false;

	for (unsigned int n = 0; n < m_OFF_types.size(); ++n) {
		if (off_cluster->clusterSize() == m_OFF_types[n]) { OFF_type_match = true; break; }
	}

	if (!m_OFF_types.empty() && !OFF_type_match) continue;

	vec_off_clusters.push_back({off_clus_et*0.001, off_cluster->eta(), off_cluster->phi(), (xAOD::CaloCluster*) off_cluster});
  }

  //////////////////
  // HLT CLUSTERS //
  //////////////////

  unsigned int n_hlt_clusters = 0;
  unsigned int n_hlt_barrel_high_et_clusters = 0;
  unsigned int n_hlt_clusters_no_match = 0;
  unsigned int n_hlt_clusters_with_match = 0;

  // All HLT clusters
  auto HLT_num = Monitored::Scalar<int>("HLT_num",0);
  auto HLT_et = Monitored::Collection("HLT_et", vec_hlt_clusters, &clus_kin<xAOD::TrigEMCluster*>::et);
  auto HLT_eta = Monitored::Collection("HLT_eta", vec_hlt_clusters, &clus_kin<xAOD::TrigEMCluster*>::eta);
  auto HLT_phi = Monitored::Collection("HLT_phi", vec_hlt_clusters, &clus_kin<xAOD::TrigEMCluster*>::phi);
  auto HLT_size = Monitored::Collection("HLT_size", vec_hlt_clusters, []( const clus_kin<xAOD::TrigEMCluster*>& clus) { return clus.parent->nCells(); } );

  // HLT cut masks
  std::vector<char> vec_hlt_barrel_high_et, vec_hlt_no_off_match, vec_hlt_with_off_match;
  auto HLT_barrel_high_et = Monitored::Collection("HLT_barrel_high_et", vec_hlt_barrel_high_et);
  auto HLT_no_OFF_match = Monitored::Collection("HLT_no_OFF_match", vec_hlt_no_off_match);
  auto HLT_with_OFF_match = Monitored::Collection("HLT_with_OFF_match", vec_hlt_with_off_match);

  auto HLT_barrel_high_et_num = Monitored::Scalar<int>("HLT_barrel_high_et_num",0);
  auto HLT_no_OFF_match_num = Monitored::Scalar<int>("HLT_no_OFF_match_num",0);
  auto HLT_with_OFF_match_num = Monitored::Scalar<int>("HLT_with_OFF_match_num",0);

  // HLT clusters vs. OFF clusters
  std::vector<float> vec_hlt_vs_off_minimum_delta_r, vec_hlt_vs_off_delta_eta, vec_hlt_vs_off_delta_phi, vec_hlt_vs_off_resolution, vec_off_match_et;

  auto HLT_matched_fraction = Monitored::Scalar<float>("HLT_matched_fraction", 0.0);
  auto HLT_vs_OFF_minimum_delta_r = Monitored::Collection("HLT_vs_OFF_minimum_delta_r", vec_hlt_vs_off_minimum_delta_r);
  auto HLT_vs_OFF_minimum_delta_eta = Monitored::Collection("HLT_vs_OFF_minimum_delta_eta", vec_hlt_vs_off_delta_eta);
  auto HLT_vs_OFF_minimum_delta_phi = Monitored::Collection("HLT_vs_OFF_minimum_delta_phi", vec_hlt_vs_off_delta_phi);
  auto HLT_vs_OFF_resolution = Monitored::Collection("HLT_vs_OFF_resolution", vec_hlt_vs_off_resolution);
  auto OFF_match_et = Monitored::Collection("OFF_match_et", vec_off_match_et);

  const clus_kin<xAOD::CaloCluster*> *off_match = nullptr; // For matching

  // Loop over HLT clusters

  for (const auto& hlt_cluster : vec_hlt_clusters) {

	++n_hlt_clusters;

	// high-ET clusters
        if (hlt_cluster.et > (m_HLT_high_et*0.001) && fabs(hlt_cluster.eta) < 2.5) {

                ++n_hlt_barrel_high_et_clusters;

		vec_hlt_barrel_high_et.push_back(1);
        }
	else vec_hlt_barrel_high_et.push_back(0);

	// matching HLT clusters to OFF clusters

        float min_delta_r  = 999999.9;

        for (const auto& off_cluster : vec_off_clusters) {

                float delta_r = calculateDeltaR(m_max_delta_r, hlt_cluster.eta, hlt_cluster.phi, off_cluster.eta, off_cluster.phi);

                if (delta_r < min_delta_r) {

                        min_delta_r = delta_r;
                        off_match = &off_cluster;
                }

        } // End loop over OFF clusters

        vec_hlt_vs_off_minimum_delta_r.push_back(min_delta_r);

        // No OFF match
        if (min_delta_r >= m_max_delta_r) {

                ++n_hlt_clusters_no_match;

		vec_off_match_et.push_back(0.);
		vec_hlt_vs_off_resolution.push_back(0.);
		vec_hlt_vs_off_delta_eta.push_back(0.);
		vec_hlt_vs_off_delta_phi.push_back(0.);

		vec_hlt_no_off_match.push_back(1);
		vec_hlt_with_off_match.push_back(0);
        }

        // With OFF match
        else {

                ++n_hlt_clusters_with_match;

		vec_off_match_et.push_back(off_match->et);
		vec_hlt_vs_off_resolution.push_back(((off_match->et - hlt_cluster.et) / off_match->et) * 100);
		vec_hlt_vs_off_delta_eta.push_back(off_match->eta - hlt_cluster.eta);
		vec_hlt_vs_off_delta_phi.push_back(calculateDeltaPhi(off_match->phi, hlt_cluster.phi));

		vec_hlt_no_off_match.push_back(0);
		vec_hlt_with_off_match.push_back(1);
	}

  } // End loop over HLT clusters

  HLT_num = n_hlt_clusters;
  HLT_barrel_high_et_num = n_hlt_barrel_high_et_clusters;
  HLT_no_OFF_match_num = n_hlt_clusters_no_match;
  HLT_with_OFF_match_num = n_hlt_clusters_with_match;
  HLT_matched_fraction = (float) n_hlt_clusters_with_match / (float) (n_hlt_clusters_with_match + n_hlt_clusters_no_match);

  //////////////////
  // OFF CLUSTERS //
  //////////////////

  unsigned int n_off_clusters = 0;
  unsigned int n_off_clusters_no_match = 0;
  unsigned int n_off_clusters_with_match = 0;

  // OFF cluster
  auto OFF_num = Monitored::Scalar<int>("OFF_num",0);
  auto OFF_et = Monitored::Collection("OFF_et", vec_off_clusters, &clus_kin<xAOD::CaloCluster*>::et);
  auto OFF_eta = Monitored::Collection("OFF_eta", vec_off_clusters, &clus_kin<xAOD::CaloCluster*>::eta);
  auto OFF_phi = Monitored::Collection("OFF_phi", vec_off_clusters, &clus_kin<xAOD::CaloCluster*>::phi);
  auto OFF_type = Monitored::Collection("OFF_type", vec_off_clusters, []( const clus_kin<xAOD::CaloCluster*>& clus) { return clus.parent->clusterSize(); } );

  // cut masks
  std::vector<char> vec_off_no_hlt_match, vec_off_with_hlt_match;
  auto OFF_no_HLT_match = Monitored::Collection("OFF_no_HLT_match", vec_off_no_hlt_match);
  auto OFF_with_HLT_match = Monitored::Collection("OFF_with_HLT_match", vec_off_with_hlt_match);

  auto OFF_no_HLT_match_num = Monitored::Scalar<int>("OFF_no_HLT_match_num",0);
  auto OFF_with_HLT_match_num = Monitored::Scalar<int>("OFF_with_HLT_match_num",0);

  // OFF clusters vs. HLT clusters
  std::vector<float> vec_off_vs_hlt_minimum_delta_r, vec_off_vs_hlt_delta_eta, vec_off_vs_hlt_delta_phi, vec_off_vs_hlt_resolution, vec_hlt_match_et;

  auto OFF_matched_fraction = Monitored::Scalar<float>("OFF_matched_fraction", 0.0);
  auto OFF_vs_HLT_minimum_delta_r = Monitored::Collection("OFF_vs_HLT_minimum_delta_r", vec_off_vs_hlt_minimum_delta_r);
  auto OFF_vs_HLT_minimum_delta_eta = Monitored::Collection("OFF_vs_HLT_minimum_delta_eta", vec_off_vs_hlt_delta_eta);
  auto OFF_vs_HLT_minimum_delta_phi = Monitored::Collection("OFF_vs_HLT_minimum_delta_phi", vec_off_vs_hlt_delta_phi);
  auto OFF_vs_HLT_resolution = Monitored::Collection("OFF_vs_HLT_resolution", vec_off_vs_hlt_resolution);
  auto HLT_match_et = Monitored::Collection("HLT_match_et", vec_hlt_match_et);

  const clus_kin<xAOD::TrigEMCluster*> *hlt_match = nullptr; // For matching

  // Loop over OFF clusters

  for (const auto& off_cluster : vec_off_clusters) {

	++n_off_clusters;

	// matching HLT clusters to OFF clusters

	float min_delta_r  = 999999.9;

	for (const auto& hlt_cluster : vec_hlt_clusters) {

		float delta_r = calculateDeltaR(m_max_delta_r, off_cluster.eta, off_cluster.phi, hlt_cluster.eta, hlt_cluster.phi);

		if (delta_r < min_delta_r) {

			min_delta_r = delta_r;
			hlt_match = &hlt_cluster; // avoid HLT double counts?
		}

	} // End loop over HLT clusters

	vec_off_vs_hlt_minimum_delta_r.push_back(min_delta_r);

	// No HLT match
	if (min_delta_r >= m_max_delta_r) {

		++n_off_clusters_no_match;

		vec_hlt_match_et.push_back(0.);

		vec_off_vs_hlt_resolution.push_back(0.);
		vec_off_vs_hlt_delta_eta.push_back(0.);
		vec_off_vs_hlt_delta_phi.push_back(0.);

		vec_off_no_hlt_match.push_back(1);
		vec_off_with_hlt_match.push_back(0);
	}

	// With HLT match
	else {

		++n_off_clusters_with_match;

		vec_hlt_match_et.push_back(hlt_match->et);

		vec_off_vs_hlt_resolution.push_back(((off_cluster.et - hlt_match->et) / off_cluster.et) * 100);
		vec_off_vs_hlt_delta_eta.push_back(off_cluster.eta - hlt_match->eta);
		vec_off_vs_hlt_delta_phi.push_back(calculateDeltaPhi(off_cluster.phi, hlt_match->phi));

		vec_off_no_hlt_match.push_back(0);
		vec_off_with_hlt_match.push_back(1);
	}

  } // End loop over OFF clusters

  OFF_num = n_off_clusters;
  OFF_no_HLT_match_num = n_off_clusters_no_match;
  OFF_with_HLT_match_num = n_off_clusters_with_match;

  OFF_matched_fraction = (float) n_off_clusters_with_match / (float) (n_off_clusters_with_match + n_off_clusters_no_match);

  // Fill everything
  fill(m_mongroup_name,
        // HLT clusters
        HLT_num, HLT_et, HLT_eta, HLT_phi, HLT_size, HLT_barrel_high_et_num, HLT_bc,

	// HLT cutmasks
	HLT_barrel_high_et, HLT_no_OFF_match, HLT_with_OFF_match,

        // OFF clusters
        OFF_num, OFF_et, OFF_eta, OFF_phi, OFF_type, 

	// OFF cutmasks
	OFF_no_HLT_match, OFF_with_HLT_match,

        // HLT matched to OFF
        HLT_matched_fraction, HLT_no_OFF_match_num, HLT_vs_OFF_minimum_delta_r, HLT_with_OFF_match_num, 
	OFF_match_et, HLT_vs_OFF_resolution, HLT_vs_OFF_minimum_delta_eta, HLT_vs_OFF_minimum_delta_phi,

	// OFF matched to OFF
	OFF_matched_fraction, OFF_no_HLT_match_num, OFF_vs_HLT_minimum_delta_r, OFF_with_HLT_match_num, 
	HLT_match_et, OFF_vs_HLT_resolution, OFF_vs_HLT_minimum_delta_eta, OFF_vs_HLT_minimum_delta_phi
  );

  return StatusCode::SUCCESS;
}


float HLTCalo_L2CaloEMClustersMonitor::calculateDeltaR( float max_deltar, float eta_1, float phi_1, float eta_2, float phi_2 ) const {
  // reject the match as early as possible to avoid the expensive delta r calculation
  if (fabs(eta_1-eta_2) > max_deltar) return 99.9;
  double DeltaPhi = calculateDeltaPhi(phi_1, phi_2);
  if (DeltaPhi > max_deltar) return 99.9;
  return sqrt( ((eta_1-eta_2)*(eta_1-eta_2)) + (DeltaPhi*DeltaPhi) );
}

float HLTCalo_L2CaloEMClustersMonitor::calculateDeltaPhi( float phi_1, float phi_2 ) const {
  return fabs( fabs( fabs( phi_1 - phi_2 ) - TMath::Pi() ) - TMath::Pi() );
}
