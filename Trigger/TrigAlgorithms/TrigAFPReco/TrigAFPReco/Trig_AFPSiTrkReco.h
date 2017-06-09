/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIG_AFPSITRKRECO_H
#define TRIG_AFPSITRKRECO_H 1

#include <string>

#include "TrigInterfaces/FexAlgo.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AFP_ByteStream2RawCnv/AFP_RawDataProviderTool.h"
#include "AFP_LocRecoInterfaces/IAFPSiDLocRecoTool.h"

#include "AFP_Raw2Digi/IAFP_Raw2DigiTool.h"

class ISvcLocator;

class Trig_AFPSiTrkReco: public HLT::FexAlgo {
public:
  Trig_AFPSiTrkReco(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Trig_AFPSiTrkReco();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  HLT::ErrorCode hltFinalize();
private:
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  ToolHandle<AFP_RawDataProviderTool> m_rawDataTool;
  ToolHandle<IAFP_Raw2DigiTool> m_digiTool;
  ToolHandle<IAFPSiDLocRecoTool> m_trackRecoTool;
  std::string m_rawDataCollectionKey;
  std::string m_siHitContainerName;
  std::string m_trackContainerName;
};

#endif
