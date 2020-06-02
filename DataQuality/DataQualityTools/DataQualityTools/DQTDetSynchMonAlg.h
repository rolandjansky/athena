/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTDetSynchMonAlg.h
// PACKAGE:  DataQualityTools
//
// AUTHOR:   Peter Onyisi
//           following original by Luca Fiorini <Luca.Fiorini@cern.ch>
//
//
// ********************************************************************
#ifndef DQTDetSynchMonAlg_H
#define DQTDetSynchMonAlg_H

#include <set>
#include "GaudiKernel/ToolHandle.h"
#include <stdint.h>
#include "TH1.h"
#include "TH2.h"
#include "LWHists/TProfile_LW.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "TileEvent/TileDigitsContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "MuonRDO/RpcPadContainer.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"


class TProfile;

class DQTDetSynchMonAlg: public AthMonitorAlgorithm
{

 public:

  DQTDetSynchMonAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DQTDetSynchMonAlg();

  virtual StatusCode initialize() override;

  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  uint32_t findid(const std::multiset<uint32_t>& mset) const;
  float findfrac(const std::multiset<uint32_t>& mset, uint16_t ctpid) const;

private:

  Gaudi::Property<Int_t> m_solenoidPositionX { this, "SolenoidPositionX", 0 };
  Gaudi::Property<Int_t> m_solenoidPositionY { this, "SolenoidPositionY", 10 };
  Gaudi::Property<Int_t> m_solenoidPositionZ { this, "SolenoidPositionZ", 0 };
  Gaudi::Property<Int_t> m_toroidPositionX { this, "ToroidPositionX", 0 };
  Gaudi::Property<Int_t> m_toroidPositionY { this, "ToroidPositionY", 6000 };
  Gaudi::Property<Int_t> m_toroidPositionZ { this, "ToroidPositionZ", 0 };
  Gaudi::Property<bool> m_doRPC { this, "doRPC", true };
  Gaudi::Property<bool> m_run2Compat { this, "run2Compat", false, "Make histograms with Run 2 naming" };

  // some strings so we don't need to do string manipulation every event
  static const int NDETS = 7;
  std::vector<std::string> m_diffnamevec;

  // storegate keys
  SG::ReadHandleKeyArray<InDetTimeCollection> m_InDetTimeCollectionKeys
    { this, "InDetTimeCollectionKeys", {"TRT_BCID", "SCT_BCID", "PixelBCID", "TRT_LVL1ID", "SCT_LVL1ID", "PixelLVL1ID"} } ;
  SG::ReadHandleKey<LArFebHeaderContainer> m_LArFebHeaderContainerKey
    { this, "LArFebHeaderKey", "LArFebHeader" };
  SG::ReadHandleKey<TileDigitsContainer> m_TileDigitsContainerKey
    { this, "TileDigitsContainerKey", "TileDigitsFlt" };
  SG::ReadHandleKey<RpcPadContainer> m_RpcPadContainerKey
    { this, "RpcPadContainerKey", "RPCPAD" };
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey{
    this,
    "AtlasFieldCacheCondObj",
    "fieldCondObj",
    "Name of the Magnetic Field conditions object key"
  };

};

#endif
