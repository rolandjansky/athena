/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFCaloHypoNoise.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigCaloHypo
 *
 * AUTHOR:   Denis Oliveira Damazio
 * CREATED:  2016, January
 *
 *********************************************************************/
#ifndef TRIGEFCALOHYPONOISE_H
#define TRIGEFCALOHYPONOISE_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CaloInterface/ILArNoisyROTool.h"


namespace hltinterface{
  class GenericHLTContainer;
}
class StoreGateSvc;
class TriggerElement;

class TrigEFCaloHypoNoise : public HLT::HypoAlgo {

 public:

  TrigEFCaloHypoNoise(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFCaloHypoNoise();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:

  double     m_EtCut;
  double     m_etaMinCut;
  double     m_etaMaxCut;

  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  bool m_acceptAll;

  // Switch on Monitoring:
  
  bool m_doMonitoring;

  StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;
  unsigned int m_MinBadFEB;

  std::shared_ptr<hltinterface::GenericHLTContainer> m_IsObject;
  size_t m_evntPos;
  size_t m_timeTagPos;
  size_t m_timeTagPosns;
  bool m_isInterface;
  long int m_timeTagPosToClear;
  long int m_timeTagPosRec;
  long int m_publishTime;

  ToolHandle<ILArNoisyROTool> m_noisyROTool;

  // helpers to define what is to be used to save events
  bool m_badFEBFlaggedPartitions;
  bool m_satTightFlaggedPartitions;
  bool m_mNBLooseFlaggedPartitions;
  bool m_mNBTightFlaggedPartitions;
  bool m_mNBTight_PsVetoFlaggedPartitions;
  unsigned int m_mask;

};
#endif

