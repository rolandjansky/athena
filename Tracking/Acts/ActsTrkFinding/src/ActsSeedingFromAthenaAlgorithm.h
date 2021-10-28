/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRACKFINDING_ACTSSEEDINGFROMATHENAALGORITHM_H
#define ACTSTRACKFINDING_ACTSSEEDINGFROMATHENAALGORITHM_H 1
  
// Base Class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Tools
#include "ActsTrkFinding/IActsSeedingTool.h"

// Athena
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
#include "TrkSpacePoint/SpacePointContainer.h"

// Handle Keys
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

namespace ActsTrk {

  class ActsSeedingFromAthenaAlgorithm :
    public AthReentrantAlgorithm {
    
  public:
    ActsSeedingFromAthenaAlgorithm(const std::string &name, 
				   ISvcLocator *pSvcLocator);
    virtual ~ActsSeedingFromAthenaAlgorithm() = default;

    
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    // Tool Handles
    ToolHandle< ActsTrk::IActsSeedingTool > m_seedsTool {this, "SeedTool", "","Seed Tool"};

    // Handle Keys
    SG::ReadCondHandleKey< InDet::BeamSpotData > m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};
    SG::ReadCondHandleKey< AtlasFieldCacheCondObj > m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
        "Name of the Magnetic Field conditions object key"};

    SG::ReadHandleKey< ::SpacePointContainer > m_spacePointKey {this,"InputSpacePoints","","Input Athena Space Points"};
    SG::WriteHandleKey< ActsTrk::SeedContainer > m_seedKey {this,"OutputSeeds","","Output Seeds"};    
    SG::WriteHandleKey< ActsTrk::SpacePointContainer > m_actsSpacePointKey {this,"OutputSpacePoints","","Output Space Points"};
    SG::WriteHandleKey< ActsTrk::SpacePointData > m_actsSpacePointDataKey {this,"OutputSpacePointData","","Output Space Points"};
  };
  
} // namespace

#endif
