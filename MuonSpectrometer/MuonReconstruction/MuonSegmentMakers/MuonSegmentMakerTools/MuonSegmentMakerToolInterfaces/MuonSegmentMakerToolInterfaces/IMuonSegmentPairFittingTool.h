/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTPAITFITTER_H
#define MUON_IMUONSEGMENTPAITFITTER_H

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"


static const InterfaceID IID_IMuonSegmentPairFittingTool("Muon::IMuonSegmentPairFittingTool",1,0);


namespace Muon {
  
  class MuonSegment;

  /**
     @brief tool to fit pairs of segments

  */
  class IMuonSegmentPairFittingTool : virtual public IAlgTool {
  public:
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_IMuonSegmentPairFittingTool; }

  };

}

#endif
