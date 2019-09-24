/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTCalo_L2CaloEMClustersMonitor.h"

HLTCalo_L2CaloEMClustersMonitor::HLTCalo_L2CaloEMClustersMonitor( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
  declareProperty("HLTContainer", m_HLT_cont_key = "HLT_L2CaloEMClusters");
  declareProperty("OFFContainer", m_OFF_cont_key = "egammaClusters");
  declareProperty("MonGroupName", m_mongroup_name = "");

  declareProperty("OFFTypes",  m_OFF_types);
  declareProperty("HLTMinET",  m_HLT_min_et  = -1.0);
  declareProperty("OFFMinET",  m_OFF_min_et  = -1.0);
  declareProperty("MaxDeltaR", m_max_delta_r = 0.04);
}


HLTCalo_L2CaloEMClustersMonitor::~HLTCalo_L2CaloEMClustersMonitor() {}


StatusCode HLTCalo_L2CaloEMClustersMonitor::initialize() {
  ATH_CHECK(m_HLT_cont_key.initialize());
  ATH_CHECK(m_OFF_cont_key.initialize());

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

  //////////////////
  // HLT CLUSTERS //
  //////////////////

  unsigned int n_hlt_clusters = 0;

  auto HLT_num = Monitored::Scalar<int>("HLT_num",0);
  auto HLT_et = Monitored::Scalar<float>("HLT_et",0.0);
  auto HLT_eta = Monitored::Scalar<float>("HLT_eta",0.0);
  auto HLT_phi = Monitored::Scalar<float>("HLT_phi",0.0);
  auto HLT_size = Monitored::Scalar<float>("HLT_size",0.0);

  // Loop over HLT clusters

  for (const auto& hlt_cluster : *hltCluster_readHandle) {

	if (hlt_cluster->et() < m_HLT_min_et) continue;

	++n_hlt_clusters;

	HLT_et = hlt_cluster->et() * 0.001;
	HLT_eta = hlt_cluster->eta();
	HLT_phi = hlt_cluster->phi();
	HLT_size = hlt_cluster->nCells();

  } // End loop over HLT clusters

  HLT_num = n_hlt_clusters;

  fill("TrigCaloMonitor",HLT_num,HLT_et,HLT_eta,HLT_phi,HLT_size);

  //////////////////
  // OFF CLUSTERS //
  //////////////////

  unsigned int n_off_clusters = 0;

  auto OFF_num = Monitored::Scalar<int>("OFF_num",0);
  auto OFF_et = Monitored::Scalar<float>("OFF_et",0.0);
  auto OFF_eta = Monitored::Scalar<float>("OFF_eta",0.0);
  auto OFF_phi = Monitored::Scalar<float>("OFF_phi",0.0);
  auto OFF_type = Monitored::Scalar<int>("OFF_type",0);

  //mutable xAOD::TrigEMCluster hlt_match; // For matching

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

	// matching HLT clusters to OFF clusters

	float min_delta_r  = 999999.9;

	for (const auto& hlt_cluster : *hltCluster_readHandle) {

		if (hlt_cluster->et() < m_HLT_min_et) continue;

		float delta_r = calculateDeltaR(off_cluster->eta(), off_cluster->phi(), hlt_cluster->eta(), hlt_cluster->phi());

		if (delta_r < min_delta_r) {

			min_delta_r = delta_r;
			//hlt_match = hlt_cluster; // avoid HLT double counts?
		}

	} // End loop over HLT clusters

  } // End loop over OFF clusters

  OFF_num = n_off_clusters;

  fill("TrigCaloMonitor",OFF_num,OFF_et,OFF_eta,OFF_phi,OFF_type);

  return StatusCode::SUCCESS;
}


float HLTCalo_L2CaloEMClustersMonitor::calculateDeltaR( float eta_1, float phi_1, float eta_2, float phi_2 ) const {

  double DeltaPhi = fabs( fabs( fabs( phi_1 - phi_2 ) - TMath::Pi() ) - TMath::Pi() );
  return sqrt( ((eta_1-eta_2)*(eta_1-eta_2)) + (DeltaPhi*DeltaPhi) );
}
