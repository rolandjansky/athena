/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOMONITORALGORITHM_H
#define PFOMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElementContainer.h"


class PFOMonitorAlgorithm : public AthMonitorAlgorithm {
public:
    PFOMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~PFOMonitorAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
private:
    SG::ReadHandleKey<xAOD::FlowElementContainer> m_ChargedPFOContainerKey {this, "JetETMissChargedParticleFlowObjects", "JetETMissChargedParticleFlowObjects"};
    SG::ReadHandleKey<xAOD::FlowElementContainer> m_NeutralPFOContainerKey {this, "JetETMissNeutralParticleFlowObjects", "JetETMissNeutralParticleFlowObjects"};
    //release 21 and earlier
    //SG::ReadHandleKey<xAOD::PFOContainer> m_oldChargedPFOContainerKey {this, "JetETMissChargedParticleFlowObjects", "JetETMissChargedParticleFlowObjects"};
    //SG::ReadHandleKey<xAOD::PFOContainer> m_oldNeutralPFOContainerKey {this, "JetETMissNeutralParticleFlowObjects", "JetETMissNeutralParticleFlowObjects"};
};
#endif
