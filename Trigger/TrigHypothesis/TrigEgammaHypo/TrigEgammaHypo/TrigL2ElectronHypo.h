// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2ElectronHypo.h
 **
 **   Description: Hypothesis algorithms to be run at Level 2 after 
 **                tracking algorithms: creates TrigElectrons
 **
 **   Authour: R.Goncalo <r.goncalo@rhul.ac.uk> Thu Jan 20 22:02:43 BST 2005
 **
 **   Created:      Sat Mar  1 19:55:56 GMT 2005
 **   Modified:     V. Perez Reale added doxygen comments 26/6/07
 **
 **************************************************************************/ 

#ifndef TRIG_TrigL2ElectronHypo_H 
#define TRIG_TrigL2ElectronHypo_H

// standard stuff
#include <map>
#include <cmath> 

// general athena stuff
#include "GaudiKernel/IToolSvc.h"

// trigger includes
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

// trigger EDM
//#include "TrigInDetEvent/TrigInDetTrack.h"
//#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "xAODTracking/TrackParticleContainer.h"

//#include "TrigCaloEvent/TrigEMCluster.h"
//#include "TrigParticle/TrigElectron.h"
//#include "TrigParticle/TrigElectronContainer.h"

#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"


//class ITrigInDetTrackExtrapolator;


/**
 * \class TrigL2ElectronHypo
 * \brief TrigL2ElectronHypo is a Trigger Hypo Algorithm that retrieves the L2 TrigElectronContainer
 * created by TrigL2ElectronFex and then apply a subset of electron selection cuts. A TE will be
 * set active if the selection cuts are fullfilled.
 *
 *
 */

class TrigL2ElectronHypo: public HLT::HypoAlgo  {
  
 public:

  TrigL2ElectronHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2ElectronHypo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  
 private:
  
  // Properties:
  unsigned int m_trackalgoID; //!<  integer that assigns the tracking algorihtm used
  
  //L1-L2 matching variables
  float  m_detacluster; //!<  deta between track and L1 roI
  float  m_dphicluster; //!<  dphi between track and L1 roI
  
  //tracking cut
  float  m_trackPtthr; //!< pT cut on track
  int m_NPixelSpacePoints;
  int m_NSCT_SpacePoints;
  int m_NStrawHits;
  
  //calo-tracking cuts
  std::vector<float> m_etabin;
  std::vector<float> m_calotrackdeta; //!<  deta between calo and track
  std::vector<float> m_calotrackdphi;  //!<  dphi between calo and track
  std::vector<float> m_calotrackdeoverp_low;  //!<  E/p lower cut between calo and track
  std::vector<float> m_calotrackdeoverp_high; //!<  E/p upper cut between calo and track  

  //radius and Z of calorimeter face
  float m_RCAL;  //!<  radious of calorimeter face
  float m_ZCAL;  //!<  Z of calorimeter face
  
  //TRT cuts
  std::vector<float> m_trtratio; //!< cut on ratio of NTRHits/NTRStrawHits for IDScan and SiTrack tracks

  //TRTSegFinder cuts
  float  m_trackPtthrTRT; //!< pT cut on track
  std::vector<float> m_etabinTRT; //!< eta bins for TRT cuts
  std::vector<float> m_trtratioTRT; //!< cut on ratio of NTRHits/NTRStrawHits for TRTSegFinder tracks
  std::vector<float> m_calotrackdetaTRT; //!<  deta between calo and track
  std::vector<float> m_calotrackdphiTRT;  //!<  dphi between calo and track
  std::vector<float> m_calotrackdeoverp_lowTRT;  //!<  E/p lower cut between calo and track
  std::vector<float> m_calotrackdeoverp_highTRT; //!<  E/p upper cut between calo and track  


  // to set Accept-All mode
  bool m_acceptAll; //!<  accept all events if true

  // monitoring variables
  int m_cutCounter;
  float m_dEta, m_dPhi, m_EoverP, m_TRTRatio;

  //needed for monitoring to work
  const xAOD::TrigElectronContainer* m_egamma_container;

  ///Static getter methods for monitoring of TrigElectron container
  static inline double getCaloPt(const xAOD::TrigElectron* aElectron){
    
    if(!aElectron) return -999.0;
    return aElectron->pt();
  }

  static inline double getTkPt(const xAOD::TrigElectron* aElectron){

  if(!aElectron) return -999.0;
  return (aElectron->trackParticle() ? aElectron->trackParticle()->pt()   : -999); 
}

  // for extrapolating TrigInDetTracks to calorimeter surface
//   ITrigInDetTrackExtrapolator* m_trackExtrapolator;
//   std::string m_trackExtrapolatorName;
};

#endif
