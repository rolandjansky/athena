/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

  Author : B. Laforge (laforge@lpnhe.in2p3.fr)
  4 May 2020
*/


#ifndef MonitorFwdElectronAlgorithm_H
#define MonitorFwdElectronAlgorithm_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

#include <string>

class MonitorFwdElectronAlgorithm : public AthMonitorAlgorithm  {
public:
    MonitorFwdElectronAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~MonitorFwdElectronAlgorithm();
    virtual StatusCode initialize() override;
    StatusCode fillHistograms( const EventContext& ctx ) const override;

    enum Regions : int { ENDCAP=0, FORWARD, NREGIONS };

private:

  SG::ReadHandleKey<xAOD::ElectronContainer> m_ParticleContainerKey {this, "ParticleContainerName", "ForwardElectrons", "Name of electron container" };
  Gaudi::Property<std::string> m_ParticlePrefix {this,"ParticlePrefix","FwdElectron","Name of the particle prefix to be used to define hists"};
  Gaudi::Property<std::string> m_RecoName {this,"RecoName","LHTight","Name of particle flavor in egamma reco"};
  Gaudi::Property<std::string> m_WithTrigger {this,"WithTrigger","","Name extension when electrons are chosen from triggered events"};

  Regions GetRegion(Float_t eta) const {
    Float_t aeta = fabs(eta);
    // check if object is in barrel
    if( aeta < 3.2 ) return MonitorFwdElectronAlgorithm::ENDCAP;
    return MonitorFwdElectronAlgorithm::FORWARD;
  }

};
#endif
