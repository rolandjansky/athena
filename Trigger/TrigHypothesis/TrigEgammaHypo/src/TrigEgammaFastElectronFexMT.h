// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2ElectronFexMT.h
 **
 **   Description: Fex algo to be run before TrigL2ElectronHypo
 **
 **   Authors: R. Goncalo     <r.goncalo@rhul.ac.uk>
 **            V. Perez-Reale <perezr@mail.cern.ch>
 **            N. Berger      <Nicolas.Berger@cern.ch> Tue Nov 28 0:56:50 CET 2006
 **            P. Urquijo     <Phillip.Urquijo@cern.ch>
 **
 **   Created:  27 Jul 2017
 **************************************************************************/ 

#ifndef TRIG_TrigEgammaFastElectronFexMT_H 
#define TRIG_TrigEgammaFastElectronFexMT_H

// standard stuff
#include <vector>
#include <cmath> 

// general athena stuff
#include "GaudiKernel/IToolSvc.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h" 

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "CLHEP/Units/SystemOfUnits.h"

//namespace Trk
//{ class IParticleCaloExtensionTool; } 

/**
 * \class TrigEgammaFastElectronFexMT 
 * \brief TrigEgammaFastElectronFexMT is a Trigger Fex Algorithm that retrieves the L2 TrigEMCluster
 * container and the L2 TrackCollection containers and then creates a TrigElectron Container 
 * with a subset of calorimeter-ID selection variables that are calculated (eg. E/p)
 * TrigL2ElectronFexMT will apply some very loose selection cuts to the TrigElectronContainer created
 * which is of TrigParticle type TrigElectron
 * The TrigElectron conatiner will then be retrieved by the hypothesis algorithm TrigEgammaFastElectronHypo
 * that will perform the corresponding L2 electron selection
 *
 */

class TrigEgammaFastElectronFexMT : public AthAlgorithm  {
  
 public:

  TrigEgammaFastElectronFexMT(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEgammaFastElectronFexMT();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute() override;


 private:
  
  // track-extrapolation error counter
  unsigned long m_extrapolator_failed;

  ///Static getter methods for monitoring of TrigElectron container
  static inline double getCaloPt(const xAOD::TrigElectron* aElectron){
    if(!aElectron) return -999.0;
    return aElectron->pt();
  }

  static inline double getTkPt(const xAOD::TrigElectron* aElectron){
      if(!aElectron) return -999.0;
      return (aElectron->trackParticle() ? aElectron->trackParticle()->pt()   : -999);
  }

  bool extrapolate(const xAOD::TrigEMCluster *, const xAOD::TrackParticle *, double &, double &);
  
   // Algorithm properties: the parameters are {this, <name>, <default value>, <documentation>}
  Gaudi::Property<bool>  m_acceptAll  {this, "AcceptAll", false, "Build electrons for all tracks"};
  Gaudi::Property<float> m_clusEtthr {this,  "ClusEt",  20.0*CLHEP::GeV , " lower limit on cluster Et"};
  Gaudi::Property<float> m_trackPtthr {this,  "TrackPt",  5.0*CLHEP::GeV , "lower limit on TrackPt cut" };
  Gaudi::Property<float> m_calotrkdeta_noextrap {this,  "CaloTrackdEtaNoExtrap",   0.5, "Upper limit on DEta between Calo cluster and Track for track preselection before extrapolation"};
  Gaudi::Property<float> m_trackPtthr_highet  {this,  "TrackPtHighEt",  2.0*CLHEP::GeV , "lower limit on TrackPt cut High Et Cluster (20GeV)"};
  Gaudi::Property<float> m_calotrkdeta_noextrap_highet {this,  "CaloTrackdEtaNoExtrapHighEt",  0, "upper limit on DEta between Calo cluster and Track for track preselection before extrapolation for High Et cluster (20GeV)"};
  Gaudi::Property<float> m_calotrackdeta {this,  "CaloTrackdETA",    0, "Upper limit on DEta between Calo cluster and Track"};
  Gaudi::Property<float> m_calotrackdphi {this,  "CaloTrackdPHI",     0, "Upper limit on DPhi between Calo cluster and Track"}; 
  Gaudi::Property<float> m_calotrackdeoverp_low {this,  "CaloTrackdEoverPLow",  0, "lower limit on E(calo)/p(track)"};
  Gaudi::Property<float> m_calotrackdeoverp_high {this,  "CaloTrackdEoverPHigh", 0, "upper limit on track E(calo)/p(track)"};
  Gaudi::Property<float> m_RCAL {this,  "RCalBarrelFace",  1470.0*CLHEP::mm , "Radius of inner face of the barrel calorimeter"};
  Gaudi::Property<float> m_ZCAL {this,  "ZCalEndcapFace",     3800.0*CLHEP::mm, "z of the inner face of endcap calorimeter"};
  // Too be changed Public Tools depreciated
  ToolHandle<Trk::IParticleCaloExtensionTool > m_caloExtensionTool {this,  "ParticleCaloExtensionTool",  "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool", "Tool to extrapolate Track to Calo inner surface"};
 
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey { this, 
      "RoIs",                             // property name
      "rois",                                                        // default value of StoreGate key
      "input RoI Collection name"};
  
  SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_TrigEMClusterContainerKey{ this,
      "TrigEMClusterName",       // property name
      "clusters",                                                // default value of StoreGate key
      "input TrigEMCluster Container name"};
  
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackParticleContainerKey{ this,
      "TrackParticlesName",         // property name
      "Tracks",                                                  // default value of StoreGate key
      "input TrackParticle container name"};

  SG::WriteHandleKey<xAOD::TrigElectronContainer> m_outputElectronsKey{ this,
      "ElectronsName",                  // property name
      "Electrons",                                             // default value of StoreGate key
      "output Electron container name "};


  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  
};  

#endif
