/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFCaloHypoNoise.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
 *
 * AUTHOR:   Denis Oliveira Damazio
 * CREATED:  2012, January
 *
 *********************************************************************/
#ifndef TRIGEFJETHYPONOISE_H
#define TRIGEFJETHYPONOISE_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CaloInterface/ILArNoisyROTool.h"

//#ifdef ONLINEIS
//#include "hltinterface/IInfoRegister.h"
//#endif

namespace hltinterface{
  class GenericHLTContainer;
}
class StoreGateSvc;
class TriggerElement;

class TrigEFJetHypoNoise : public HLT::HypoAlgo {

 public:

  TrigEFJetHypoNoise(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFJetHypoNoise();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:

  double     m_EtCut;
  //double     m_etaMinCut;
  //double     m_etaMaxCut;

  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  bool m_acceptAll;

  // Switch on Monitoring:
  
  bool m_doMonitoring;

  //StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;
  unsigned int m_MinBadFEB;

//#ifdef ONLINEIS
  std::shared_ptr<hltinterface::GenericHLTContainer> m_IsObject;
//#endif
  size_t m_evntPos;
  size_t m_timeTagPos;
  size_t m_timeTagPosns;
  bool m_isInterface;
  long int m_timeTagPosToClear;
  long int m_timeTagPosRec;
  long int m_publishTime;

  ToolHandle<ILArNoisyROTool> m_noisyROTool;

};
#endif

