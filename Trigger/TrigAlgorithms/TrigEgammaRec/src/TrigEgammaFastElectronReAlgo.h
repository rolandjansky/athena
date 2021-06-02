// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **   Description: Fex algo to be run before TrigL2ElectronHypo
 **
 **   Authors: R. Goncalo     <r.goncalo@rhul.ac.uk>
 **            V. Perez-Reale <perezr@mail.cern.ch>
 **            N. Berger      <Nicolas.Berger@cern.ch> Tue Nov 28 0:56:50 CET 2006
 **            P. Urquijo     <Phillip.Urquijo@cern.ch>
 **
 **   Created:  27 Jul 2017
 **************************************************************************/ 

#ifndef TrigEgammaFastElectronReAlgo_H 
#define TrigEgammaFastElectronReAlgo_H

#include <vector>
#include <cmath> 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h" 
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


/**
 * \class TrigEgammaFastElectronReAlgo 
 * \brief TrigEgammaFastElectronReAlgo is a Trigger Fex Algorithm that retrieves the TrigEMCluster
 * container and the TrackCollection containers and then creates a TrigElectron Container 
 * with a subset of calorimeter-ID selection variables that are calculated (eg. E/p)
 * The algo will apply some very loose selection cuts to the TrigElectronContainer created
 * which is of TrigParticle type TrigElectron
 * The TrigElectron conatiner will then be retrieved by the hypothesis algorithm TrigEgammaFastElectronHypo
 * that will perform the corresponding fast electron selection
 */

class TrigEgammaFastElectronReAlgo : public AthReentrantAlgorithm  {
  
  public:

    TrigEgammaFastElectronReAlgo(const std::string & name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;


  private:


    // Static getter methods for monitoring of TrigElectron container
    static inline double getCaloPt(const xAOD::TrigElectron* el) {
      if(!el) return -999.0;
      return el->pt();
    }

    static inline double getTkPt(const xAOD::TrigElectron* el) {
      if(!el) return -999.0;
      return (el->trackParticle() ? el->trackParticle()->pt()   : -999);
    }

    bool extrapolate(const xAOD::TrigEMCluster *, const xAOD::TrackParticle *, double &, double &) const;

     // Algorithm properties: the parameters are {this, <name>, <default value>, <documentation>}
    Gaudi::Property<bool>  m_acceptAll  {this, "AcceptAll", false, "Build electrons for all tracks"};
    Gaudi::Property<float> m_clusEtthr {this,  "ClusEt",  20.0*Gaudi::Units::GeV , " lower limit on cluster Et"};
    Gaudi::Property<float> m_trackPtthr {this,  "TrackPt",  5.0*Gaudi::Units::GeV , "lower limit on TrackPt cut" };
    Gaudi::Property<float> m_calotrkdeta_noextrap {this,  "CaloTrackdEtaNoExtrap",   0.5, "Upper limit on DEta between Calo cluster and Track for track preselection before extrapolation"};
    Gaudi::Property<float> m_trackPtthr_highet  {this,  "TrackPtHighEt",  2.0*Gaudi::Units::GeV , "lower limit on TrackPt cut High Et Cluster (20GeV)"};
    Gaudi::Property<float> m_calotrkdeta_noextrap_highet {this,  "CaloTrackdEtaNoExtrapHighEt",  0, "upper limit on DEta between Calo cluster and Track for track preselection before extrapolation for High Et cluster (20GeV)"};
    Gaudi::Property<float> m_calotrackdeta {this,  "CaloTrackdETA",    0, "Upper limit on DEta between Calo cluster and Track"};
    Gaudi::Property<float> m_calotrackdphi {this,  "CaloTrackdPHI",     0, "Upper limit on DPhi between Calo cluster and Track"}; 
    Gaudi::Property<float> m_calotrackdeoverp_low {this,  "CaloTrackdEoverPLow",  0, "lower limit on E(calo)/p(track)"};
    Gaudi::Property<float> m_calotrackdeoverp_high {this,  "CaloTrackdEoverPHigh", 0, "upper limit on track E(calo)/p(track)"};
    Gaudi::Property<float> m_RCAL {this,  "RCalBarrelFace",  1470.0*Gaudi::Units::mm , "Radius of inner face of the barrel calorimeter"};
    Gaudi::Property<float> m_ZCAL {this,  "ZCalEndcapFace",     3800.0*Gaudi::Units::mm, "z of the inner face of endcap calorimeter"};
    // Too be changed Public Tools depreciated
    ToolHandle<Trk::IParticleCaloExtensionTool > m_caloExtensionTool {this,  "ParticleCaloExtensionTool",  "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool", "Tool to extrapolate Track to Calo inner surface"};
 
    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey { this, 
        "RoIs",                             // property name
        "rois",                             // default value of StoreGate key
        "input RoI Collection name"};

    SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_TrigEMClusterContainerKey{ this,
        "TrigEMClusterName",       // property name
        "clusters",                // default value of StoreGate key
        "input TrigEMCluster Container name"};

    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackParticleContainerKey{ this,
        "TrackParticlesName",         // property name
        "Tracks",                     // default value of StoreGate key
        "input TrackParticle container name"};

    SG::WriteHandleKey<xAOD::TrigElectronContainer> m_outputElectronsKey{ this,
        "ElectronsName",                  // property name
        "Electrons",                      // default value of StoreGate key
        "output Electron container name "};

    SG::WriteHandleKey<xAOD::TrigElectronContainer> m_outputDummyElectronsKey{ this,
        "DummyElectronsName",                  // property name
        "Electrons",                           // default value of StoreGate key
        "output Dummy Electron container name "};

    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  
};  

#endif
