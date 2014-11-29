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

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;
 
private:

  std::vector< xAOD::JetFourMom_t > m_selectedInputJetContainer;

  float m_jetPtCut;
  float m_jetEtaCut;
  float m_nJetPassThrough;

  HLT::ErrorCode attachJetCollection(HLT::TriggerElement*,
                                     const xAOD::JetContainer*);

  // bool passJet(xAOD::Jet* jet);

}; 
#endif
