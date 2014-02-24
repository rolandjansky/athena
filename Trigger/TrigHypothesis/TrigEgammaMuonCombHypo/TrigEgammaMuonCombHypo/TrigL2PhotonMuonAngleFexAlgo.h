/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigL2PhotonMuonAngleFexAlgo.h
 **
 **   Description: - FEX algorithm: searc for pairs of photon and muons with 
 **                delta phi in some interval + some additional cuts; intended for a1->tautau->emu
 **                - Modified from TrigEFDiphotonMassHypo and TrigEFDiphotonMassFex by R Goncalo
			and TrigL2DimuonMassHypo by C. Schiavi
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Jan 30, 2011
 **
 **
 **************************************************************************/ 

#ifndef TRIGEGAMMAMUONCOMBHYPO_TRIGL2PHOTONMUONANGLEFEXALGO_H 
#define TRIGEGAMMAMUONCOMBHYPO_TRIGL2PHOTONMUONANGLEFEXALGO_H

// standard stuff
// trigger includes
#include "TrigInterfaces/ComboAlgo.h"


/* class StoreGateSvc; */
/* class TriggerElement; */

/**
 * \class TrigL2PhotonMuonAngleFexAlgo
 * \brief TrigL2PhotonMuonAngleFexAlgo is a Trigger Fex Algorithm that retrieves the L2 TrigPhoton
 * container and L2 combined muon feature
 * apply loose cuts on distance (it is possible to cut delta phi and on the full distance dR)
 * check charge: track pT is signed according to charge 
 * check vertex position: two leptons should come from common 
 * reject combination of leptons if distance squared between perigee of tracks is greater than a value
 * (should accept 99% of good combinations)
 * apply loose inv mass cuts (upper and lower cut) to the two leptons
 *
 */

class TrigL2PhotonMuonAngleFexAlgo: public HLT::ComboAlgo  {
  
public:
  TrigL2PhotonMuonAngleFexAlgo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2PhotonMuonAngleFexAlgo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
    
private:
  // monitoring
  double m_monMass;  
  double m_monDPhi;
  double m_monDR;
  
};




#endif
