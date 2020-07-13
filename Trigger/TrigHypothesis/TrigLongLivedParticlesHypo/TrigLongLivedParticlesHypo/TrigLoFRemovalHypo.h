/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigLoFRemovalHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigLongLivedParticlesHypo
 *
 * AUTHOR:   A. Policicchio 
 *
 *********************************************************************/
#ifndef TRIGLOFREMOVALHYPO_H
#define TRIGLOFREMOVALHYPO_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigLoFRemovalHypo : public HLT::HypoAlgo {

 public:

  TrigLoFRemovalHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigLoFRemovalHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:



  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  bool m_acceptAll;
  int m_LoFCellContSize;

  // Switch on Monitoring:

  int m_cutCounter;
  float m_CellContainerSize;
};
#endif

