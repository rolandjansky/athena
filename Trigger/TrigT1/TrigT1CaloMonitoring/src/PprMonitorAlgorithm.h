/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
    double maxADC;    /// max ADC timeslice
  };


private:

  StringProperty m_packageName{this,"PackageName","PprMonitor","group name for histogramming"};

  /// container keys including steering parameter and description
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_xAODTriggerTowerContainerName{this, "BS_xAODTriggerTowerContainer",LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation,"Trigger Tower Container"};
  

  /// Properties
  Gaudi::Property<double> m_phiScaleTT{this, "phiScaleTT", 32./M_PI, "Scale factor to convert trigger tower phi to integer binning"};
  Gaudi::Property<int> m_TT_ADC_HitMap_Thresh{this, "TT_ADC_HitMap_Thresh", 50, "ADC cut for hit maps"};
  Gaudi::Property<int> m_SliceNo{this, "SliceNo", 15, "Number of possible time slices in the readout"};
  Gaudi::Property<int> m_EMFADCCut{this, "EMFADCCut", 40, "EM FADC cut for signal"};
  Gaudi::Property<int> m_TT_ADC_Pedestal{this, "ADCPedestal", 32, "Nominal pedestal value"};
  Gaudi::Property<std::vector<int>> m_TT_HitMap_ThreshVec{this, "LUTHitMap_ThreshVec", {0, 1, 3, 5, 10, 20, 30, 50}, "Thresholds for LUT hitmaps"}; 
  
  /// Groups for GenericMonitoringArrays
  std::map<std::string,int> m_groupTimeslice_EM; 
  std::map<std::string,int> m_groupTimeslice_HAD; 

  /// Helper functions
  
 StatusCode fillPPMTowerEtaPhi( const xAOD::TriggerTower_v2* tt, 
                               std::vector<MonitorTT> &vecMonTT_EM, 
                               std::vector<MonitorTT> &vecMonTT_HAD,  
                               std::vector<MonitorTT> &vecMonTT) const;

  double recTime(const std::vector<short unsigned int> &vFADC, int cut) const;

  std::string getPartition(int layer, double eta) const;

  StatusCode fillPPMEtaVsPhi( MonitorTT &monTT, 
                              const std::string& groupName, 
                              const std::string& weightName,
                              double weight=1.) const;

  StatusCode fillPPMPhi( MonitorTT &monTT,
                         const std::string& groupName) const;


};
#endif
