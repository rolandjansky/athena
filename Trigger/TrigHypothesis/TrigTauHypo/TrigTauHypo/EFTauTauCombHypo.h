/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 ** File: Trigger/TrigHypothesis/TrigTauHypo/EFTauTauCombHypo.h
 **
 ** Description: HYPO algorithm: search for pairs of taus with delta eta in some interval +
 **              some additional cuts; intended for H-> tau tau SM and MSSM
 **
 ** Author: Phillip Urquijo <Phillip.Urquijo@cern.ch>
 ** Created: August 1
 **************************************************************************/

#ifndef TRIGTAUHYPO_TRIGEFTAUTAUCOMBHYPO_H
#define TRIGTAUHYPO_TRIGEFTAUTAUCOMBHYPO_H

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
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"

//tauRec includes
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauCommonExtraDetails.h"
#include "tauEvent/TauPID.h"

class EFTauTauCombHypo: public HLT::HypoAlgo  {
  
 public:
  EFTauTauCombHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~EFTauTauCombHypo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  
 private:
  
  // cuts
  float m_lowerMassCut; //!<  lower inv mass cut
  float m_upperMassCut; //!<  upper inv mass cut
  float m_MaxDR; //!< maximal dR=sqrt(dPhi^2 + dEta^2) cut
  float m_MinDR; //!< minimal dR=sqrt(dPhi^2 + dEta^2) cut 
  float m_MaxDPhi; //!< upper cut on tau-tau Delta Phi
  float m_MaxDEta; //!< upper cut on tau-tau Delta Eta
  bool  m_commonVertex; //!<  true if lepton tracks have common vertex
  bool  m_oppositeCharge; //!<  true if taus have opposite charge
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
  int    m_monVxState;
  int    m_moncombtype;
  
};

#endif
