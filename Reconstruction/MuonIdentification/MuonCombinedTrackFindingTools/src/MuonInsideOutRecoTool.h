/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONINSIDEOUTRECOTOOL_H
#define MUON_MUONINSIDEOUTRECOTOOL_H

#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonPrepRawDataProviderTools/MuonLayerHashProviderTool.h"
#include "xAODTracking/VertexContainer.h"
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
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include <string>
#include <vector>

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
				 TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments) const override;

    /**IMuonCombinedInDetExtensionTool interface: deprecated*/
    virtual void extend(const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks, TrackCollection* meTracks,
			Trk::SegmentCollection* segments) const override;

    /** find the best candidate for a given set of segments */
    std::pair<std::unique_ptr<const Muon::MuonCandidate>,Trk::Track*> 
    findBestCandidate( const xAOD::TrackParticle& indetTrackParticle, const std::vector< Muon::MuonLayerRecoData >& allLayers) const;
    
    void cleanUp() const;

  private:
    /** handle a single candidate */
    void handleCandidate( const InDetCandidate& inDetCandidate, InDetCandidateToTagMap* tagMap, IMuonCombinedInDetExtensionTool::MuonPrdData prdData,
			  TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments) const;

    /** add muon candidate to indet candidate */
    void addTag( const InDetCandidate& indetCandidate, InDetCandidateToTagMap* tagMap, 
		 const Muon::MuonCandidate& candidate, Trk::Track* selectedTrack, TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments ) const;

    /** access data in layer */
    bool getLayerData( int sector, Muon::MuonStationIndex::DetectorRegionIndex regionIndex, Muon::MuonStationIndex::LayerIndex layerIndex, 
		       Muon::MuonLayerPrepRawData& layerPrepRawData, IMuonCombinedInDetExtensionTool::MuonPrdData prdData) const;

    /** access data in layer for a given technology */
    template<class COL>
      bool getLayerDataTech( int sector, Muon::MuonStationIndex::TechnologyIndex technology, Muon::MuonStationIndex::DetectorRegionIndex regionIndex,
			     Muon::MuonStationIndex::LayerIndex layerIndex, const Muon::MuonPrepDataContainer< COL >* input, std::vector<const COL*>& output ) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this,"MuonEDMPrinterTool","Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
    ToolHandle<Muon::IMuonLayerSegmentFinderTool>    m_segmentFinder{this,"MuonLayerSegmentFinderTool","Muon::MuonLayerSegmentFinderTool/MuonLayerSegmentFinderTool"};
    ToolHandle<Muon::IMuonLayerSegmentMatchingTool>  m_segmentMatchingTool{this,"MuonLayerSegmentMatchingTool","Muon::MuonLayerSegmentMatchingTool/MuonLayerSegmentMatchingTool"};
    ToolHandle<Muon::IMuonLayerAmbiguitySolverTool>  m_ambiguityResolver{this,"MuonLayerAmbiguitySolverTool","Muon::MuonLayerAmbiguitySolverTool/MuonLayerAmbiguitySolverTool"};
    ToolHandle<Muon::IMuonCandidateTrackBuilderTool> m_candidateTrackBuilder{this,"MuonCandidateTrackBuilderTool","Muon::MuonCandidateTrackBuilderTool/MuonCandidateTrackBuilderTool"};
    ToolHandle<Muon::IMuonRecoValidationTool>        m_recoValidationTool{this,"MuonRecoValidationTool",""};
    ToolHandle<Rec::ICombinedMuonTrackBuilder>       m_trackFitter{this,"MuonTrackBuilder","Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder"};
    ToolHandle<Trk::ITrackAmbiguityProcessorTool>    m_trackAmbiguityResolver{this,"TrackAmbiguityProcessor","Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"};
    ToolHandle<Muon::MuonLayerHashProviderTool>      m_layerHashProvider{this,"MuonLayerHashProviderTool","Muon::MuonLayerHashProviderTool"};
    ToolHandle<Trk::IExtendedTrackSummaryTool>       m_trackSummaryTool{this,"TrackSummaryTool","MuonTrackSummaryTool"};
    
    Gaudi::Property<double> m_idTrackMinPt{this,"IDTrackMinPt",2500};
    Gaudi::Property<bool> m_ignoreSiAssocated{this,"IgnoreSiAssociatedCandidates",true};

    // vertex container key
    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey { this, "VertexContainer", "PrimaryVertices", "vertex container key" };
  };
}



#endif
 


