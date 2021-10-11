//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOBCIDAVGALG_H
#define CALOREC_CALOBCIDAVGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "CaloLumiConditions/CaloBCIDLumi.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

class CaloBCIDAvgAlg : public AthReentrantAlgorithm {
public:

  // constructor 
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  // Algorithm virtual methods 
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

private:
  //Event input: Only the BCID from Event Info
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","SG Key of EventInfo object"};

  //Event output: CaloBCIDAverage object
  SG::WriteHandleKey<CaloBCIDAverage> m_bcidAvgKey{this,"WriteKey","CaloBCIDAverage","SG Key of resulting CaloBCIDAverage object"};

  //ConditionsInput
  SG::ReadCondHandleKey<LArMCSym> m_mcSym{this,"MCSym","LArMCSym","SG Key of LArMCSym object"};
  SG::ReadCondHandleKey<CaloBCIDLumi> m_bcidLumiKey
  {this, "BCIDLumiKey", "CaloBCIDLumi", "SG Key of input CaloBCIDLumi conditions object"};

  /// Property: m_isSC
  Gaudi::Property<bool> m_isSC { this, "IsSuperCell",false,"Is SuperCell" };

  //Tool Handles:
  // For online monitoring purposes
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
};

#endif
