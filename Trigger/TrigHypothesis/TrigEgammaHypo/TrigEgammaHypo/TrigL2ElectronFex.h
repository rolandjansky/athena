// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2ElectronFex.h
 **
 **   Description: Fex algo to be run before TrigL2ElectronHypo
 **
 **   Authors: R. Goncalo     <r.goncalo@rhul.ac.uk>
 **            V. Perez-Reale <perezr@mail.cern.ch>
 **            N. Berger      <Nicolas.Berger@cern.ch> Tue Nov 28 0:56:50 CET 2006
 **            P. Urquijo     <Phillip.Urquijo@cern.ch>
 **
 **   Created:  Tue Nov 28 0:56:50 CET 2006
 **   Modified:  V. Perez Reale added doxygen comments 26/6/07
 **
 **************************************************************************/ 

#ifndef TRIG_TrigL2ElectronFex_H 
#define TRIG_TrigL2ElectronFex_H

// standard stuff
#include <vector>
#include <cmath> 

// general athena stuff
#include "GaudiKernel/IToolSvc.h"

// trigger includes
#include "TrigInterfaces/FexAlgo.h"

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

//#include "TrigCaloEvent/RingerEvent.h" // will be moved to TrigEvent
//#include "TrigCaloEvent/RingerEventContainer.h" // will be moved to TrigEvent

//class ITrigInDetTrackExtrapolator;
//class IExtrapolateToCaloTool;
namespace Trk
{ class IParticleCaloExtensionTool; } 

/**
 * \class TrigL2ElectronFex 
 * \brief TrigL2ElectronFex is a Trigger Fex Algorithm that retrieves the L2 TrigEMCluster
 * container and the L2 TrackCollection containers and then creates a TrigElectron Container 
 * with a subset of calorimeter-ID selection variables that are calculated (eg. E/p)
 * TrigL2ElectronFex will apply some very loose selection cuts to the TrigElectronContainer created
 * which is of TrigParticle type TrigElectron
 * The TrigElectron conatiner will then be retrieved by the hypothesis algorithm TrigL2ElectronHypo
 * that will perform the corresponding L2 electron selection
 *
 */

class TrigL2ElectronFex : public HLT::FexAlgo  {
  
 public:

  TrigL2ElectronFex(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2ElectronFex();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  
  //HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass);

 private:
  
  // Properties:
  unsigned int m_trackalgoID; // used to select algorithm producing tracks, 0: all, 1: SiTrack, 2: IDScan, 4: TRTxK, 5: SiTrack+IDScan
  unsigned int m_calo_algoID; // used to run from either TrigEMCluster or RingerEvent

  //L1-L2 matching variables
  float  m_detacluster;
  float  m_dphicluster;
  
  //tracking cut
  float  m_trackPtthr;
  float  m_trackPtthrTRT; //Track pt cut on TRT tracks
  
  //calo-tracking cuts
  std::vector<float> m_etabin;
  std::vector<float> m_calotrackdeta; //!<  deta between calo and track
  std::vector<float> m_calotrackdphi; //!<  dphi between calo and track
  std::vector<float> m_calotrackdeoverp_low; //!<  E/p lower cut between calo and track
  std::vector<float> m_calotrackdeoverp_high; //!<  E/p upper cut  between calo and track

  //TRTSegFinder specific calo-track cuts
  std::vector<float> m_calotrackdetaTRT; //!<  deta between calo and track
  std::vector<float> m_calotrackdphiTRT; //!<  dphi between calo and track

  
  //radius and Z of calorimeter face
  float m_RCAL;  //!<  radious of calorimeter face
  float m_ZCAL;  //!<  Z of calorimeter face
  
  // to set Accept-All mode and to be able 
  // to avoid generating TrigElectrons
  bool m_acceptAll;
  //  bool m_saveTrigEl; // now needed to transfer information to hypo

  // for extrapolating TrigInDetTracks to calorimeter surface
//  ITrigInDetTrackExtrapolator* m_trackExtrapolator; //!<  pointer for extrapolating TrigInDetTracks to calorimeter surface
  //ToolHandle<IExtrapolateToCaloTool> m_trackExtrapolator;
  ToolHandle< Trk::IParticleCaloExtensionTool > m_caloExtensionTool; 
//  std::string m_trackExtrapolatorName;

  xAOD::TrigElectronContainer* m_trigElecColl; //!<  pointer to TrigElectron container

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


};

#endif
