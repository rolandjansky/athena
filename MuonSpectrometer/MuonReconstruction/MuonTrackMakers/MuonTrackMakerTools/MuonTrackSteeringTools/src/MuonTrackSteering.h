/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKSTEERING_H
#define MUONTRACKSTEERING_H

#include "MuonRecToolInterfaces/IMuonTrackFinder.h"

#include "MuonStationIndex/MuonStationIndex.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatEntity.h"

#include "Identifier/Identifier.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackCollection.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentInOverlapResolvingTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMerger.h"
#include "MuonRecToolInterfaces/IMuonTrackBuilder.h"
#include "MuonRecToolInterfaces/IMuonSegmentFittingTool.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuPatCandidateTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "MooTrackBuilder.h"
#include "MooCandidateMatchingTool.h"
#include "MuonTrackSelectorTool.h"

#include "TrkTrack/Track.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

#include <mutex>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace Muon {
  class MuPatSegment;
  class MuonTrackSteeringStrategy;
  class MuPatTrack;
}

namespace Trk {
  class Track;
  class IExtendedTrackSummaryTool;
}
namespace Muon {

  typedef std::vector<const Muon::MuonSegment*> MuonSegmentCollection;

  /**
      Implementation of an IMuonTrackFinder.

      For more details look at the mainpage of this package.
  */
  class MuonTrackSteering :  virtual public IMuonTrackFinder, public AthAlgTool   {
  public:
    typedef std::vector<MuPatTrack*>             TrkVec;
    typedef std::vector<Trk::Track*>             TrkCollVec;
    typedef std::vector<MuPatSegment*>           SegCol;
    typedef std::vector<MuPatSegment*>::iterator SegColIt;
    typedef std::vector< SegCol >                SegColVec;
    typedef SegColVec::iterator                  SegColVecIt;
    typedef SegColVec::const_iterator            SegColVecCit;

    typedef std::set<MuonStationIndex::ChIndex> ChSet;
    typedef ChSet::iterator                     ChIt;
    typedef ChSet::const_iterator               ChCit;

    typedef std::set<MuonStationIndex::StIndex> StSet;
    typedef StSet::iterator                     StIt;
    typedef StSet::const_iterator               StCit;

  public:
    /** default AlgTool constructor */
    MuonTrackSteering(const std::string&, const std::string&, const IInterface*);

    /** destructor */
    virtual ~MuonTrackSteering() = default;

    /** initialize method, method taken from bass-class AlgTool */
    virtual StatusCode initialize() override;

    /** finialize method, method taken from bass-class AlgTool */
    virtual StatusCode finalize() override;

    /** @brief find tracks starting from a MuonSegmentCollection
        @param coll a reference to a MuonSegmentCollection
        @return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
    */
    TrackCollection* find( const MuonSegmentCollection& coll ) const override;

  private:
    TrackCollection *selectTracks(std::vector<std::unique_ptr<MuPatTrack> > & candidates, bool takeOwnership = true ) const;

    /** actual find method */
    TrackCollection* findTracks( SegColVec& chamberSegments, SegColVec& stationSegments ) const;
    bool extractSegments( const MuonSegmentCollection& coll, SegColVec& chamberSegments, SegColVec& stationSegments, ChSet&  chambersWithSegments, StSet& stationsWithSegments  ) const;

    void cleanUp() const;

    StatusCode decodeStrategyVector(const std::vector<std::string>& strategy);
    const MuonTrackSteeringStrategy* decodeStrategy(const std::string& strategy) const;
    bool decodeList(const std::string& input, std::vector<std::string>& list) const;

    std::vector<std::unique_ptr<MuPatTrack> > extendWithLayer(MuPatTrack& candidate, const SegColVec& segcol, unsigned int nextlayer, const unsigned int endlayer, int cutLevel = 0 ) const;
    /** @brief Find tracks starting from a good segment
        @param seedSeg the seeding MuonSegment pointer
        @param strat the current track finding strategy
        @param layer the current layer for the seed
    */
    std::vector< std::unique_ptr<MuPatTrack> > findTrackFromSeed( MuPatSegment& seedSeg , const MuonTrackSteeringStrategy & strat , const unsigned int layer , const SegColVec& segs ) const;

    void refineTracks(std::vector<std::unique_ptr<MuPatTrack> >& candidates) const;

    /** @brief Resolve ambiguities among tracks for a single strategy
               This allows a strategy-specific ambiguity solving (with some options per strategy)
        @param vector of tracks that were found
        @param strat the steering strategy
    */
    void solveAmbiguities( std::vector< std::unique_ptr<MuPatTrack> >& tracks , const MuonTrackSteeringStrategy* strat = 0 ) const;

    void combineOverlapSegments( std::vector< MuPatSegment*>& ch1, std::vector< MuPatSegment*>& ch2, SegColVec& stationSegments, StSet& stationsWithSegments ) const;

  private:

    ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper",
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
      "Handle to the service providing the IMuonEDMHelperSvc interface" };    //!< Tool for general EDM manipulation
    ToolHandle<MuonEDMPrinterTool>        m_printer
      {this, "MuonPrinterTool", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};            //<! tool to print EDM objects
    ToolHandle<MuPatCandidateTool>        m_candidateTool
      {this, "MuPatCandidateTool", "Muon::MuPatCandidateTool/MuPatCandidateTool"};
    ToolHandle<IMuonTrackBuilder>        m_trackBTool
      {this, "TrackBuilderTool", "Muon::MooTrackBuilder/MooMuonTrackBuilder"};
    ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiTool
      {this, "AmbiguityTool", "Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"};
    ToolHandle<MooTrackBuilder> m_mooBTool
      {this, "MooBuilderTool", "Muon::MooTrackBuilder/MooMuonTrackBuilder"};//<! Temporary tool for helping to combine two segments
    ToolHandle<MooCandidateMatchingTool> m_candidateMatchingTool
      {this, "CandidateMatchingTool", "Muon::MooCandidateMatchingTool/MooCandidateMatchingTool"};//<! Temporary tool for helping to combine two segments
    ToolHandle<IMuonTrackRefiner> m_trackRefineTool
      {this, "TrackRefinementTool", "Muon::MooTrackBuilder/MooMuonTrackBuilder"};//<! Temporary tool for helping to combine two segments
    ToolHandle<IMuonSegmentFittingTool> m_segmentFitter
      {this, "MuonSegmentFittingTool", "Muon::MuonSegmentFittingTool/MuonSegmentFittingTool"};//<! segment fitting tool
    ToolHandle<IMuonSegmentMerger> m_segmentMerger
      {this, "MuonSegmentMerger", ""};//<! segment merger
    ToolHandle<Muon::MuonTrackSelectorTool> m_trackSelector
      {this, "MuonTrackSelector", "Muon::MuonTrackSelectorTool/MuonTrackSelectorTool"};//<! track selector
    ToolHandle<IMuonHoleRecoveryTool> m_muonHoleRecoverTool
      {this, "HoleRecoveryTool", "Muon::MuonChamberHoleRecoveryTool/MuonChamberHoleRecoveryTool"};//<! track selector
    ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool    
      {this, "TrackSummaryTool", "MuonTrackSummaryTool"};

    mutable SegCol m_segmentsToDelete ATLAS_THREAD_SAFE;
    mutable std::vector<const MuonSegment*>  m_constsegmentsToDelete ATLAS_THREAD_SAFE;
    mutable std::mutex m_segmentsMutex;
    mutable std::mutex m_constSegmentsMutex;

    std::vector<const MuonTrackSteeringStrategy*> m_strategies;
    std::vector<std::string> m_stringStrategies;

    int m_segQCut[3]; //!< Required segment quality for seed, 2nd, and other segments
    bool m_outputSingleStationTracks;
    bool m_combinedSLOverlaps;
    bool m_doSummary;
    bool m_useTightMatching;
    bool m_onlyMDTSeeding;
    int m_segThreshold;

    mutable std::atomic_uint m_findingDepth {0};
    mutable std::atomic_uint m_seedCombinatorics {0};
  };

}

#endif
