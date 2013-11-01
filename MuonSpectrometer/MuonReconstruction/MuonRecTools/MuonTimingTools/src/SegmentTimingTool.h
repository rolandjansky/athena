/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SEGMENTTIMINGTOOL_H
#define SEGMENTTIMINGTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkSpaceTimePoint/SpaceTimePoint.h"

#include "TrkSegment/SegmentCollection.h"

#include "TrkToolInterfaces/ITrackTimingTool.h"

class AtlasDetectorID;            //!< to identify measurements

/** @class SegmentTimingTool
    
    SegmentTimingTool matches muon segments to a track and then provides a list os SpaceTimePoints 
    corresponding to the global position and t0 offset of these segments. To be used by another tool 
    for cosmic ray filtering applications

    The choice was made to organise the track<->segment matching in this tool,
    and a SegmentCollection added to the track interface, so that differences
    between the track's and segments' L-R solutions can be taken as additional
    estimate for the time measurements reliability.

    @author Tomo Lazovich <tomo.lazovich@cern.ch>
*/

#define FLAG -88888.0

namespace MuonCalib
{
  class IIdToFixedIdTool;
}

namespace Muon{

  class MuonSegment;
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
  
  class SegmentTimingTool : virtual public Trk::ITrackTimingTool, public AthAlgTool{
  public:
    SegmentTimingTool(const std::string&, const std::string&, const IInterface*);

    /** default destructor **/
    virtual ~SegmentTimingTool();

    /** standard initialization method **/
    virtual StatusCode initialize();
    
    /** standard finalization method **/
    virtual StatusCode finalize();

    /** Returns a list of SpaceTimePoints corresponding to segments that match
        with the input track, where a match is specified by the jobOption
        MatchThreshold. At least MatchThreshold fraction of the segment's hits
        must be on track in order for the segment to be matched.
    */
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const Trk::Track& trk,
                 const Trk::SegmentCollection* ) const;

    /** unsupported interface ? */
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const Trk::TrackParticleBase& trk) const;

    /** Returns a list of SpaceTimePoints corresponding to the segments given */
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(std::vector<const Trk::Segment*> segments) const;

  private:
    // Returns the number of matched segments to a track if the matching conditions are satisfied
    // (MatchThreshold cut), otherwise returns 0.
    int matchSegmentTrk(const Trk::Track& origTrk, const Trk::Segment*& seg) const ;

    //Return the number of common hits in two segments
    bool GetNumberOfCommonHits(const Trk::Segment*& seg1, const Trk::Segment*& seg2) const;

    float getError(const Muon::MuonSegment*& seg) const ; 

    // CSC specific timing code
    void MuonboyT0CSCSegment(const Muon::MuonSegment*& pMuonSegment, float& t0, float& t0Error) const;

    const AtlasDetectorID* m_idHelper;
    ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
    ToolHandle<MuonCalib::IIdToFixedIdTool> m_idToFixedIdTool;
    
    double m_matchThreshold; 
    int m_numberOfMatchedCut;

    // switch to turn on/off the time reconstruction for mboy CSC segments (needed until release 17.1.0)
    bool m_doMuonBoyCSCTiming;
  };
}

#endif
