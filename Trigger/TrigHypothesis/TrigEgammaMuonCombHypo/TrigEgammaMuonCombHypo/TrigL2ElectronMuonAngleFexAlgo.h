/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigL2ElectronMuonAngleFexAlgo.h
 **
 **   Description: - FEX algorithm: searc for pairs of electron and muons with 
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

#ifndef TRIGEGAMMAMUONCOMBHYPO_TRIGL2ELECTRONMUONANGLEFEXALGO_H 
#define TRIGEGAMMAMUONCOMBHYPO_TRIGL2ELECTRONMUONANGLEFEXALGO_H

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
#include "TrigInterfaces/ComboAlgo.h"

// trigger EDM
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigParticle/TrigElectron.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"

/* class StoreGateSvc; */
/* class TriggerElement; */

/**
 * \class TrigL2ElectronMuonAngleFexAlgo
 * \brief TrigL2ElectronMuonAngleFexAlgo is a Trigger Fex Algorithm that retrieves the L2 TrigElectron
 * container and L2 combined muon feature
 * apply loose cuts on distance (it is possible to cut delta phi and on the full distance dR)
 * check charge: track pT is signed according to charge 
 * check vertex position: two leptons should come from common 
 * reject combination of leptons if distance squared between perigee of tracks is greater than a value
 * (should accept 99% of good combinations)
 * apply loose inv mass cuts (upper and lower cut) to the two leptons
 *
 *
namespace TrigL2ElectronMuonAngleHelpers {


  //bool validElectrons(const TrigElectron* electron1, const TrigElectron* electron2);

  
  enum Vertex {  Common, NotCommon, Unapplicable };
  Vertex commonVertex(const TrigElectron* electron1, const CombinedMuonFeature* muon1);

  bool opositeCharge(const TrigElectron* electron1, const CombinedMuonFeature* muon1);

  double invariantMass(const TrigElectron* electron1, const CombinedMuonFeature* muon1);
  
  double deltaPhi(const TrigElectron* electron1, const CombinedMuonFeature* muon1);
  double deltaR(const TrigElectron* electron1, const CombinedMuonFeature* muon1);
  
}
*/

class TrigL2ElectronMuonAngleFexAlgo: public HLT::ComboAlgo  {
  
public:
  TrigL2ElectronMuonAngleFexAlgo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2ElectronMuonAngleFexAlgo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );
  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
  
    
private:
  // cuts
  bool  m_electronValid; //!< true if electrons have different RoI positions 
  float m_MaxDR; //!< maximal dR=sqrt(dPhi^2 + dEta^2) cut
  float m_MaxDPhi; //!< upper cut on electron-muon Delta Phi
  float m_lowerMassCut; //!<  lower inv mass cut
  float m_upperMassCut; //!<  upper inv mass cut
  float m_DPhi; //!< calculated Delta Phi
  float m_DR; //!< calculated Delta R
  float m_mass; //!< calculated mass
  
  // to set Accept-All mode: should be done with force-accept when possible
  bool m_acceptAll; //!<  true will accept all events

  // monitoring
  double m_monMassAccepted;
  double m_monMassAll;  
  int    m_monCut;
  double m_monDPhiAccepted;
  double m_monDPhiAll;
  double m_monDRAccepted;
  double m_monDRAll;
  
   ElectronMuonTopoInfoContainer* m_egMuTopoColl; //!< Pointer to ElectronMuonTopoInfoCollection
  
};




#endif
