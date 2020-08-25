/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGENERICALGS_DETECTORTIMINGALGO_H
#define TRIGGENERICALGS_DETECTORTIMINGALGO_H

/**
 * @brief  Algorithm to analyse the detector timing information after the L1 correlation algo
 * @author Nils Ruthmann
 *
 * $Id: $
 */

//#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/AllTEAlgo.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"

#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigMuonToolInterfaces/ITrigMuonRoITool.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

/* #include "TrigL2MuonSA/MuFastDataPreparator.h" */
/* #include "TrigL2MuonSA/MuFastPatternFinder.h" */
/* #include "TrigL2MuonSA/MuFastStationFitter.h" */
/* #include "TrigL2MuonSA/MuFastTrackFitter.h" */
/* #include "TrigL2MuonSA/MuFastTrackExtrapolator.h" */
/* #include "TrigL2MuonSA/RecMuonRoIUtils.h" */
/* #include "TrigL2MuonSA/MuCalStreamerTool.h" */
#include "TrigL2MuonSA/RpcDataPreparator.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h" 

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class DetectorTimingAlgo : public HLT::AllTEAlgo {
public:

  unsigned int getBitMaskValue( const unsigned int uintValue, const unsigned int mask );
  
  DetectorTimingAlgo(const std::string& name, ISvcLocator* pSvcLocator);

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize()   { return HLT::OK; }

  /// Run once per event
  HLT::ErrorCode hltExecute(std::vector<HLT::TEVec>&, unsigned int output); 

  void updateHandler(Gaudi::Details::PropertyBase& p);
  // TrigL2MuonSA::RecMuonRoIUtils  m_recMuonRoIUtils;
  //  ToolHandle<TrigL2MuonSA::MuFastDataPreparator>     m_dataPreparator;
  ToolHandle<TrigL2MuonSA::RpcDataPreparator>m_rpcDataPreparator;
  ToolHandle<ITrigMuonRoITool> m_trigMuonRoITool;
  ToolHandle<TrigL2MuonSA::RpcPatFinder> m_dummypatfinder;
  //ToolHandle<RpcDataPreparator>  m_rpcDataPreparator;
  
  // ToolHandle<ITrigMuonBackExtrapolator> m_backExtrapolatorTool;
private:
  const xAOD::CaloClusterContainer *m_caloCluster;
  //const TrigRoiDescriptor* m_roiDescriptor;
  std::vector<const TrigRoiDescriptor*> m_roiDescriptorVector;

  //const xAOD::L2StandAloneMuonContainer *m_muonColl;
    
   /* ToolHandle<ITrigMuonRoITool> m_trigMuonRoITool; */
  ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc; 
   /* ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc; */
  ServiceHandle<TrigConf::ITrigConfigSvc > m_configSvc;

  std::vector<double> m_l1roi_eta;
  std::vector<double> m_l1roi_phi;
    
  std::vector<float> m_clustertime_5;
  std::vector<float> m_clustertime_10;
  float m_clustertime_10_sigma;
  std::vector<float> m_clustertime_25;
  std::vector<float> m_clustertime_50;

  std::vector<float> m_muonroi_intime_pt;
  std::vector<float> m_muonroi_intime_eta;
  std::vector<float> m_muonroi_intime_phi;
  std::vector<float> m_muonroi_outtime_eta;
  std::vector<float> m_muonroi_outtime_phi;

  std::vector<float> m_rpctime_matched_intimeroi;
  std::vector<float> m_rpctime_matched_outtimeroi;

  std::vector<float> m_rpctime_matched_intimeroi_averaged;
  std::vector<float> m_rpctime_matched_outtimeroi_averaged;

  std::vector<float> m_rpctime_matched_intimeroi_averaged_NOL1BC;
  std::vector<float> m_rpctime_matched_outtimeroi_averaged_NOL1BC;

  
  std::vector<float> m_rpceta_matched_intimeroi;
  std::vector<float> m_rpcphi_matched_intimeroi;
  std::vector<float> m_rpceta_matched_outtimeroi;
  std::vector<float> m_rpcphi_matched_outtimeroi;

  std::vector<float> m_rpctime_all;
  std::vector<float> m_rpctime_eta;
  std::vector<float> m_rpctime_phi;

  std::vector<float> m_rpctime_005;
  std::vector<float> m_rpctime_intimemuonl1;
  std::vector<float> m_rpctime_othermuonl1;
  
  //float m_ootfrac_bc;
  //float m_ootfrac_bc_10;
  //float m_ootfrac_bc_25;
  //float m_ootfrac_bc_50;
  //float m_ootfrac;
  //float m_itfrac;
  int m_accept;
  //double m_weighted_L1AClusterEta;
  double m_weighted_L1AClusterTime;

  double m_max_L1AClusterTime_10;
  double m_max_L1AClusterTime_2;
  double m_closest_L1AClusterTime;

  double m_summed_L1AClusterE;

  std::vector<double> m_L1ACluster_E;
  std::vector<double> m_L1ACluster_eta;
  std::vector<double> m_L1ACluster_phi;
  std::vector<double> m_L1ACluster_t;

  std::vector<double> m_L1ACluster_eta_funnypeak;
  std::vector<double> m_L1ACluster_phi_funnypeak;
  std::vector<double> m_L1ACluster_t_funnypeak;

  /* TrigL2MuonSA::MuonRoad     m_muonRoad; */
  /* TrigL2MuonSA::RpcHits      m_rpcHits; */
  /* TrigL2MuonSA::RpcFitResult m_rpcFitResult; */
  /* TrigL2MuonSA::TgcFitResult m_tgcFitResult; */
  /* TrigL2MuonSA::MdtHits      m_mdtHits_normal; */
  /* TrigL2MuonSA::MdtHits      m_mdtHits_overlap; */
  /* TrigL2MuonSA::CscHits      m_cscHits; */
  /* TrigL2MuonSA::MdtRegion    m_mdtRegion; */
  /*  float m_scaleRoadBarrelInner; */
  /*  float m_scaleRoadBarrelMiddle; */
  /*  float m_scaleRoadBarrelOuter; */

  const xAOD::TriggerTowerContainer *m_TriggerTowers;
  float m_TT_median;
  float m_TT_mean;
  float m_TT_sigma;
  int m_TT_nHad;
  int m_TT_nEM;
  int m_TT_total;
  std::vector<float> m_TT_timings;
  std::vector<float> m_TT_timings_noFCAL;
  std::vector<float> m_TT_timings_RoI;
  std::vector<float> m_deltaR;

  int m_y0, m_y1, m_y2, m_y3, m_y4;
  float m_x0, m_x1, m_x2, m_x3, m_x4;
  float m_eta, m_xv;
  int m_readout, m_TTAlg_accept;
  int  m_ObjAlg_accept_symmetric;
  //float m_TT_median_noFCAL, m_TT_median_RoI;
  //float m_TT_mean_noFCAL, m_TT_mean_RoI;
  //float m_TT_sigma_noFCAL, m_TT_sigma_RoI;


  float m_combinedObjects_mean;
  float m_combinedObjects_sigma;
  std::vector<float> m_combinedObjects_timings;

  float m_mu_mean;
  float m_mu_sigma;
  std::vector<float> m_mu_timings;
};

#endif

