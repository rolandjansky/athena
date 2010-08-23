/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGPILEUPHYPO_H
#define TRIGPILEUPHYPO_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;


class TrigPileUpHypo : public HLT::HypoAlgo {

 public:

  TrigPileUpHypo(const std::string& name, ISvcLocator* pSvcLocator); 

  HLT::ErrorCode hltInitialize(); 
  HLT::ErrorCode hltFinalize();   
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool & pass);

 private:

  std::string  m_featureLabel;

  double m_CutType;

  int m_FJCut;
  int m_PVCut;

  int m_minFJ_Thr;
  int m_minPV_Track;
  
  int m_accepted;   //!< number of accepted events
  int m_rejected;   //!< number of rejected events
  int m_errors;     //!< number of errors

  int m_cutCounter;

  TrigTimer*  m_totTime;


};
#endif
