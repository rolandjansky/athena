/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONINSIDEOUTRECOTOOL_H
#define MUON_MUONINSIDEOUTRECOTOOL_H

#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonPrepRawDataProviderTools/MuonLayerHashProviderTool.h"
#include "xAODTracking/VertexContainer.h"



#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonSegmentMakerToolInterfaces/IMuonLayerSegmentFinderTool.h"
#include "MuonCombinedToolInterfaces/IMuonLayerSegmentMatchingTool.h"
#include "MuonCombinedToolInterfaces/IMuonLayerAmbiguitySolverTool.h"
#include "MuonCombinedToolInterfaces/IMuonCandidateTrackBuilderTool.h"
#include "MuonRecToolInterfaces/IMuonRecoValidationTool.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

namespace Muon {
  struct MuonCandidate;
  struct MuonLayerPrepRawData;
}

namespace Trk {
  class Track;
  class IExtendedTrackSummaryTool;
}

static const InterfaceID IID_MuonInsideOutRecoTool("MuonCombined::MuonInsideOutRecoTool",1,0);

namespace MuonCombined { 

  class MuonInsideOutRecoTool : virtual public MuonCombined::IMuonCombinedInDetExtensionTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonInsideOutRecoTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonInsideOutRecoTool()=default;
    virtual StatusCode initialize() override;

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonInsideOutRecoTool; }

    /**IMuonCombinedInDetExtensionTool interface: extend ID candidate with PRDs for segment-finding */   
    virtual void extendWithPRDs( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, IMuonCombinedInDetExtensionTool::MuonPrdData prdData,
				 TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments) override;

    /**IMuonCombinedInDetExtensionTool interface: deprecated*/
    virtual void extend(const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks, TrackCollection* meTracks,
			Trk::SegmentCollection* segments) override;

    /** find the best candidate for a given set of segments */
    std::pair<std::unique_ptr<const Muon::MuonCandidate>,Trk::Track*> 
    findBestCandidate( const xAOD::TrackParticle& indetTrackParticle, const std::vector< Muon::MuonLayerRecoData >& allLayers);

  private:
    /** handle a single candidate */
    void handleCandidate( const InDetCandidate& inDetCandidate, InDetCandidateToTagMap* tagMap, IMuonCombinedInDetExtensionTool::MuonPrdData prdData,
			  TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments);

    /** add muon candidate to indet candidate */
    void addTag( const InDetCandidate& indetCandidate, InDetCandidateToTagMap* tagMap, 
		 const Muon::MuonCandidate& candidate, Trk::Track* selectedTrack, TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments ) const;

    /** access data in layer */
    bool getLayerData( int sector, Muon::MuonStationIndex::DetectorRegionIndex regionIndex, Muon::MuonStationIndex::LayerIndex layerIndex, 
		       Muon::MuonLayerPrepRawData& layerPrepRawData, IMuonCombinedInDetExtensionTool::MuonPrdData prdData);

    /** access data in layer for a given technology */
    template<class COL>
      bool getLayerDataTech( int sector, Muon::MuonStationIndex::TechnologyIndex technology, Muon::MuonStationIndex::DetectorRegionIndex regionIndex,
			     Muon::MuonStationIndex::LayerIndex layerIndex, const Muon::MuonPrepDataContainer< COL >* input, std::vector<const COL*>& output );

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer; 
    ToolHandle<Muon::IMuonLayerSegmentFinderTool>    m_segmentFinder;
    ToolHandle<Muon::IMuonLayerSegmentMatchingTool>  m_segmentMatchingTool;
    ToolHandle<Muon::IMuonLayerAmbiguitySolverTool>  m_ambiguityResolver;
    ToolHandle<Muon::IMuonCandidateTrackBuilderTool> m_candidateTrackBuilder;
    ToolHandle<Muon::IMuonRecoValidationTool>        m_recoValidationTool;
    ToolHandle<Rec::ICombinedMuonTrackBuilder>       m_trackFitter;
    ToolHandle<Trk::ITrackAmbiguityProcessorTool>    m_trackAmbiguityResolver;
    ToolHandle<Muon::MuonLayerHashProviderTool>      m_layerHashProvider;
    ToolHandle<Trk::IExtendedTrackSummaryTool>       m_trackSummaryTool;    
    
    /** id pt cut */
    double m_idTrackMinPt;
    bool m_ignoreSiAssocated;

    // vertex container key
    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey { this, "VertexContainer", "PrimaryVertices", "vertex container key" };
  };
}



#endif
 


