/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGDETAFEXBASE_H
#define TRIG_TRIGDETAFEXBASE_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigInterfaces/AlgoConfig.h"
#include <string>
#include <vector>

//using HLT::ErrorCode;
//using HLT::TEConstVec;

class ITrigTimerSvc;
class INavigable4Momentum;

class TrigDEtaFexBase : public HLT::AllTEAlgo {
  
public:
  
  TrigDEtaFexBase(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigDEtaFexBase(){};
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> > &inputTEs,
                            unsigned int outType);
  
  
private:
  
  virtual HLT::ErrorCode getJets(const HLT::TriggerElement *te, 
                                 std::vector<const INavigable4Momentum*> &jets) = 0;
  
  virtual HLT::ErrorCode saveJets(HLT::TriggerElement *output, 
                                  std::vector<const INavigable4Momentum*>,
                                         //const INavigable4Momentum *jet, 
                                         std::string label) = 0;
  
  double m_dEtaCut;
  double m_MjjCut;

  double m_EtCut;
  double m_etaMaxCut;
  bool m_checkMostForwardBackwardPair;

  ITrigTimerSvc* m_timersvc;
  std::vector<TrigTimer*>   m_timers;
  std::string m_label;

  int m_nJets;
  int m_nJetsSelected;
  
  double m_gapSize;
  std::vector<double> m_jetEta;

};
#endif
