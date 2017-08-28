// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef TRIG_TrigL2ElectronFexMT_H 
#define TRIG_TrigL2ElectronFexMT_H

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

namespace Trk
{ class IParticleCaloExtensionTool; } 

/**
 * \class TrigL2ElectronFexMT 
 * \brief TrigL2ElectronFexMT is a Trigger Fex Algorithm that retrieves the L2 TrigEMCluster
 * container and the L2 TrackCollection containers and then creates a TrigElectron Container 
 * with a subset of calorimeter-ID selection variables that are calculated (eg. E/p)
 * TrigL2ElectronFexMT will apply some very loose selection cuts to the TrigElectronContainer created
 * which is of TrigParticle type TrigElectron
 * The TrigElectron conatiner will then be retrieved by the hypothesis algorithm TrigL2ElectronHypo
 * that will perform the corresponding L2 electron selection
 *
 * Cleanup / Update for Run2
 * Remove m_calo_algoID; m_trackalgoID -- only 1 type of track
 * Remove deta/dphi cluster -- this is checked at L2Calo
 * Remove track pt for TRT tracks -- TRT only not used
 * Remove m_calotrackdeta/dphiTRT -- not cutting on TRT tracks
 * calotrack deta/dphi and eoverp cuts flat (not as function of eta)
 */

class TrigL2ElectronFexMT : public AthAlgorithm  {
  
 public:

  TrigL2ElectronFexMT(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2ElectronFexMT();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();


 private:
  
  //tracking cut
  float  m_trackPtthr;
  float  m_trackPtthr_highet; //!< track pt cut for high et cluster (20 GeV)
  float  m_clusEtthr; //!< cluster Et threshold for high et cuts
  float m_calotrkdeta_noextrap; //!< preselection between track eta and cluster before extrapolation 
  float m_calotrkdeta_noextrap_highet; //!< preselection between track eta and cluster before extrapolation for high et cluster
  //calo-tracking cuts

  float m_calotrackdeta; //!<  deta between calo and track
  float m_calotrackdphi; //!<  dphi between calo and track
  float m_calotrackdeoverp_low; //!<  E/p lower cut between calo and track
  float m_calotrackdeoverp_high; //!<  E/p upper cut  between calo and track
  
  //radius and Z of calorimeter face
  float m_RCAL;  //!<  radious of calorimeter face
  float m_ZCAL;  //!<  Z of calorimeter face
  
  // build electrons for all tracks  
  bool m_acceptAll;

  // for extrapolating TrigInDetTracks to calorimeter surface
  ToolHandle< Trk::IParticleCaloExtensionTool > m_caloExtensionTool; 

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

  std::vector<float> m_calotrkdeta_noextrap_mon; //!< monitor preselection between track eta and cluster before extrapolation 
  std::vector<float> m_calotrackdeta_mon; 
  std::vector<float> m_calotrackdphi_mon; 
  std::vector<float> m_calotrackdeoverp_mon;
  std::vector<float> m_trackpt_mon;
  std::vector<float> m_calopt_mon;

  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
  SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_TrigEMClusterContainerKey;
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackParticleContainerKey;
  SG::WriteHandleKey<xAOD::TrigElectronContainer> m_outputElectronsKey;
};

#endif
