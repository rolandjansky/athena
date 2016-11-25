/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERAMBIGUITYSOLVERTOOL_H
#define MUON_MUONLAYERAMBIGUITYSOLVERTOOL_H

#include "MuonCombinedToolInterfaces/IMuonLayerAmbiguitySolverTool.h"

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonLayerEvent/MuonCandidate.h"

namespace Muon {

  class IMuonSegmentSelectionTool;
  class IMuonSegmentMatchingTool;
  class IMuonSegmentTrackBuilder;
  class MuonEDMPrinterTool;
  struct MuonCandidate;

  class MuonLayerAmbiguitySolverTool : virtual public IMuonLayerAmbiguitySolverTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonLayerAmbiguitySolverTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonLayerAmbiguitySolverTool();
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonLayerAmbiguitySolverTool interface: find */   
    void resolveOverlaps( const std::vector<Muon::MuonLayerRecoData>& allLayers, std::vector< MuonCandidate >& resolvedCandidates ) const;

  private:
    void buildLayerVec( const std::vector<MuonLayerRecoData>& allLayers,  
                        std::vector< std::vector<MuonLayerIntersection> >& muonLayerDataHashVec ) const;

    bool getNextSeed( const std::vector< std::vector<MuonLayerIntersection> >& muonLayerDataHashVec, 
                      std::set<const MuonSegment*>& usedSegments, 
                      std::vector<MuonStationIndex::StIndex>& inverseSeedLayerOrder,
                      MuonLayerIntersection& layerIntersection ) const;

    bool extendCandidatesWithLayers( std::vector<MuonCandidate>& candidates, 
                                     const std::vector< std::vector<MuonLayerIntersection> >& muonLayerRecoDataHashVec,
                                     const std::vector<MuonStationIndex::StIndex>& inverseSeedLayerOrder ) const;

    void resolveSmallLargeOverlaps( std::vector<MuonLayerIntersection>& existingLayerIntersections, 
                                    const std::vector<MuonLayerIntersection>& newLayerIntersections ) const;

    bool match( const MuonCandidate& candidate, const MuonLayerIntersection& layerIntersection ) const;


    unsigned int m_maxSeeds; // maximum number of seeds to be tried
    int          m_seedQualityThreshold; // seed quality threshold
    int          m_minSegmentQuality; // minimum quality for a segment to be considered
    ToolHandle<IMuonSegmentSelectionTool> m_segmentSelector;
    ToolHandle<IMuonSegmentMatchingTool>  m_segmentMatchingTool;
    ToolHandle<IMuonSegmentTrackBuilder>  m_muonTrackBuilder; 
    ToolHandle<MuonEDMPrinterTool>        m_printer;
  };
}



#endif
 

