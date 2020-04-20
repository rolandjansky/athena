/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MonitorElectronAlgorithm_H
#define MonitorElectronAlgorithm_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

#include <string>

class MonitorElectronAlgorithm : public AthMonitorAlgorithm {

public:

    MonitorElectronAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~MonitorElectronAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:

    SG::ReadHandleKey<xAOD::ElectronContainer> m_ParticleContainerKey {this, "ParticleContainerName", "Electrons", "Name of electrons container" };

    Gaudi::Property<std::string> m_ParticlePrefix {this,"ParticlePrefix","electron","Name of the particle prefix to be used to define hists"};
    Gaudi::Property<std::string> m_RecoName {this,"RecoName","LHTight","Name of particle flavor in egamma reco"};
    Gaudi::Property<std::string> m_WithTrigger {this,"WithTrigger","","Name extension when electrons are chosen from triggered events"};
    Gaudi::Property<bool> m_CheckTrigger {this,"CheckTrigger",false,"Variable to check if trigger filter is to be used"};

};
#endif
