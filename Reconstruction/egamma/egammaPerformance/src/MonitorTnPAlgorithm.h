/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

  Author : B. Laforge (laforge@lpnhe.in2p3.fr)
  4 May 2020
*/


#ifndef MonitorTnPAlgorithm_H
#define MonitorTnPAlgorithm_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

#include <string>

class MonitorTnPAlgorithm : public AthMonitorAlgorithm  {
public:
    MonitorTnPAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~MonitorTnPAlgorithm();
    virtual StatusCode initialize() override;
    StatusCode fillHistograms( const EventContext& ctx ) const override;
    StatusCode fillElectronProbe(const xAOD::Electron *el, const bool isleading, const EventContext& ctx) const;
    enum Regions : int { BARREL=0, CRACK, ENDCAP, FORWARD, NREGIONS };

private:

  SG::ReadHandleKey<xAOD::ElectronContainer> m_ParticleContainerKey {this, "ParticleContainerName", "Electrons", "Name of electron container" };
  Gaudi::Property<std::string> m_ParticlePrefix {this,"ParticlePrefix","electron","Name of the particle prefix to be used to define hists"};
  Gaudi::Property<std::string> m_RecoName {this,"RecoName","LHLoose","Name of particle flavor in egamma reco"};
  Gaudi::Property<std::string> m_TnPType {this,"TnPType","Z","Variable to describe what is the TnP resonance name"};

  Gaudi::Property<Float_t> m_MassPeak {this, "MassPeak", 91188, "Resonance peak position"};
  Gaudi::Property<Float_t> m_ElectronEtCut {this, "ElectronEtCut", 15000, "Et cut for electron"};
  Gaudi::Property<Float_t> m_MassLowerCut {this, "MassLowerCut", 70000, "Lower Mass cut"};
  Gaudi::Property<Float_t> m_MassUpperCut {this, "MassUpperCut", 110000, "Upper Mass Cut"};

  Regions GetRegion(Float_t eta) const {
    Float_t aeta = fabs(eta);
    // check if object is in barrel
    if ( aeta < 1.37 ) return MonitorTnPAlgorithm::BARREL;
    // check if object is in end-caps
    if ( aeta > 1.52 && aeta < 2.47 ) return MonitorTnPAlgorithm::ENDCAP;
    // check if object is in crack region
    if ( aeta > 1.37 && aeta < 1.52 ) return MonitorTnPAlgorithm::CRACK;
    return MonitorTnPAlgorithm::NREGIONS; // out of acceptance
  }

};
#endif
