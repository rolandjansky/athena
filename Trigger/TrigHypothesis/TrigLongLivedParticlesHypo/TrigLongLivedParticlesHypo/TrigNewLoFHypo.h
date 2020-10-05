/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNEWLOFHYPO_H
#define TRIGNEWLOFHYPO_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigNewLoFHypo : public HLT::HypoAlgo {

 public:

  TrigNewLoFHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigNewLoFHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  int m_accepted;
  int m_rejected;
  int m_errors;

  bool m_acceptAll;
  int m_LoFCellContSize;

  int m_cutCounter;
  float m_CellContainerSize;
  float m_numberCells;
};
#endif

