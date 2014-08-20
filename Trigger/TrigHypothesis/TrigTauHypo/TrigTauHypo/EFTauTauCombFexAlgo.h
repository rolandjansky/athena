/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 ** File: Trigger/TrigHypothesis/TrigTauHypo/EFTauTauCombFexAlgo.h
 **
 ** Description: FEX algorithm: search for pairs of taus with delta eta in some interval +
 **              some additional cuts; intended for H-> tau tau SM and MSSM
 **
 ** Author: Phillip Urquijo <Phillip.Urquijo@cern.ch>
 ** Created: August 1
 **************************************************************************/

#ifndef TRIGTAUHYPO_TRIGEFTAUTAUCOMBFEXALGO_H
#define TRIGTAUHYPO_TRIGEFTAUTAUCOMBFEXALGO_H

//standard
#include <string>
#include <map>
#include <cmath>

//gaudi and athena
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

//trigger includes
#include "TrigInterfaces/ComboAlgo.h"
#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"

//tauRec includes
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauCommonExtraDetails.h"
#include "tauEvent/TauPID.h"

//electron and muon
#include "egammaEvent/egammaContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "VxVertex/RecVertex.h"
#include "MuidEvent/MuidTrackContainer.h"



class EFTauTauCombFexAlgo: public HLT::ComboAlgo  {
  
 public:
  EFTauTauCombFexAlgo(const std::string & name, ISvcLocator* pSvcLocator);
  ~EFTauTauCombFexAlgo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );
  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
  
 private:
	       
  // cuts
  float m_lowerMassCut; //!<  lower inv mass cut
  float m_upperMassCut; //!<  upper inv mass cut
  float m_MaxDR; //!< maximal dR=sqrt(dPhi^2 + dEta^2) cut
//  float m_MinDR; //!< minimal dR=sqrt(dPhi^2 + dEta^2) cut 
  float m_MaxDPhi; //!< upper cut on tau-tau Delta Phi
  float m_MaxDEta; //!< upper cut on tau-tau Delta Phi
//  bool  m_commonVertex; //!<  true if lepton tracks have common vertex
//  bool  m_oppositeCharge; //!<  true if taus have opposite charge
  float m_mass; //!< calculated mass
  float m_DR; //!< calculated Delta R
  float m_DPhi; //!< calculated Delta Phi
  float m_DEta; //!< calculated Delta Eta

  std::string m_inputLabel; //!< label of input collection
  
  
  // to set Accept-All mode: should be done with force-accept when possible
  bool m_acceptAll;  //!<  true will accept all events
  // monitoring
  double m_monMassAccepted;
  double m_monMassAll; 
  int    m_monCut;
  double m_monDPhiAccepted;
  double m_monDPhiAll;
  double m_monDEtaAccepted;
  double m_monDEtaAll;
  double m_monDRAccepted;
  double m_monDRAll;
 // int    m_monVxState;
  int m_moncombtype;


  ElectronMuonTopoInfoContainer* m_tautauTopoColl; //!< Pointer to ElectronMuonTopoInfoCollection

};

#endif
