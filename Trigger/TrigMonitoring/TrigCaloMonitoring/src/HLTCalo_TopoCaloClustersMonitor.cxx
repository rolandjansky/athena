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

  unsigned int n_hlt_clusters = 0;

  auto HLT_num = Monitored::Scalar<int>("HLT_num",0);
  auto HLT_et = Monitored::Scalar<float>("HLT_et",0.0);
  auto HLT_eta = Monitored::Scalar<float>("HLT_eta",0.0);
  auto HLT_phi = Monitored::Scalar<float>("HLT_phi",0.0);
  auto HLT_type = Monitored::Scalar<int>("HLT_type",0);
  auto HLT_size = Monitored::Scalar<float>("HLT_size",0.0);

  // Loop over HLT clusters

  for (const auto& hlt_cluster : *hltCluster_readHandle) {

	if (hlt_cluster->et() < m_HLT_min_et) continue;

	bool HLT_type_match = false;

	for (unsigned int n = 0; n < m_HLT_types.size(); ++n) {
		if(hlt_cluster->clusterSize() == m_HLT_types[n]) { HLT_type_match = true; break; }
	}

	if (!m_HLT_types.empty() && !HLT_type_match) continue;

	++n_hlt_clusters;

	HLT_et = hlt_cluster->et() * 0.001;
	HLT_eta = hlt_cluster->eta();
	HLT_phi = hlt_cluster->phi();
	HLT_type = hlt_cluster->clusterSize();
	if (hlt_cluster->isAvailable<int>("nCells")) {
		HLT_size = hlt_cluster->auxdata<int>("nCells");
	}

	fill(m_mongroup_name, HLT_et, HLT_eta, HLT_phi, HLT_type, HLT_size);

  } // End loop over HLT clusters

  HLT_num = n_hlt_clusters;

  fill(m_mongroup_name, HLT_num);

  //////////////////
  // OFF CLUSTERS //
  //////////////////

  unsigned int n_off_clusters = 0;
  unsigned int n_off_clusters_no_match = 0;
  unsigned int n_off_clusters_with_match = 0;

  // OFF cluster
  auto OFF_num = Monitored::Scalar<int>("OFF_num",0);
  auto OFF_et = Monitored::Scalar<float>("OFF_et",0.0);
  auto OFF_eta = Monitored::Scalar<float>("OFF_eta",0.0);
  auto OFF_phi = Monitored::Scalar<float>("OFF_phi",0.0);
  auto OFF_type = Monitored::Scalar<int>("OFF_type",0);

  // OFF cluster without HLT match
  auto OFF_no_HLT_match_num = Monitored::Scalar<int>("OFF_no_HLT_match_num",0);
  auto OFF_no_HLT_match_et = Monitored::Scalar<float>("OFF_no_HLT_match_et",0.0);
  auto OFF_no_HLT_match_eta = Monitored::Scalar<float>("OFF_no_HLT_match_eta",0.0);
  auto OFF_no_HLT_match_phi = Monitored::Scalar<float>("OFF_no_HLT_match_phi",0.0);
  auto OFF_no_HLT_match_type = Monitored::Scalar<int>("OFF_no_HLT_match_type",0);

  // OFF cluster with HLT match
  auto OFF_with_HLT_match_num = Monitored::Scalar<int>("OFF_with_HLT_match_num",0);
  auto OFF_with_HLT_match_et = Monitored::Scalar<float>("OFF_with_HLT_match_et",0.0);
  auto OFF_with_HLT_match_eta = Monitored::Scalar<float>("OFF_with_HLT_match_eta",0.0);
  auto OFF_with_HLT_match_phi = Monitored::Scalar<float>("OFF_with_HLT_match_phi",0.0);
  auto OFF_with_HLT_match_type = Monitored::Scalar<int>("OFF_with_HLT_match_type",0);

  // OFF clusters vs. HLT clusters
  auto HLT_vs_OFF_minimum_delta_r = Monitored::Scalar<float>("HLT_vs_OFF_minimum_delta_r",0.0);
  auto HLT_vs_OFF_delta_eta = Monitored::Scalar<float>("HLT_vs_OFF_delta_eta",0.0);
  auto HLT_vs_OFF_delta_phi = Monitored::Scalar<float>("HLT_vs_OFF_delta_phi",0.0);
  auto HLT_vs_OFF_resolution = Monitored::Scalar<float>("HLT_vs_OFF_resolution",0.0);
  auto HLT_match_et = Monitored::Scalar<float>("HLT_match_et",0.0);

  const xAOD::CaloCluster *hlt_match; // For matching

  // Loop over OFF clusters

  for (const auto& off_cluster : *offCluster_readHandle) {

	if (off_cluster->et() < m_OFF_min_et) continue;

	bool OFF_type_match = false;

	for (unsigned int n = 0; n < m_OFF_types.size(); n++) {
		if (off_cluster->clusterSize() == m_OFF_types[n]) { OFF_type_match = true; break; }
	}

	if (!m_OFF_types.empty() && !OFF_type_match) continue;

	++n_off_clusters;

	OFF_et = off_cluster->et() * 0.001;
	OFF_eta = off_cluster->eta();
	OFF_phi = off_cluster->phi();
	OFF_type = off_cluster->clusterSize();

	fill(m_mongroup_name, OFF_et, OFF_eta, OFF_phi, OFF_type);

	// matching HLT clusters to OFF clusters

	float min_delta_r  = 999999.9;

	for (const auto& hlt_cluster : *hltCluster_readHandle) {

		if (hlt_cluster->et() < m_HLT_min_et) continue;

		bool HLT_type_match = false;

		for (unsigned int n = 0; n < m_HLT_types.size(); ++n) {
			if(hlt_cluster->clusterSize() == m_HLT_types[n]) { HLT_type_match = true; break; }
		}

		if (!m_HLT_types.empty() && !HLT_type_match) continue;

		float delta_r = calculateDeltaR(off_cluster->eta(), off_cluster->phi(), hlt_cluster->eta(), hlt_cluster->phi());

		if (delta_r < min_delta_r) {

			min_delta_r = delta_r;
			hlt_match = hlt_cluster; // avoid HLT double counts?
		}

	} // End loop over HLT clusters

	HLT_vs_OFF_minimum_delta_r = min_delta_r;
	fill(m_mongroup_name, HLT_vs_OFF_minimum_delta_r);

	// No HLT match
	if (min_delta_r > m_max_delta_r) {

		++n_off_clusters_no_match;

		OFF_no_HLT_match_et = off_cluster->et() * 0.001;
		OFF_no_HLT_match_eta = off_cluster->eta();
		OFF_no_HLT_match_phi = off_cluster->phi();
		OFF_no_HLT_match_type = off_cluster->clusterSize();

		fill(m_mongroup_name, OFF_no_HLT_match_et, OFF_no_HLT_match_eta, OFF_no_HLT_match_phi, OFF_no_HLT_match_type);
	}

	// With HLT match
	else {

		++n_off_clusters_with_match;

		OFF_with_HLT_match_et = off_cluster->et() * 0.001;
		OFF_with_HLT_match_eta = off_cluster->eta();
		OFF_with_HLT_match_phi = off_cluster->phi();
		OFF_with_HLT_match_type = off_cluster->clusterSize();
		HLT_match_et = hlt_match->et() * 0.001;

		HLT_vs_OFF_resolution = ((off_cluster->et() - hlt_match->et()) / off_cluster->et()) * 100;
		HLT_vs_OFF_delta_eta = off_cluster->eta() - hlt_match->eta();
		HLT_vs_OFF_delta_phi = calculateDeltaPhi(off_cluster->phi(), hlt_match->phi());

		fill(m_mongroup_name, OFF_with_HLT_match_et, HLT_match_et, OFF_with_HLT_match_eta, OFF_with_HLT_match_phi, OFF_with_HLT_match_type, HLT_vs_OFF_resolution, HLT_vs_OFF_delta_eta, HLT_vs_OFF_delta_phi);

	}

  } // End loop over OFF clusters


  OFF_num = n_off_clusters;
  OFF_no_HLT_match_num = n_off_clusters_no_match;
  OFF_with_HLT_match_num = n_off_clusters_with_match;

  fill(m_mongroup_name, OFF_num, OFF_no_HLT_match_num, OFF_with_HLT_match_num);

  return StatusCode::SUCCESS;
}


float HLTCalo_TopoCaloClustersMonitor::calculateDeltaR( float eta_1, float phi_1, float eta_2, float phi_2 ) const {
  double DeltaPhi = calculateDeltaPhi(phi_1, phi_2);
  return sqrt( ((eta_1-eta_2)*(eta_1-eta_2)) + (DeltaPhi*DeltaPhi) );
}

float HLTCalo_TopoCaloClustersMonitor::calculateDeltaPhi( float phi_1, float phi_2 ) const {
  return fabs( fabs( fabs( phi_1 - phi_2 ) - TMath::Pi() ) - TMath::Pi() );
}
