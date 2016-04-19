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
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

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
  
private:
  const xAOD::CaloClusterContainer *m_caloCluster;
  const TrigRoiDescriptor* m_roiDescriptor;
  std::vector<const TrigRoiDescriptor*> m_roiDescriptorVector;
    
   /* ToolHandle<ITrigMuonRoITool> m_trigMuonRoITool; */
   /* ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc; */
   /* ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc; */
  ServiceHandle<TrigConf::ITrigConfigSvc > m_configSvc;

  std::vector<double> m_l1roi_eta;
  std::vector<double> m_l1roi_phi;
    
  std::vector<float> m_clustertime_5;
  std::vector<float> m_clustertime_10;
  std::vector<float> m_clustertime_25;
  std::vector<float> m_clustertime_50;

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
  
};

#endif

