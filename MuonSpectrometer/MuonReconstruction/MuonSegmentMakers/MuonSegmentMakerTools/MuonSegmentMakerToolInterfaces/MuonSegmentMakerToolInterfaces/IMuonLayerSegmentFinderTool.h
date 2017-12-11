/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONLAYERSEGMENTFINDERTOOL_H
#define MUON_IMUONLAYERSEGMENTFINDERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonLayerEvent/MuonLayerPrepRawData.h"

static const InterfaceID IID_IMuonLayerSegmentFinderTool("Muon::IMuonLayerSegmentFinderTool",1,0);


namespace Muon {

  class MuonSegment;

  /** Interface for tools calculating hit count summaries for track */
  class IMuonLayerSegmentFinderTool : virtual public IAlgTool {     
  public:
    /** IAlgTool interface */
    static const InterfaceID& interfaceID();

    /** @brief Find segments for a given MuonSystemExtension::Intersection */
    virtual void find( const MuonSystemExtension::Intersection& intersection, std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments, 
		       MuonLayerPrepRawData& layerPrepRawData) const = 0;

  };
 
  inline const InterfaceID& IMuonLayerSegmentFinderTool::interfaceID()
  {
    return IID_IMuonLayerSegmentFinderTool;
  }


} // end of name space

#endif // IMuonLayerSegmentFinderTool_H
