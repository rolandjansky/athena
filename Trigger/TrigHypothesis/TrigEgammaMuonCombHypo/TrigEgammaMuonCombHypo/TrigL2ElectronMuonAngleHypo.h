/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigL2ElectronMuonAngleHypo.h
 **
 **   Description: - Hypo algorithm: searc for pairs of electron and muons with 
 **                delta phi in some interval + some additional cuts; intended for a1->tautau->emu
 **                - Modified from TrigEFDielectronMassHypo and TrigEFDielectronMassFex by R Goncalo
			and TrigL2DimuonMassHypo by C. Schiavi
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Jan 30, 2011
 **
 **
 **************************************************************************/ 

#ifndef TRIGEGAMMAMUONCOMBHYPO_TRIGL2ELECTRONMUONANGLEHYPO_H 
#define TRIGEGAMMAMUONCOMBHYPO_TRIGL2ELECTRONMUONANGLEHYPO_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
// trigger includes
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
// trigger EDM
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigParticle/TrigElectron.h"
#include "TrigParticle/TrigElectronContainer.h"

/* class StoreGateSvc; */
/* class TriggerElement; */


/**
 * \class TrigL2ElectronMuonAngleHypo
 * \brief TrigL2ElectronMuonAngleHypo is a Trigger Hypo Algorithm that retrieves distance dPhi and dR
 * and inv mass calculated by TrigL2ElectronMuonAngleFexAlgo
 * 
 * Apply dPhi cut, dR and inv mass cuts (upper and lower cut) to the two leptons and activates TE if condition is 
 * satisfied
 *
 */

class TrigL2ElectronMuonAngleHypo: public HLT::HypoAlgo  {
  
  public:
    TrigL2ElectronMuonAngleHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2ElectronMuonAngleHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

private:
  
  // cuts
  float m_MaxDR; //!< maximal dR=sqrt(dPhi^2 + dEta^2) cut
  float m_MinDR; //!< minimal dR=sqrt(dPhi^2 + dEta^2) cut  
  float m_MaxDPhi; //!< upper cut on electron-muon Delta Phi
  float m_lowerMassCut; //!<  lower inv mass cut
  float m_upperMassCut; //!<  upper inv mass cut
  std::string m_inputLabel; //!< label of the collection with Egamma-Muon pairs

  // to set Accept-All mode: should be done with force-accept when possible
  bool m_acceptAll;  //!<  true will accept all events
  // monitoring
  double m_monMassAccepted;
  double m_monMassAll;  
  int    m_monCut;
  double m_monDPhiAccepted;
  double m_monDPhiAll;
  double m_monDRAccepted;
  double m_monDRAll;
  bool  m_oppositeCharge; //!<  true if leptons have opposite charge
  bool  m_commonVertex; //!<  true if lepton tracks have common vertex
  
};

#endif
