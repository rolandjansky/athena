/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONLAYERAMBIGUITYSOLVERTOOL_H
#define MUON_IMUONLAYERAMBIGUITYSOLVERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonLayerEvent/MuonCandidate.h"

static const InterfaceID IID_IMuonLayerAmbiguitySolverTool("Muon::IMuonLayerAmbiguitySolverTool",1,0);


namespace Muon {

  class MuonSegment;

  /** Interface for tools calculating hit count summaries for track */
  class IMuonLayerAmbiguitySolverTool : virtual public IAlgTool {     
  public:
    /** IAlgTool interface */
    static const InterfaceID& interfaceID();

    /** @brief resolve ambiguities and return a set of unique combinations */
    virtual void resolveOverlaps( const std::vector<Muon::MuonLayerRecoData>& allLayers, std::vector< MuonCandidate >& resolvedCandidates ) const = 0;

    virtual void cleanUp() const = 0;
  };
 
  inline const InterfaceID& IMuonLayerAmbiguitySolverTool::interfaceID()
  {
    return IID_IMuonLayerAmbiguitySolverTool;
  }


} // end of name space

#endif // IMuonLayerAmbiguitySolverTool_H
