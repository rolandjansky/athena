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
 **   Modified:     R. White optimisation for v7 menu (Run2) 07/02/2017
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
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"

/**
 * \class TrigL2ElectronHypo
 * \brief TrigL2ElectronHypo is a Trigger Hypo Algorithm that retrieves the L2 TrigElectronContainer
 * created by TrigL2ElectronFex and then apply a subset of electron selection cuts. A TE will be
 * set active if the selection cuts are fullfilled.
 * Cleanup / Update for Run2
 * Remove m_calo_algoID; m_trackalgoID -- only 1 type of track
 * Remove deta/dphi cluster -- this is checked at L2Calo
 * Remove track pt for TRT tracks -- TRT only not used
 * Remove m_calotrackdeta/dphiTRT -- not cutting on TRT tracks
 * calotrack deta/dphi and eoverp cuts flat (not as function of eta)
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
  
  //tracking cut
  float  m_trackPtthr; //!< pT cut on track
  
  //calo-tracking cuts
  float m_calotrackdeta; //!<  deta between calo and track
  float m_calotrackdphi;  //!<  dphi between calo and track
  float m_calotrackdeoverp_low;  //!<  E/p lower cut between calo and track
  float m_calotrackdeoverp_high; //!<  E/p upper cut between calo and track 
  float m_trtratio; //!< trt ratio cut


  // to set Accept-All mode
  bool m_acceptAll; //!<  accept all events if true

  // monitoring variables
  int m_cutCounter;

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

};

#endif
