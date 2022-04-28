/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKANALYSIS_SEEDANALYSIS_H
#define ACTSTRKANALYSIS_SEEDANALYSIS_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "ActsTrkEvent/Seed.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

namespace ActsTrk {

  class SeedAnalysis final :
    public AthMonitorAlgorithm {
  public:
    SeedAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SeedAnalysis() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:
    std::array<float, 7> estimateParameters(const ActsTrk::Seed& seed,
					    float pTPerHelixRadius) const;
      
  private:
    SG::ReadCondHandleKey< InDet::BeamSpotData > m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};
    SG::ReadCondHandleKey< AtlasFieldCacheCondObj > m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
        "Name of the Magnetic Field conditions object key"};

    SG::ReadHandleKey< ActsTrk::SeedContainer > m_inputSeedColletionKey {this,  "InputSeedCollection", "", ""}; 
  };

}

#endif
