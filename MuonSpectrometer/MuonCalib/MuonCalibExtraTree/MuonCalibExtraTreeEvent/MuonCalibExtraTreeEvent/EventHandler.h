/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EventHandler_h
#define EventHandler_h

#include "MuonCalibExtraTreeEvent/MuonCalibExtendedRawHits.h"
#include "CxxUtils/checker_macros.h"

#include <map>
#include <string>
#include <vector>

namespace MuonCalib{

  class MuonCalibExtendedTrack;
  class MuonCalibExtendedSegment;
  class MuonCalibEvent_E;

  /**
     @author Niels.van.eldik@cern.ch
  */
  class ATLAS_NOT_THREAD_SAFE EventHandler {

  public:
    struct AuthorIndices {
      AuthorIndices( int s, int t ) : segment(s),track(t) {}
      int segment;
      int track;
    };

  public:
    typedef std::vector<const MuonCalibExtendedTrack*> TrackVec;
    typedef TrackVec::iterator                         TrkIt;
    typedef TrackVec::const_iterator                   TrkCit;
    typedef std::map< int, TrackVec >                  AuthorTrackVecMap;
    typedef AuthorTrackVecMap::iterator                AuthorTrackVecIt;

    typedef std::vector<const MuonCalibExtendedSegment*> SegmentVec;
    typedef SegmentVec::iterator                         SegIt;
    typedef SegmentVec::const_iterator                   SegCit;
    typedef std::map< int, SegmentVec >                  AuthorSegmentVecMap;
    typedef AuthorSegmentVecMap::iterator                AuthorSegmentVecIt;
  public:
    /** default constructor */
    EventHandler();

    /** default destructor */
    ~EventHandler();
    
    /** clear all buffers */
    void clear();

    /** set a new event (will call clear on the previous) */
    void setEvent( const MuonCalibEvent_E& event );

    /** get extended tracks */
    const TrackVec& extendedTracks( int author ) const { return m_extendedTracks[author]; }

    /** get extended tracks */
    const SegmentVec& extendedSegments( int author ) const { return m_extendedSegments[author]; }

    /** get extended raw hits */
    const MuonCalibExtendedRawHits& extendedRawHits() const { return m_extendedRawHits; }

    /** get event */
    const MuonCalibEvent_E& event() const { return *m_event; }

    /** get author indices for Moore */
    const AuthorIndices& mooreIndices() const { return m_mooreIndices; }

    /** get author indices for Muonboy */
    const AuthorIndices& muonboyIndices() const { return m_muonboyIndices; }
    
    /** dump routines */
    std::string dumpRawHits() const;
    std::string dumpTracks() const;
    std::string dumpTracks( const TrackVec& tracks ) const;
    std::string dumpSegments( const SegmentVec& segments ) const;
    std::string dumpSegments() const;
    std::string dumpEvent() const;
    void setdbg(const bool dbg_on);

  private:
    /** create extended tracks for event */
    void createExtendedTracks();

    /** create extended tracks for event */
    void createExtendedTruthTracks();

    /** create extended segments for event */
    void createExtendedSegments();

    /** create extend raw hits */
    void createExtendedRawHits();

    /** create association between tracks and segments */
    void associateSegmentsWithTracks( SegmentVec& segments, TrackVec& tracks );

    /** check whether segment and track share the same hits */
    int associateSegmentWithTrack( const MuonCalibExtendedSegment& segment, const MuonCalibExtendedTrack& track );

    /** check whether segments  share the same hits */
    int segmentOverlap( const MuonCalibExtendedSegment& segment1, const MuonCalibExtendedSegment& segment2 );

    /** link tracks to the reference track using the hits on the track */
    void linkTracks( int referenceIndex );

    /** find best match of tracks with reference */
    void match( const MuonCalibExtendedTrack& reference, TrackVec& tracks );


    // data members
    const MuonCalibEvent_E* m_event; 
    mutable AuthorTrackVecMap        m_extendedTracks;
    mutable AuthorSegmentVecMap      m_extendedSegments;
    mutable MuonCalibExtendedRawHits m_extendedRawHits;

    // author indices of segment and tracks in ntuple
    AuthorIndices m_mooreIndices;
    AuthorIndices m_muonboyIndices;
    bool m_debug;

  };

}//namespace MuonCalib
#endif
