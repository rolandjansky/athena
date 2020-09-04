/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERAMBIGUITYSOLVERTOOL_H
#define MUON_MUONLAYERAMBIGUITYSOLVERTOOL_H

#include "MuonCombinedToolInterfaces/IMuonLayerAmbiguitySolverTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMatchingTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentTrackBuilder.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonLayerEvent/MuonCandidate.h"
#include "MuonStationIndex/MuonStationIndex.h"

#include <vector>

namespace Muon {
  struct MuonCandidate;

  class MuonLayerAmbiguitySolverTool : virtual public IMuonLayerAmbiguitySolverTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonLayerAmbiguitySolverTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonLayerAmbiguitySolverTool()=default;
    StatusCode initialize();

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

    ToolHandle<IMuonSegmentSelectionTool> m_segmentSelector{this,"MuonSegmentSelectionTool","Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool"};
    ToolHandle<IMuonSegmentMatchingTool> m_segmentMatchingTool{this,"MuonSegmentMatchingTool","Muon::MuonSegmentMatchingTool/MuonSegmentMatchingToolTight"};
    ToolHandle<IMuonSegmentTrackBuilder> m_muonTrackBuilder{this,"MuonSegmentTrackBuilder","Muon::MooTrackBuilder/MooMuonTrackBuilder"};
    ToolHandle<MuonEDMPrinterTool> m_printer{this,"MuonEDMPrinterTool","Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};

    Gaudi::Property<unsigned int> m_maxSeeds {this, "MaxSeeds", 30, "maximum number of seeds to be tried"};
    Gaudi::Property<int> m_seedQualityThreshold {this, "SeedQualityThreshold", 2, "seed quality threshold"};
    Gaudi::Property<int> m_minSegmentQuality {this, "MinimumSegmentQuality", 1, "minimum quality for a segment to be considered"};

  };
}



#endif
 

