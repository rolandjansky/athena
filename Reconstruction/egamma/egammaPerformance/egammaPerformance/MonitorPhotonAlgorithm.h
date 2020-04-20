/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MonitorPhotonAlgorithm_H
#define MonitorPhotonAlgorithm_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonxAODHelpers.h"

#include <string>

class MonitorPhotonAlgorithm : public AthMonitorAlgorithm {

public:

    MonitorPhotonAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~MonitorPhotonAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:

    SG::ReadHandleKey<xAOD::PhotonContainer> m_ParticleContainerKey {this, "ParticleContainerName", "Photons", "Name of photons container" };

    Gaudi::Property<std::string> m_ParticlePrefix {this,"ParticlePrefix","photon","Name of the particle prefix to be used to define hists"};
    Gaudi::Property<std::string> m_RecoName {this,"RecoName","LHTight","Name of particle flavor in egamma reco"};
    Gaudi::Property<std::string> m_WithTrigger {this,"WithTrigger","","Name extension when photons are chosen from triggered events"};
    Gaudi::Property<bool> m_CheckTrigger {this,"CheckTrigger",false,"Variable to check if trigger filter is to be used"};


};
#endif
