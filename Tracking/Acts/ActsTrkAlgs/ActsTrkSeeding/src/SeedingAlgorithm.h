/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKSEEDING_SEEDINGALGORITHM_H
#define ACTSTRKSEEDING_SEEDINGALGORITHM_H 1
  
// Base Class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Tools
#include "ActsTrkToolInterfaces/ISeedingTool.h"

// Athena
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

// Handle Keys
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKey.h"

namespace ActsTrk {

  class SeedingAlgorithm :
    public AthReentrantAlgorithm {
    
  public:
    SeedingAlgorithm(const std::string &name,
			 ISvcLocator *pSvcLocator);
    virtual ~SeedingAlgorithm() = default;

    
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    // Tool Handles
    ToolHandle< ActsTrk::ISeedingTool > m_seedsTool {this, "SeedTool", "","Seed Tool"};

    // Handle Keys
    SG::ReadCondHandleKey< InDet::BeamSpotData > m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};
    SG::ReadCondHandleKey< AtlasFieldCacheCondObj > m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
        "Name of the Magnetic Field conditions object key"};

    SG::ReadHandleKeyArray< ActsTrk::SpacePointContainer > m_spacePointKey {this,"InputSpacePoints",{},"Input Acts Space Points"};
    SG::WriteHandleKey< ActsTrk::SeedContainer > m_seedKey {this,"OutputSeeds","","Output Seeds"};    
  };
  
} // namespace

#endif
