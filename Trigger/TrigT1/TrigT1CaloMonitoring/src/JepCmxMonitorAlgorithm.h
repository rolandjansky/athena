/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_JEPCMXMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_JEPCMXMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"
#include "xAODTrigL1Calo/CMXEtSumsContainer.h"
#include "xAODTrigL1Calo/CMXRoIContainer.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/CrateEnergy.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

class JepCmxMonitorAlgorithm : public AthMonitorAlgorithm {
public:JepCmxMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JepCmxMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:

  StringProperty m_packageName{this,"PackageName","JepCmxMonitor","group name for histograming"};

  Gaudi::Property<int> m_crates       {this, "s_crates",        2,  "Number of JEM crates"};
  Gaudi::Property<int> m_modules      {this, "s_modules",       16, "Number of modules per crate"};
  Gaudi::Property<int> m_tobsPerJem   {this, "s_tobsPerJem",    4,  "Maximum number of TOBs per JEM sent to CMX"};
  Gaudi::Property<int> m_maxTobsPerCmx{this, "s_maxTobsPerCmx", 64, "Maximum number of TOBs per CMX plotted"};

  enum SummaryErrors { JetStatus, EnergyStatus, JetParity, EnergyParity, NumberOfSummaryBins };

  // container keys including steering parameter and description
  SG::ReadHandleKey<xAOD::CMXJetTobContainer>  m_CMXJetTobLocation {this, "CMXJetTobLocation",  LVL1::TrigT1CaloDefs::CMXJetTobLocation,  "CMX Jet TOB Container"};
  SG::ReadHandleKey<xAOD::CMXJetHitsContainer> m_CMXJetHitsLocation{this, "CMXJetHitsLocation", LVL1::TrigT1CaloDefs::CMXJetHitsLocation, "CMX Jet Hits Container"};
  SG::ReadHandleKey<xAOD::CMXEtSumsContainer>  m_CMXEtSumsLocation {this, "CMXEtSumsLocation",  LVL1::TrigT1CaloDefs::CMXEtSumsLocation,  "CMX Et Sums Container"};
  SG::ReadHandleKey<xAOD::CMXRoIContainer>     m_CMXRoILocation    {this, "CMXRoILocation",     LVL1::TrigT1CaloDefs::CMXRoILocation,     "CMX RoI Container"};

  void fillThresholds(Monitored::Scalar<int> &monVar,
                      Monitored::Scalar<int> &hitVar,
                      int val, int nThresh, 
                      int nBits, int offset = 0
                      ) const;

  void fillThresholdsVsY(Monitored::Scalar<int> &monVarX,
                         Monitored::Scalar<int> &monVarY,
                         Monitored::Scalar<int> &hitVar,
                         int val, int y, int nThresh,
                         int nBits, int offset = 0
                         ) const;
};

#endif
