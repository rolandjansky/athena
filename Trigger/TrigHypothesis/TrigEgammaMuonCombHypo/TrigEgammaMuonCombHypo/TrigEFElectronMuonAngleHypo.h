/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleHypo.h
 **
 **   Description: - Hypothesis algorithm: searc for pairs of electron and muons with 
 **                delta phi in some interval + some additional cuts; intended for a1->tautau->emu
 **                - Modified from TrigEFDielectronMassHypo and TrigEFDielectronMassFex by R Goncalo
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Jan 30, 2011
 **************************************************************************/ 

#ifndef TRIGEGAMMAMUONCOMBHYPO_TRIGEFELECTRONMUONANGLEHYPO_H 
#define TRIGEGAMMAMUONCOMBHYPO_TRIGEFELECTRONMUONANGLEHYPO_H

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
 * \class TrigEFElectronMuonAngleHypo
 * \brief TrigEFElectronMuonAngleHypo is a Trigger Hypo Algorithm that retrieves the distance (dPhi and dR) and invariant mass 
 * calculated by FEX algorithm. It also retrievers information about common vertex state (0=common, 1= not common, 2= not applicable)
 * Apply distance cut ( upper on dPhi, upper and lower on dR) to the two leptons, possibly also inv. mass cut and activates TE if condition is 
 * satisfied
 *
 */

class TrigEFElectronMuonAngleHypo: public HLT::HypoAlgo  {
  
  public:
    TrigEFElectronMuonAngleHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFElectronMuonAngleHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

  private:
        
    // cuts
    float m_lowerMassCut; //!<  lower inv mass cut
    float m_upperMassCut; //!<  upper inv mass cut
    float m_MaxDR; //!< maximal dR=sqrt(dPhi^2 + dEta^2) cut
  float m_MinDR; //!< minimal dR=sqrt(dPhi^2 + dEta^2) cut  
  float m_MaxDPhi; //!< upper cut on electron-muon Delta Phi
  bool  m_commonVertex; //!<  true if lepton tracks have common vertex
  bool  m_oppositeCharge; //!<  true if electtons have opposite charge
  std::string m_inputLabel; //!< label of input collection
  
	
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
	int    m_monVxState;
 
};

#endif
