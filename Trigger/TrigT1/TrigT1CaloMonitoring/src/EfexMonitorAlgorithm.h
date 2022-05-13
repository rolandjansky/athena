/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_EFEXMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_EFEXMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
//
#include "xAODTrigger/eFexEMRoIContainer.h"
#include "xAODTrigger/eFexEMRoI.h"
#include "xAODTrigger/eFexTauRoIContainer.h"
#include "xAODTrigger/eFexTauRoI.h"

class EfexMonitorAlgorithm : public AthMonitorAlgorithm {
public:EfexMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~EfexMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:

  StringProperty m_packageName{this,"PackageName","EfexMonitor","group name for histograming"};

  // container keys including this, steering parameter, default value and help description
  SG::ReadHandleKey<xAOD::eFexEMRoIContainer> m_eFexContainerKey{this,"eFexEMRoIContainer","L1_eEMRoI","SG key of the input eFex RoI container"};
  SG::ReadHandleKey<xAOD::eFexTauRoIContainer> m_eFexTauContainerKey{this,"eFexTauRoIContainer","L1_eTauRoI","SG key of the input eFex Tau RoI container"};

};
#endif
