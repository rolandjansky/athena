/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_PPMSIMBSMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_PPMSIMBSMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

#include "TrigT1CaloToolInterfaces/IL1TriggerTowerToolRun3.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloCalibConditions/L1CaloRunParameters.h"  
#include "TrigT1CaloCalibConditions/L1CaloRunParametersContainer.h"  
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"


class PPMSimBSMonitorAlgorithm : public AthMonitorAlgorithm {
public:PPMSimBSMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~PPMSimBSMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  
  /// Struct to contain PPM trigger tower info
  struct MonitorTT { 
    const xAOD::TriggerTower* tower;
    double phiScaled; /// phi for 2d maps with integer bins (taking into account granularity in eta)
    double phi1d;     /// phi for 1d phi distributions (taking into account granularity in eta) 
    int jepET;
    double maxADC;    /// max ADC timeslice
  };


private:
  typedef std::vector<int> ErrorVector;
  
  StringProperty m_packageName{this,"PackageName","PPMSimBSMonitor","group name for histogramming"};

  /// container keys including steering parameter and description
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_xAODTriggerTowerContainerName{this, "BS_xAODTriggerTowerContainer",LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation,"Trigger Tower Container"};
 
  ToolHandle<LVL1::IL1TriggerTowerToolRun3> m_ttTool{this,"L1TriggerTowerToolRun3", "LVL1::L1TriggerTowerToolRun3/L1TriggerTowerToolRun3", "L1TriggerTowerToolRun3"};

    /// Properties
  Gaudi::Property<double> m_phiScaleTT{this, "phiScaleTT", 32./M_PI, "Scale factor to convert trigger tower phi to integer binning"};
  Gaudi::Property<int> m_simulationADCCut{this, "SimulationADCCut", 36, "Minimum ADC cut to avoid unnecessary simulation"};

  
  
  // Error vector StoreGate key
  SG::WriteHandleKey<std::vector<int>> m_errorLocation{this,"ErrorLocation","L1CaloPPMSimBSMismatchVector","ErrorVector"};

  
  // L1CaloCond rel22
  SG::ReadCondHandleKey<L1CaloRunParametersContainer>  m_runParametersContainer{ this, "InputKeyRunParameters", "L1CaloRunParametersContainer"};

  /// Helper functions
  StatusCode makePPMTower( const xAOD::TriggerTower* tt, 
                               std::vector<MonitorTT> &vecMonTT) const;

  StatusCode fillPPMEtaPhi( MonitorTT &monTT, 
                              const std::string& groupName, 
                              const std::string& weightName,
                              double weight) const;


 // count number of error events per lumiblock across threads for each type of error
  mutable std::mutex m_mutex{};
  mutable std::map<uint32_t, int> m_errorLB_tt_counter ATLAS_THREAD_SAFE; 


   
};
#endif

