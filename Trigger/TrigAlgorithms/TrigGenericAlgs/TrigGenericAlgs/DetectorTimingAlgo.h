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

#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include "TrigMuonRoITools/ITrigMuonRoITool.h"
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

//#include "xAODTrigMuon/L2StandAloneMuonContainer.h" 

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

/// for muon tests
class Property;




class DetectorTimingAlgo : public HLT::AllTEAlgo {
public:
  DetectorTimingAlgo(const std::string& name, ISvcLocator* pSvcLocator);

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize()   { return HLT::OK; }

  /// Run once per event
  HLT::ErrorCode hltExecute(std::vector<HLT::TEVec>&, unsigned int output); 

  void updateHandler(Property& p);
  // TrigL2MuonSA::RecMuonRoIUtils  m_recMuonRoIUtils;
  //  ToolHandle<TrigL2MuonSA::MuFastDataPreparator>     m_dataPreparator;
  // ToolHandle<ITrigMuonBackExtrapolator> m_backExtrapolatorTool;
private:
  const xAOD::CaloClusterContainer *m_caloCluster;
  const TrigRoiDescriptor* m_roiDescriptor;
  std::vector<const TrigRoiDescriptor*> m_roiDescriptorVector;

  //  const xAOD::L2StandAloneMuonContainer *m_muonColl;
    
   /* ToolHandle<ITrigMuonRoITool> m_trigMuonRoITool; */
   /* ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc; */
   /* ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc; */
  ServiceHandle<TrigConf::ITrigConfigSvc > m_configSvc;

  std::vector<double> m_l1roi_eta;
  std::vector<double> m_l1roi_phi;
    
  std::vector<float> m_clustertime_5;
  std::vector<float> m_clustertime_10;
  float m_clustertime_10_sigma;
  std::vector<float> m_clustertime_25;
  std::vector<float> m_clustertime_50;
  std::vector<float> m_rpctime_15;
  
  float m_ootfrac_bc;
  float m_ootfrac_bc_10;
  float m_ootfrac_bc_25;
  float m_ootfrac_bc_50;
  float m_ootfrac;
  float m_itfrac;
  int accept;
  double weighted_L1AClusterEta;
  double weighted_L1AClusterTime;

  double max_L1AClusterTime_10;
  double max_L1AClusterTime_2;
  double closest_L1AClusterTime;

  double summed_L1AClusterE;

  std::vector<double> L1ACluster_E;
  std::vector<double> L1ACluster_eta;
  std::vector<double> L1ACluster_phi;
  std::vector<double> L1ACluster_t;

  std::vector<double> L1ACluster_eta_funnypeak;
  std::vector<double> L1ACluster_phi_funnypeak;
  std::vector<double> L1ACluster_t_funnypeak;

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
  float TT_median;
  float TT_mean;
  float TT_sigma;
  int TT_nHad;
  int TT_nEM;
  int TT_total;
  std::vector<float> TT_timings;
  std::vector<float> TT_timings_noFCAL;
  std::vector<float> TT_timings_RoI;
  std::vector<float> deltaR;

  int y0, y1, y2, y3, y4;
  float x0, x1, x2, x3, x4;
  float eta, xv;
  int readout, TTAlg_accept;

  float TT_median_noFCAL, TT_median_RoI;
  float TT_mean_noFCAL, TT_mean_RoI;
  float TT_sigma_noFCAL, TT_sigma_RoI;
};

#endif

