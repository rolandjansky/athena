/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 ** File: Trigger/TrigHypothesis/TrigTauHypo/T2TauTauCombFexAlgo.h
 **
 ** Description: FEX algorithm: search for pairs of taus with delta eta in some interval +
 **              some additional cuts; intended for H-> tau tau SM and MSSM
 **
 ** Author: Phillip Urquijo <Phillip.Urquijo@cern.ch>
 ** Created: July 27
 **************************************************************************/

#ifndef TRIGTAUHYPO_TRIGL2TAUTAUCOMBFEXALGO_H
#define TRIGTAUHYPO_TRIGL2TAUTAUCOMBFEXALGO_H

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

//trigger EDM
#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticle/TrigElectron.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"
#include "TrigTopoEvent/ElectronMuonTopoInfo.h"

class T2TauTauCombFexAlgo: public HLT::ComboAlgo{
 public:
  T2TauTauCombFexAlgo(const std::string & name, ISvcLocator* pSvcLocator);
  ~T2TauTauCombFexAlgo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );
  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);

 private:
  bool m_tauValid; //!< true if taus have different RoI positions
  float m_MaxDR; //!< maximal dR=sqrt(dPhi^2 + dEta^2) cut
  float m_MaxDPhi; //!< upper cut on tau-tau Delta Phi
  float m_MaxDEta; //!< upper cut on tau-tau Delta Eta
  float m_lowerMassCut; //!<  lower inv mass cut
  float m_upperMassCut; //!<  upper inv mass cut
  float m_DPhi; //!< calculated Delta Phi
  float m_DR; //!< calculated Delta R
  float m_DEta; //!< calculated Delta Eta from dPhi and dR
  float m_mass; //!< calculated mass

  bool m_acceptAll; //!<  true will accept all events
  
  //monitoring
  double m_monMassAccepted;
  double m_monMassAll; 
  int    m_monCut;
  double m_monDPhiAccepted;
  double m_monDPhiAll;
  double m_monDEtaAccepted;
  double m_monDEtaAll;
  double m_monDRAccepted;
  double m_monDRAll;
  int m_moncombtype;

  //The following may need to be changed to a tautau top info container, for now lets test with an existing EDM 
  ElectronMuonTopoInfoContainer* m_tautauTopoColl; //!< Pointer to ElectronMuonTopoInfoCollection
};

#endif
