/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

  Author : B. Laforge (laforge@lpnhe.in2p3.fr)
  4 May 2020
*/


#ifndef MonitorElectronAlgorithm_H
#define MonitorElectronAlgorithm_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

#include <string>

class MonitorElectronAlgorithm : public AthMonitorAlgorithm  {
public:
    MonitorElectronAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~MonitorElectronAlgorithm();
    virtual StatusCode initialize() override;
    StatusCode fillHistograms( const EventContext& ctx ) const override;

    enum Regions : int { BARREL=0, CRACK, ENDCAP, FORWARD, NREGIONS };

private:

  SG::ReadHandleKey<xAOD::ElectronContainer> m_ParticleContainerKey {this, "ParticleContainerName", "Electrons", "Name of electron container" };
  Gaudi::Property<std::string> m_ParticlePrefix {this,"ParticlePrefix","electron","Name of the particle prefix to be used to define hists"};
  Gaudi::Property<std::string> m_RecoName {this,"RecoName","LHTight","Name of particle flavor in egamma reco"};
  Gaudi::Property<std::string> m_WithTrigger {this,"WithTrigger","","Name extension when electrons are chosen from triggered events"};

  Regions GetRegion(Float_t eta) const {
    Float_t aeta = fabs(eta);
    // check if object is in barrel
    if ( aeta < 1.37 ) return MonitorElectronAlgorithm::BARREL;
    // check if object is in end-caps
    if ( aeta > 1.52 && aeta < 2.47 ) return MonitorElectronAlgorithm::ENDCAP;
    // check if object is in crack region
    if ( aeta > 1.37 && aeta < 1.52 ) return MonitorElectronAlgorithm::CRACK;
    return MonitorElectronAlgorithm::NREGIONS; // out of acceptance
  }

};
#endif
