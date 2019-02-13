/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigMuonEF_SegmentCache_H__
#define TrigMuonEF_SegmentCache_H__

#include <vector>
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackCollection.h"


class TrigMuonEFInfo;
class SegmentCache  {
  
 public:
  /// Default Constructor
  SegmentCache();
  
  /// Destructor
  ~SegmentCache();
  
  
  //void AddMuonInfo(TrigMuonEFInfo* MI);
  void AddPatternCollection(const MuonPatternCombinationCollection* pat);
  void AddSegments(const Trk::SegmentCollection* trk);
  void SetNROI(int n);
  void SetSpectrometerTrackCollection( const TrackCollection* msTracks );
  void SetExtrapolatedTrackCollection( TrackCollection* saTracks );
  const MuonPatternCombinationCollection* PattColl() const {return m_pattcoll;}
  const Trk::SegmentCollection* Segments() const {return m_segments;}
  int NROI() const {return m_nroi;}
  const TrackCollection* SpectrometerTrackColl() { return m_msTracks;}
  TrackCollection* ExtrapolatedTrackColl() { return m_saTracks;}

 private:
  
  const MuonPatternCombinationCollection* m_pattcoll;
  const Trk::SegmentCollection* m_segments;   
  const TrackCollection* m_msTracks;
  TrackCollection* m_saTracks;
  int m_nroi;
};

#endif
