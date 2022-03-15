/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPTOFSITALGORITHM_H
#define AFPTOFSITALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPToFHitContainer.h"

/**
 * @brief AFP monitoring algorithm combining data from Si-layer and ToF
 */
class AFPToFSiTAlgorithm : public AthMonitorAlgorithm {
  public:
    AFPToFSiTAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~AFPToFSiTAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  private:
    SG::ReadHandleKey<xAOD::AFPSiHitContainer> m_afpSiHitContainerKey;
    SG::ReadHandleKey<xAOD::AFPToFHitContainer> m_afpToFHitContainerKey;
};

#endif
