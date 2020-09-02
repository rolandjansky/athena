/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

  Author : B. Laforge (laforge@lpnhe.in2p3.fr)
  4 May 2020
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
    StatusCode fillHistograms( const EventContext& ctx ) const override;

    enum Regions : int { BARREL=0, CRACK, ENDCAP, FORWARD, NREGIONS };

  private:
    SG::ReadHandleKey<xAOD::PhotonContainer> m_ParticleContainerKey {this, "ParticleContainerName", "Photons", "Name of electron container" };
    Gaudi::Property<std::string> m_ParticlePrefix {this,"ParticlePrefix","photon","Name of the particle prefix to be used to define hists"};
    Gaudi::Property<std::string> m_RecoName {this,"RecoName","Tight","Name of particle flavor in egamma reco"};
    Gaudi::Property<std::string> m_WithTrigger {this,"WithTrigger","","Name extension when electrons are chosen from triggered events"};

    Regions GetRegion(Float_t eta) const {
      Float_t aeta = fabs(eta);
      // check if object is in barrel
      if ( aeta < 1.37 ) return MonitorPhotonAlgorithm::BARREL;
      // check if object is in end-caps
      if ( aeta > 1.52 && aeta < 2.47 ) return MonitorPhotonAlgorithm::ENDCAP;
      // check if object is in crack region
      if ( aeta > 1.37 && aeta < 1.52 ) return MonitorPhotonAlgorithm::CRACK;
      return MonitorPhotonAlgorithm::NREGIONS; // out of acceptance
    }

};
#endif
