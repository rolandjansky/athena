/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 ** File: Trigger/TrigHypothesis/TrigTauHypo/T2TauTauCombHypo.h
 **
 ** Description: HYPO algorithm: search for pairs of taus with delta eta in some interval +
 **              some additional cuts; intended for H-> tau tau SM and MSSM
 **
 ** Author: Phillip Urquijo <Phillip.Urquijo@cern.ch>
 ** Created: August 1
 **************************************************************************/

#ifndef TRIGTAUHYPO_TRIGL2TAUTAUCOMBHYPO_H
#define TRIGTAUHYPO_TRIGL2TAUTAUCOMBHYPO_H

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
#include "TrigParticle/TrigTau.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"
#include "TrigTopoEvent/ElectronMuonTopoInfo.h"

namespace HLT {
  class TriggerElement;
}

/**
 *
 * \class T2TauTauCombHypo
 * \brief T2TauTauCombHypo retrieves dPhi, dR and invaraiant masses calculated by T2TauTauCombFexAlgo
 *
 * Apply dEta cut on the two two candidates
 *
 **/

class T2TauTauCombHypo: public HLT::HypoAlgo {

 public:
  T2TauTauCombHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~T2TauTauCombHypo();
  
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

 private:
 
 //cuts
  float m_MaxDR; //!< maximal dR=sqrt(dPhi^2 + dEta^2) cut
  float m_MinDR; //!< minimal dR=sqrt(dPhi^2 + dEta^2) cut 
  float m_MaxDPhi; //!< upper cut on tau-tau Delta Phi
  float m_MaxDEta; //!< upper cut on tau-tau Delta Phi
  float m_lowerMassCut; //!<  lower inv mass cut
  float m_upperMassCut; //!<  upper inv mass cut
  std::string m_inputLabel; //!< label of the collection with tau-tau pairs

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
  bool  m_oppositeCharge; //!<  true if leptons have opposite charge
  bool  m_commonVertex; //!<  true if lepton tracks have common vertex
  int m_moncombtype;

};

#endif
