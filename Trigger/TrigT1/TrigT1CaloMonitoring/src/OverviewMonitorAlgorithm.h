/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_OVERVIEWMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_OVERVIEWMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
//#include "TrigT1Interfaces/TrigT1CaloDefs.h"

class OverviewMonitorAlgorithm : public AthMonitorAlgorithm {
public:OverviewMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~OverviewMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:

  StringProperty m_packageName{this,"PackageName","OverviewMonitor","group name for histograming"};
  
  // Error vector StoreGate key
  StringProperty m_ppmErrorLocation{this,"PPMErrorLocation","L1CaloPPMErrorVector","PPM Error Vector"};
  StringProperty m_ppmSpareErrorLocation{this,"PPMSpareErrorLocation","L1CaloPPMSpareErrorVector","PPM Spare Error Vector"};
  StringProperty m_cpmErrorLocation{this,"CPMErrorLocation","L1CaloCPMErrorVector","CPM Error Vector"};
  StringProperty m_jemErrorLocation{this,"JEMErrorLocation","L1CaloJEMErrorVector","JEM Error Vector"};
  StringProperty m_jemCmxErrorLocation{this,"JEMCMXErrorLocation","L1CaloJEMCMXErrorVector","JEM-CMX Error Vector"};
  StringProperty m_rodErrorLocation{this,"RODErrorLocation","L1CaloRODErrorVector","ROD Error Vector"};
  StringProperty m_ppmMismatchLocation{this,"PPMMismatchLocation","L1CaloPPMMismatchVector","PPM Mismatch Error Vector"};
  StringProperty m_cpmMismatchLocation{this,"CPMMismatchLocation","L1CaloCPMMismatchVector","CPM Mismatch Error Vector"};
  StringProperty m_jemMismatchLocation{this,"JEMMismatchLocation","L1CaloJEMMismatchVector","JEM Mismatch Error Vector"};

  typedef std::vector<int> ErrorVector;

  // Enums for global summary plot

  // Hardware errors
  // Error bits in PPM error data
  enum PPMErrors { DataStatus, DataError, PPMSubStatus };
  // Error bits in CPM error data
  enum CPMErrors {
    CPMEMParity,
    CPMEMLink,
    CPMHadParity,
    CPMHadLink,
    CPMStatus,
    CMXCPTobParity,
    CMXCPSumParity,
    CMXCPStatus
  };
  // Error bits in JEM error data
  enum JEMErrors {
    JEMEMParity,
    JEMHadParity,
    JEMEMLink,
    JEMHadLink,
    JEMStatus
  };
  // Error bits in JEM CMX error data
  enum CMXErrors {
    JEMCMXJetStatus,
    JEMCMXEnergyStatus,
    JEMCMXJetParity,
    JEMCMXEnergyParity
  };
  // Error bits in ROD error data
  enum RODErrors {
    GLink,
    /*CMMParity,*/ LVDSLink,
    FIFOOverflow,
    DataTransport,
    Timeout,
    BCNMismatch,
    TriggerType,
    NoPayload,
    NoFragment,
    ROBStatusError,
    UnpackingError
  };

  // Transmission/Comparison with simulation errors
  // Error bits in PPM simulation error data
  enum PPMMismatch { LUTMismatch };
  // Error bits in CPM simulation error data
  enum CPMMismatch {
    EMTowerMismatch,
    HadTowerMismatch,
    EMRoIMismatch,
    TauRoIMismatch,
    LeftCMXTobMismatch,
    RightCMXTobMismatch,
    LocalSumMismatch,
    RemoteSumMismatch,
    TotalSumMismatch,
    TopoMismatch
  };
  // Error bits in JEM simulation error data
  enum JEMMismatch {
    EMElementMismatch,
    HadElementMismatch,
    JEMRoIMismatch,
    CMXJetTobMismatch,
    LocalJetMismatch,
    RemoteJetMismatch,
    TotalJetMismatch,
    CMXJetTopoMismatch,
    JEMEtSumsMismatch,
    CMXEtSumsMismatch,
    LocalEnergyMismatch,
    RemoteEnergyMismatch,
    TotalEnergyMismatch,
    SumEtMismatch,
    MissingEtMismatch,
    MissingEtSigMismatch,
    EnergyRoIMismatch
  };

  // Bins for global error plots
  enum GlobalErrors {
    PPMDataStatus,
    PPMDataError,
    SubStatus,
    Parity,
    LinkDown,
    Transmission,
    Simulation,
    CMXSubStatus,
    GbCMXParity,
    CMXTransmission,
    CMXSimulation,
    RODStatus,
    RODMissing,
    ROBStatus,
    Unpacking,
    NumberOfGlobalErrors
  };

};
#endif
