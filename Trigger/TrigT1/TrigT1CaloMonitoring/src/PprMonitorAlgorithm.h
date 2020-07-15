/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_PPRMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_PPRMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

class PprMonitorAlgorithm : public AthMonitorAlgorithm {
public:PprMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~PprMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
 
  /// Struct to contain PPM trigger tower info
  struct MonitorTT { 
    const xAOD::TriggerTower_v2* tower;
    double phiScaled; /// phi for 2d maps with integer bins (taking into account granularity in eta)
    double phi1d;     /// phi for 1d phi distributions (taking into account granularity in eta) 
    int jepET;
    bool isDuplicate; /// Bookkeeping of multiple bins in phi for a given eta bin in the forward region 
  };


private:

  StringProperty m_packageName{this,"PackageName","PprMonitor","group name for histogramming"};

  /// container keys including steering parameter and description
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_xAODTriggerTowerContainerName{this, "BS_xAODTriggerTowerContainer",LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation,"Trigger Tower Container"};
  

  /// Properties
  Gaudi::Property<double> m_phiScaleTT{this, "phiScaleTT", 32./M_PI, "Scale factor to convert trigger tower phi to integer binning"};
  Gaudi::Property<int> m_TT_ADC_HitMap_Thresh{this, "TT_ADC_HitMap_Thresh", 50, "ADC cut for hit maps"};


  /// Helper functions
  StatusCode fillPPMTowerEtaPhi( const xAOD::TriggerTower_v2* tt, 
                               std::vector<MonitorTT> &vecMonTT_EM, 
                               std::vector<MonitorTT> &vecMonTT_HAD,  
                               std::vector<MonitorTT> &vecMonTT) const;

};
#endif
