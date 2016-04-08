/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJetHemisphereRec.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetHemisphereRec
//
// AUTHOR:   Lawrence Lee - based on TrigHLTJetRec by others
//
// Description: FexAlgo for turning a jet collection into a razor
// "mega-jet" / "jet hemisphere" collection of two jet objects
//
// ********************************************************************

#ifndef TRIGHLTJETREC_TRIGHLTJETREC_H
#define TRIGHLTJETREC_TRIGHLTJETREC_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "xAODJet/JetContainer.h"
#include <vector>

#include "TrigTimeAlgs/TrigTimerSvc.h"


class TrigHLTJetHemisphereRec: public HLT::FexAlgo {

 public:
  TrigHLTJetHemisphereRec(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetHemisphereRec();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass );
  HLT::ErrorCode hltFinalize();

  // Timing:

  TrigTimer* m_totalTimer;
  TrigTimer* m_hemiFindingTimer;
 
private:

  std::vector< xAOD::JetFourMom_t > m_selectedInputJetContainer;

  float m_jetPtCut;
  float m_jetEtaCut;
  float m_nJetPassThrough;


  int m_NJets;
  double m_jetP[20][4];
  double m_Ptot[4];
  double m_beta[3];
  double m_gamma;
  double m_dot, m_dotgamma;


  int m_ip_max[2];
  int m_jp_max[2];
  double m_P_max;
  double m_Pref[3];

  int m_ip[2], m_jp[2], m_Nhem[2];
  double m_Phem[3];
  double m_Phem_probe[3];

  double m_Pret[2][4];


  HLT::ErrorCode attachJetCollection(HLT::TriggerElement*,
                                     const xAOD::JetContainer*);

  // bool passJet(xAOD::Jet* jet);

}; 
#endif
