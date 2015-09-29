/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//ElectronUEMonitoring.h
#ifndef TrigHIHypo_ElectronUEMonitoring_h
#define TrigHIHypo_ElectronUEMonitoring_h

#include "GaudiKernel/SystemOfUnits.h"
using Gaudi::Units::GeV;
using Gaudi::Units::TeV;


#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"


class ElectronUEMonitoring: public HLT::FexAlgo {

 public:
  ElectronUEMonitoring(const std::string& name, ISvcLocator* pSvcLocator);
  ~ElectronUEMonitoring();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,  HLT::TriggerElement* outputTE);

 private:
  std::string m_UECollectionKey;

  float m_EtCutLow;
  float m_EtCutMedium;


  float m_EtEFLow;
  float m_EtL2Low;

  float m_EtDiffLow;

  float m_EtEFMedium;
  float m_EtL2Medium;

  float m_EtDiffMedium;

  float m_EtEFHigh;
  float m_EtL2High;

  float m_EtDiffHigh;

  float m_FcalEt;
};


#endif //Trig_HIHypo_ElectronUEMonitoring_h
