/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKSTEERING_H
#define MUONTRACKSTEERING_H

#include "MuonRecToolInterfaces/IMuonTrackFinder.h"

#include "MuonIdHelpers/MuonStationIndex.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/StatEntity.h"

#include "Identifier/Identifier.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackCollection.h"

#include <vector>
#include <set>
#include <string>
#include <utility>

class MsgStream;

namespace Trk {
  class ITrackFitter;
  class Track;
  class MeasurementBase;
  class ITrackAmbiguityProcessorTool;
  class ITrackSelectorTool;
}

namespace Muon {
  class MuPatSegment;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class MuonTrackSteeringStrategy;
  class MuPatCandidateTool;
  class IMuonTrackBuilder;
  class MuPatTrack;
  class MuonSegmentCombination;
  class MooTrackBuilder;
  class MooCandidateMatchingTool;
  class IMuonTrackRefiner;
  class IMuonSegmentFittingTool;
  class IMuonSegmentMerger;
  class IMuonHoleRecoveryTool;
}

namespace Muon {

  typedef std::vector<const Muon::MuonSegment*> MuonSegmentCollection;

  /** 
      Implementation of an IMuonTrackSteering. 

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
    ~MuonTrackSteering();
    
    /** initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** finialize method, method taken from bass-class AlgTool */
    StatusCode finalize();
    
    /** @brief find tracks starting from a MuonSegmentCollection
    @param coll a reference to a MuonSegmentCollection
    @return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
    */
    TrackCollection* find( const MuonSegmentCollection& coll ) const;

    /** @brief find tracks starting from a MuonSegmentCombination
    @param combi a reference to a MuonSegmentCombination
    @return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
    */
    TrackCollection* find( const MuonSegmentCombination& combi ) const;

    /** @brief find tracks starting from a MuonSegmentCombinationCollection
    @param combiCol a reference to a MuonSegmentCombinationCollection
    @return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
    */
    //std::vector<Trk::Track*>* find( const MuonSegmentCombinationCollection& combiCol ) const;

    //  std::vector<const Trk::Track*>* find( const std::vector<const MuonSegment*>& segments ) const {};

    TrackCollection *selectTracks(std::vector<MuPatTrack*> & candidates, bool takeOwnership = true ) const;

  private:
    /** actual find method */
    TrackCollection* findTracks( ) const;
    bool extractSegments( const MuonSegmentCollection& coll ) const;
    
    void cleanUp() const;
    void init() const;

    StatusCode decodeStrategyVector(const std::vector<std::string>& strategy);
    const MuonTrackSteeringStrategy* decodeStrategy(const std::string& strategy) const;
    bool decodeList(const std::string& input, std::vector<std::string>& list) const;

    std::vector<MuPatTrack*> *extendWithLayer(MuPatTrack& candidate, const SegColVec& segcol, unsigned int nextlayer, const unsigned int endlayer, int cutLevel = 0 ) const;
    /** @brief Find tracks starting from a good segment
    @param seedSeg the seeding MuonSegment pointer
    @param strat the current track finding strategy
    @param layer the current layer for the seed
    */
    std::vector< MuPatTrack* > * findTrackFromSeed( MuPatSegment& seedSeg , const MuonTrackSteeringStrategy & strat , const unsigned int layer , const SegColVec& segs ) const;

    std::vector<MuPatTrack*> * refineTracks(std::vector<MuPatTrack*>& candidates) const;
    
    /** @brief Resolve ambiguities among tracks for a single strategy
    This allows a strategy-specific ambiguity solving (with some options per strategy)
    @param vector of tracks that were found
    @param strat the steering strategy
    */
    std::vector<MuPatTrack*> *solveAmbiguities( std::vector< MuPatTrack* >& tracks , const MuonTrackSteeringStrategy* strat = 0 ) const;

    void combineOverlapSegments( std::vector< MuPatSegment*>& ch1, std::vector< MuPatSegment*>& ch2 ) const;

  private:

    ToolHandle<MuonEDMHelperTool>  m_helper;    //!< Tool for general EDM manipulation
    ToolHandle<MuonEDMPrinterTool>  m_printer;    //!< Tool to print EDM objects
    ToolHandle<MuPatCandidateTool> m_candidateTool; //!< Tool for manipulation of candidates
    ToolHandle<IMuonTrackBuilder>    m_trackBTool;    //<! Tool for helping in track building
    mutable ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiTool; //!< Tool for ambiguity solving
    ToolHandle<MooTrackBuilder> m_mooBTool; //<! Temporary tool for helping to combine two segments
    ToolHandle<MooCandidateMatchingTool> m_candidateMatchingTool;
    ToolHandle<IMuonTrackRefiner> m_trackRefineTool;
    ToolHandle<IMuonSegmentFittingTool>           m_segmentFitter; //<! segment fitting tool
    ToolHandle<IMuonSegmentMerger>                m_segmentMerger; //<! segment merger
    ToolHandle<Trk::ITrackSelectorTool>           m_trackSelector; //<! track selector
    ToolHandle<IMuonHoleRecoveryTool>             m_muonHoleRecoverTool;

    mutable SegColVec m_chamberSegments;      // <! Segments sorted per Chamber
    mutable SegColVec m_stationSegments;      // <! Segments sorted per station
    mutable ChSet     m_chambersWithSegments; // <! Chambers having segments
    mutable StSet     m_stationsWithSegments; // <! Stations having segments

     mutable SegCol m_segmentsToDelete;
     mutable TrkVec m_tracksToDelete;
     mutable TrkCollVec m_trktracksToDelete;
     mutable std::vector<const MuonSegment*>  m_constsegmentsToDelete;
    
    std::vector<const MuonTrackSteeringStrategy*> m_strategies;
    std::vector<std::string> m_stringStrategies;

    int m_segQCut[3]; //!< Required segment quality for seed, 2nd, and other segments
    bool m_outputSingleStationTracks;
    bool m_combinedSLOverlaps;
    bool m_doSummary;
    bool m_useTightMatching;
    bool m_onlyMDTSeeding;
    int m_segThreshold;

    mutable int m_findingDepth;
    mutable int m_seedCombinatorics;
  };

}

#endif
