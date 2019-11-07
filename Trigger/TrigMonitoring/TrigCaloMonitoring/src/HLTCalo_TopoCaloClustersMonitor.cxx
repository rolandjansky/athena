/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTCalo_TopoCaloClustersMonitor.h"

HLTCalo_TopoCaloClustersMonitor::HLTCalo_TopoCaloClustersMonitor( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
  declareProperty("HLTContainer", m_HLT_cont_key = "HLT_TopoCaloClustersFS");
  declareProperty("OFFContainer", m_OFF_cont_key = "CaloCalTopoClusters");
  declareProperty("MonGroupName", m_mongroup_name = "TrigCaloMonitor");

  declareProperty("HLTTypes",  m_HLT_types);
  declareProperty("OFFTypes",  m_OFF_types);
  declareProperty("HLThighET", m_HLT_high_et = 5000.0);
  declareProperty("HLTMinET",  m_HLT_min_et  = -1.0);
  declareProperty("OFFMinET",  m_OFF_min_et  = -1.0);
  declareProperty("MatchType", m_match_types = false);
  declareProperty("MaxDeltaR", m_max_delta_r = 0.04);
}


HLTCalo_TopoCaloClustersMonitor::~HLTCalo_TopoCaloClustersMonitor() {}


StatusCode HLTCalo_TopoCaloClustersMonitor::initialize() {
  ATH_CHECK(m_HLT_cont_key.initialize());
  ATH_CHECK(m_OFF_cont_key.initialize());

  return AthMonitorAlgorithm::initialize();
}


StatusCode HLTCalo_TopoCaloClustersMonitor::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  // Get HLT cluster collections
  SG::ReadHandle<xAOD::CaloClusterContainer> hltCluster_readHandle(m_HLT_cont_key, ctx);
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

  //////////////////
  // HLT CLUSTERS //
  //////////////////

  // All HLT clusters
  unsigned int n_hlt_clusters = 0;
  std::vector<float> vec_hlt_et, vec_hlt_eta, vec_hlt_phi, vec_hlt_size, vec_hlt_time;
  std::vector<int> vec_hlt_type;

  auto HLT_num = Monitored::Scalar<int>("HLT_num",0);
  auto HLT_et = Monitored::Collection("HLT_et", vec_hlt_et);
  auto HLT_eta = Monitored::Collection("HLT_eta", vec_hlt_eta);
  auto HLT_phi = Monitored::Collection("HLT_phi", vec_hlt_phi);
  auto HLT_type = Monitored::Collection("HLT_type", vec_hlt_type);
  auto HLT_size = Monitored::Collection("HLT_size", vec_hlt_size);
  auto HLT_time = Monitored::Collection("HLT_time", vec_hlt_time);

  // Only High-ET clusters
  unsigned int n_hlt_high_et_clusters = 0;
  std::vector<float> vec_hlt_high_et_et, vec_hlt_high_et_eta, vec_hlt_high_et_phi, vec_hlt_high_et_size, vec_hlt_high_et_time;
  std::vector<int> vec_hlt_high_et_type;

  auto HLT_high_et_num = Monitored::Scalar<int>("HLT_high_et_num",0);
  auto HLT_high_et_et = Monitored::Collection("HLT_high_et_et", vec_hlt_high_et_et);
  auto HLT_high_et_eta = Monitored::Collection("HLT_high_et_eta", vec_hlt_high_et_eta);
  auto HLT_high_et_phi = Monitored::Collection("HLT_high_et_phi", vec_hlt_high_et_phi);
  auto HLT_high_et_type = Monitored::Collection("HLT_high_et_type", vec_hlt_high_et_type);
  auto HLT_high_et_size = Monitored::Collection("HLT_high_et_size", vec_hlt_high_et_size);
  auto HLT_high_et_time = Monitored::Collection("HLT_high_et_time", vec_hlt_high_et_time);

  // Loop over HLT clusters

  for (const auto& hlt_cluster : *hltCluster_readHandle) {

	if (hlt_cluster->et() < m_HLT_min_et) continue;

	bool HLT_type_match = false;

	for (unsigned int n = 0; n < m_HLT_types.size(); ++n) {
		if(hlt_cluster->clusterSize() == m_HLT_types[n]) { HLT_type_match = true; break; }
	}

	if (!m_HLT_types.empty() && !HLT_type_match) continue;

	++n_hlt_clusters;

	vec_hlt_et.push_back(hlt_cluster->et() * 0.001);
	vec_hlt_eta.push_back(hlt_cluster->eta());
	vec_hlt_phi.push_back(hlt_cluster->phi());
	vec_hlt_type.push_back(hlt_cluster->clusterSize());
	vec_hlt_time.push_back(hlt_cluster->time());
	if (hlt_cluster->isAvailable<int>("nCells")) {
		vec_hlt_size.push_back(hlt_cluster->auxdata<int>("nCells"));
	}

	// high-ET clusters
	if (hlt_cluster->et() > m_HLT_high_et) {

		++n_hlt_high_et_clusters;

		vec_hlt_high_et_et.push_back(hlt_cluster->et() * 0.001);
		vec_hlt_high_et_eta.push_back(hlt_cluster->eta());
		vec_hlt_high_et_phi.push_back(hlt_cluster->phi());
		vec_hlt_high_et_type.push_back(hlt_cluster->clusterSize());
		vec_hlt_high_et_time.push_back(hlt_cluster->time());
		if (hlt_cluster->isAvailable<int>("nCells")) {
			vec_hlt_high_et_size.push_back(hlt_cluster->auxdata<int>("nCells"));
		}

	}

  } // End loop over HLT clusters

  HLT_num = n_hlt_clusters;
  HLT_high_et_num = n_hlt_high_et_clusters;

  fill(m_mongroup_name, HLT_num, HLT_et, HLT_eta, HLT_phi, HLT_time, HLT_type, HLT_size, HLT_high_et_num, HLT_high_et_et, HLT_high_et_eta, HLT_high_et_phi, HLT_high_et_time, HLT_high_et_type, HLT_high_et_size);

  //////////////////
  // OFF CLUSTERS //
  //////////////////

  unsigned int n_off_clusters = 0;
  unsigned int n_off_clusters_no_match = 0;
  unsigned int n_off_clusters_with_match = 0;

  // OFF cluster
  std::vector<float> vec_off_et, vec_off_eta, vec_off_phi, vec_off_time;
  std::vector<int> vec_off_type;

  auto OFF_num = Monitored::Scalar<int>("OFF_num",0);
  auto OFF_et = Monitored::Collection("OFF_et", vec_off_et);
  auto OFF_eta = Monitored::Collection("OFF_eta", vec_off_eta);
  auto OFF_phi = Monitored::Collection("OFF_phi", vec_off_phi);
  auto OFF_time = Monitored::Collection("OFF_time", vec_off_time);
  auto OFF_type = Monitored::Collection("OFF_type", vec_off_type);

  // OFF cluster without HLT match
  std::vector<float> vec_off_no_hlt_match_et, vec_off_no_hlt_match_eta, vec_off_no_hlt_match_phi, vec_off_no_hlt_match_time;
  std::vector<int> vec_off_no_hlt_match_type;

  auto OFF_no_HLT_match_num = Monitored::Scalar<int>("OFF_no_HLT_match_num",0);
  auto OFF_no_HLT_match_et = Monitored::Collection("OFF_no_HLT_match_et", vec_off_no_hlt_match_et);
  auto OFF_no_HLT_match_eta = Monitored::Collection("OFF_no_HLT_match_eta", vec_off_no_hlt_match_eta);
  auto OFF_no_HLT_match_phi = Monitored::Collection("OFF_no_HLT_match_phi", vec_off_no_hlt_match_phi);
  auto OFF_no_HLT_match_time = Monitored::Collection("OFF_no_HLT_match_time", vec_off_no_hlt_match_time);
  auto OFF_no_HLT_match_type = Monitored::Collection("OFF_no_HLT_match_type", vec_off_no_hlt_match_type);

  // OFF cluster with HLT match
  std::vector<float> vec_off_with_hlt_match_et, vec_off_with_hlt_match_eta, vec_off_with_hlt_match_phi, vec_off_with_hlt_match_time;
  std::vector<int> vec_off_with_hlt_match_type;

  auto OFF_with_HLT_match_num = Monitored::Scalar<int>("OFF_with_HLT_match_num",0);
  auto OFF_with_HLT_match_et = Monitored::Collection("OFF_with_HLT_match_et", vec_off_with_hlt_match_et);
  auto OFF_with_HLT_match_eta = Monitored::Collection("OFF_with_HLT_match_eta", vec_off_with_hlt_match_eta);
  auto OFF_with_HLT_match_phi = Monitored::Collection("OFF_with_HLT_match_phi", vec_off_with_hlt_match_phi);
  auto OFF_with_HLT_match_time = Monitored::Collection("OFF_with_HLT_match_time", vec_off_with_hlt_match_time);
  auto OFF_with_HLT_match_type = Monitored::Collection("OFF_with_HLT_match_type", vec_off_with_hlt_match_type);

  // OFF clusters vs. HLT clusters
  std::vector<float> vec_hlt_vs_off_minimum_delta_r, vec_hlt_vs_off_delta_eta, vec_hlt_vs_off_delta_phi, vec_hlt_vs_off_delta_time, vec_hlt_vs_off_resolution, vec_hlt_match_et;

  auto HLT_vs_OFF_minimum_delta_r = Monitored::Collection("HLT_vs_OFF_minimum_delta_r", vec_hlt_vs_off_minimum_delta_r);
  auto HLT_vs_OFF_delta_eta = Monitored::Collection("HLT_vs_OFF_delta_eta", vec_hlt_vs_off_delta_eta);
  auto HLT_vs_OFF_delta_phi = Monitored::Collection("HLT_vs_OFF_delta_phi", vec_hlt_vs_off_delta_phi);
  auto HLT_vs_OFF_delta_time = Monitored::Collection("HLT_vs_OFF_delta_time", vec_hlt_vs_off_delta_time);
  auto HLT_vs_OFF_resolution = Monitored::Collection("HLT_vs_OFF_resolution", vec_hlt_vs_off_resolution);
  auto HLT_match_et = Monitored::Collection("HLT_match_et", vec_hlt_match_et);

  const xAOD::CaloCluster *hlt_match = nullptr; // For matching

  // Loop over OFF clusters

  for (const auto& off_cluster : *offCluster_readHandle) {

	if (off_cluster->et() < m_OFF_min_et) continue;

	bool OFF_type_match = false;

	for (unsigned int n = 0; n < m_OFF_types.size(); n++) {
		if (off_cluster->clusterSize() == m_OFF_types[n]) { OFF_type_match = true; break; }
	}

	if (!m_OFF_types.empty() && !OFF_type_match) continue;

	++n_off_clusters;

	vec_off_et.push_back(off_cluster->et()*0.001);
	vec_off_eta.push_back(off_cluster->eta());
	vec_off_phi.push_back(off_cluster->phi());
	vec_off_time.push_back(off_cluster->time());
	vec_off_type.push_back(off_cluster->clusterSize());

	// matching HLT clusters to OFF clusters

	float min_delta_r  = 999999.9;

	for (const auto& hlt_cluster : *hltCluster_readHandle) {

		if (hlt_cluster->et() < m_HLT_min_et) continue;

		bool HLT_type_match = false;

		for (unsigned int n = 0; n < m_HLT_types.size(); ++n) {
			if(hlt_cluster->clusterSize() == m_HLT_types[n]) { HLT_type_match = true; break; }
		}

		if (!m_HLT_types.empty() && !HLT_type_match) continue;

		float delta_r = calculateDeltaR(m_max_delta_r, off_cluster->eta(), off_cluster->phi(), hlt_cluster->eta(), hlt_cluster->phi());

		if (delta_r < min_delta_r) {

			min_delta_r = delta_r;
			hlt_match = hlt_cluster; // avoid HLT double counts?
		}

	} // End loop over HLT clusters

	vec_hlt_vs_off_minimum_delta_r.push_back(min_delta_r);

	// No HLT match
	if (min_delta_r >= m_max_delta_r) {

		++n_off_clusters_no_match;

		vec_off_no_hlt_match_et.push_back(off_cluster->et()*0.001);
		vec_off_no_hlt_match_eta.push_back(off_cluster->eta());
		vec_off_no_hlt_match_phi.push_back(off_cluster->phi());
		vec_off_no_hlt_match_time.push_back(off_cluster->time());
		vec_off_no_hlt_match_type.push_back(off_cluster->clusterSize());

	}

	// With HLT match
	else {

		++n_off_clusters_with_match;

		vec_off_with_hlt_match_et.push_back(off_cluster->et()*0.001);
		vec_off_with_hlt_match_eta.push_back(off_cluster->eta());
		vec_off_with_hlt_match_phi.push_back(off_cluster->phi());
		vec_off_with_hlt_match_time.push_back(off_cluster->time());
		vec_off_with_hlt_match_type.push_back(off_cluster->clusterSize());
		vec_hlt_match_et.push_back(hlt_match->et() * 0.001);

		vec_hlt_vs_off_resolution.push_back(((off_cluster->et() - hlt_match->et()) / off_cluster->et()) * 100);
		vec_hlt_vs_off_delta_eta.push_back(off_cluster->eta() - hlt_match->eta());
		vec_hlt_vs_off_delta_phi.push_back(calculateDeltaPhi(off_cluster->phi(), hlt_match->phi()));
		vec_hlt_vs_off_delta_time.push_back(off_cluster->time() - hlt_match->time());

	}

  } // End loop over OFF clusters


  OFF_num = n_off_clusters;
  OFF_no_HLT_match_num = n_off_clusters_no_match;
  OFF_with_HLT_match_num = n_off_clusters_with_match;

  fill(m_mongroup_name, OFF_num, OFF_et, OFF_eta, OFF_phi, OFF_time, OFF_type, OFF_no_HLT_match_num, HLT_vs_OFF_minimum_delta_r, OFF_no_HLT_match_et, OFF_no_HLT_match_eta, OFF_no_HLT_match_phi, OFF_no_HLT_match_time, OFF_no_HLT_match_type, OFF_with_HLT_match_num, OFF_with_HLT_match_et, HLT_match_et, OFF_with_HLT_match_eta, OFF_with_HLT_match_phi, OFF_with_HLT_match_time, OFF_with_HLT_match_type, HLT_vs_OFF_resolution, HLT_vs_OFF_delta_eta, HLT_vs_OFF_delta_phi, HLT_vs_OFF_delta_time);

  return StatusCode::SUCCESS;
}


float HLTCalo_TopoCaloClustersMonitor::calculateDeltaR( float max_deltar, float eta_1, float phi_1, float eta_2, float phi_2 ) const {
  // reject the match as early as possible to avoid the expensive delta r calculation
  if (fabs(eta_1-eta_2) > max_deltar) return 99.9;
  double DeltaPhi = calculateDeltaPhi(phi_1, phi_2);
  if (DeltaPhi > max_deltar) return 99.9;
  return sqrt( ((eta_1-eta_2)*(eta_1-eta_2)) + (DeltaPhi*DeltaPhi) );
}

float HLTCalo_TopoCaloClustersMonitor::calculateDeltaPhi( float phi_1, float phi_2 ) const {
  return fabs( fabs( fabs( phi_1 - phi_2 ) - TMath::Pi() ) - TMath::Pi() );
}
