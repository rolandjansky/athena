/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MOOCANDIDATEMATCHINGTOOL_H
#define MOOCANDIDATEMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "CxxUtils/checker_macros.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonRecToolInterfaces/IMuonTrackSegmentMatchingTool.h"
#include "MuonTrackFindingEvent/MuonTrackSegmentMatchResult.h"

#include <array>
#include <atomic>
#include <string>
#include <set>

class MsgStream;

namespace Trk {
  class IPropagator;
  class IExtrapolator;
  class Track;
  class MeasurementBase;
}

namespace Muon {
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class MuPatCandidateTool;
  class MuonSegment;
  class MuPatTrack;
  class MuPatSegment;
  class MuPatCandidateBase;
  class IMuonSegmentMatchingTool;

  static const InterfaceID IID_MooCandidateMatchingTool("Muon::MooCandidateMatchingTool",1,0);
  
  /** class to manipulate MuPatCandidateBase objects */
  class MooCandidateMatchingTool : virtual public IMuonTrackSegmentMatchingTool, public AthAlgTool {
  public:

    /** Class with matching return information */
    struct MooTrackSegmentMatchResult : public TrackSegmentMatchResult {
      const MuPatTrack* MCTBTrack;
      const MuPatSegment* MCTBSegment;

      MooTrackSegmentMatchResult();

      virtual ~MooTrackSegmentMatchResult();

      virtual void clear();
     
    };

    
    /** default AlgTool constructor */
    MooCandidateMatchingTool(const std::string&, const std::string&, const IInterface*);
    
    /** destructor */
    ~MooCandidateMatchingTool();
    
    /** initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** finialize method, method taken from bass-class AlgTool */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MooCandidateMatchingTool; }
 
    /** @brief match two segment entries */
    bool match( const MuPatSegment& entry1, const MuPatSegment& entry2, bool useTightCuts = false ) const;

    /** @brief match a segment entry with a track entry */
    bool match( const MuPatTrack& entry1, const MuPatSegment& entry2, bool useTightCuts = false ) const;

    /** @brief match a segment entry with a base entry */
    bool match( const MuPatCandidateBase& entry1, const MuPatSegment& entry2, bool useTightCuts = false ) const;

    /** @brief match a track with a segment */
    bool match( const Trk::Track& track, const MuonSegment& segment, bool useTightCuts ) const;

    /** @brief calculate the info needed for the matching decision */
    void calculateTrackSegmentMatchResult( const MuPatTrack& entry1, const MuPatSegment& entry2, MooTrackSegmentMatchResult& info ) const;

    TrackSegmentMatchCuts getMatchingCuts( const MuPatTrack& entry1, const MuPatSegment& entry2, bool useTightCuts ) const;
    
    bool applyTrackSegmentCuts( MooTrackSegmentMatchResult& info, const TrackSegmentMatchCuts& cuts ) const;

    
    /** @brief check whether two segments are on the same side of the point of closest approach to the 
        perigee of the connecting track, or on the same side of the calorimeter (i.e. not crossing calo).

        @c requireSameSideOfPerigee : True  : require the segments to be on same side of perigee
                                      False : require the segments to be on same side of calorimeter
    */
    bool sameSide( const MuPatSegment& entry1, const MuPatSegment& entry2, bool requireSameSideOfPerigee ) const;

    /** @brief Check whether the track and segment are on the same side of the point of closest approach to the 
        perigee of the connecting track, or on the same side of the calorimeter (i.e. not crossing calo).

        @c requireSameSideOfPerigee : True  : require the segments to be on same side of perigee
                                      False : require the segments to be on same side of calorimeter
    */
    bool sameSide( const MuPatTrack& entry1, const MuPatSegment& entry2, bool requireSameSideOfPerigee ) const;

    /** check whether the two tracks are on the same side of the point of closest approach to the 
        perigee of the connecting track, or on the same side of the calorimeter (i.e. not crossing calo).

        @c requireSameSideOfPerigee : True  : require the segments to be on same side of perigee
                                      False : require the segments to be on same side of calorimeter
    */
    bool sameSide( const MuPatTrack& entry1, const MuPatTrack& entry2, bool requireSameSideOfPerigee ) const;

    /** check whether two positions are on the same side of the point of closest approach to the 
        perigee of the connecting track, or on the same side of the calorimeter (i.e. not crossing calo).

        @c requireSameSideOfPerigee : True  : require the segments to be on same side of perigee
                                      False : require the segments to be on same side of calorimeter
    */
    bool sameSide( const Amg::Vector3D& pos1, const Amg::Vector3D& pos2, bool requireSameSideOfPerigee ) const;

    /** check whether two positions are on the same side of the point of closest approach to the 
        perigee of the connecting track, or on the same side of the calorimeter (i.e. not crossing calo).

        @c requireSameSideOfPerigee : True  : require the segments to be on same side of perigee
                                      False : require the segments to be on same side of calorimeter
    */
    bool sameSide( const Amg::Vector3D& dir, const Amg::Vector3D& pos1, const Amg::Vector3D& pos2, bool requireSameSideOfPerigee ) const;

    /** @brief extract Idenfitiers from a vector of measurements and copy them into a set */
    void getIdentifierSet( const std::vector<const Trk::MeasurementBase*>& measurements, std::set<Identifier>& ids ) const;

    /** @brief evaluate overlap between phi hits of two entries. 
        returns true if the entries share all phi hits or the phi hits of one of the entries are subset of the other  */
    bool checkPhiHitConsistency( const MuPatCandidateBase& entry1, const MuPatCandidateBase& entry2 ) const;

    /** @brief evaluate distance between two segments, if too large return false (cut at 3000.).  */
    bool checkSegmentDistance( const MuPatSegment& entry1, const MuPatSegment& entry2 ) const;
    
    /** @brief return whether the 2 segments are in neighbouring chambers */
    bool areInNeighbouringChambers( const MuPatSegment& seg1, const MuPatSegment& seg2 ) const;


  private:

    double m_minimumSideMatchRadius;
    double m_matchAngXCut;
    double m_matchAngYCut;
    double m_matchAngXPullCut;
    double m_matchAngYPullCut;
    double m_matchPosXCut;
    double m_matchPosYCut;
    double m_matchPosPullCut;
    double m_alignErrorPosX;
    double m_alignErrorPosY;
    double m_alignErrorAngleX;
    double m_alignErrorAngleY;
    double m_matchChiSquaredCut;
    double m_matchChiSquaredCutTight;
    
    ToolHandle<MuonIdHelperTool>          m_idHelperTool;       //<! tool to assist with Identifiers
    ToolHandle<MuonEDMHelperTool>         m_helperTool;         //<! multipurpose helper tool
    ToolHandle<MuonEDMPrinterTool>        m_printer;            //<! tool to print EDM objects
    ToolHandle<Trk::IExtrapolator>        m_slExtrapolator;     //<! straight line extrapolator
    ToolHandle<Trk::IExtrapolator>        m_atlasExtrapolator;  //<! curved extrapolator
    ToolHandle<IMuonSegmentMatchingTool>  m_segmentMatchingTool;
    ToolHandle<IMuonSegmentMatchingTool>  m_segmentMatchingToolTight;
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc; 
    ToolHandle<MuPatCandidateTool>        m_candidateTool;
    int                                   m_trackSegmentPreMatchingStrategy; //!< 0=no segments match,1=any segment match,2=all segment match
    bool                                  m_doTrackSegmentMatching; //!< apply track-segment matching or not

    /** matching counters */
    mutable std::atomic_uint m_goodSegmentMatches;
    mutable std::atomic_uint m_goodSegmentMatchesTight;
    mutable std::atomic_uint m_segmentMatches;
    mutable std::atomic_uint m_segmentMatchesTight;
    mutable std::atomic_uint m_goodSegmentTrackMatches;
    mutable std::atomic_uint m_goodSegmentTrackMatchesTight;
    mutable std::atomic_uint m_sameSideOfPerigee;
    mutable std::atomic_uint m_otherSideOfPerigee;
    mutable std::atomic_uint m_sameSideOfPerigeeTrk;
    mutable std::atomic_uint m_otherSideOfPerigeeTrk;
    mutable std::atomic_uint m_segmentTrackMatches;
    mutable std::atomic_uint m_segmentTrackMatchesTight;
    mutable std::array<std::atomic_uint, TrackSegmentMatchResult::NumberOfReasons> m_reasonsForMatchOk ATLAS_THREAD_SAFE; // Guarded by atomicity
    mutable std::array<std::atomic_uint, TrackSegmentMatchResult::NumberOfReasons> m_reasonsForMatchNotOk ATLAS_THREAD_SAFE; // Guarded by atomicity

    double m_caloMatchZ; //!< Z position of calo end-cap disks. Used to determine if segments are on same side of Calo

    bool m_requireSameSide; //!< require entries to be on the same side of the Perigee or Calorimeter
  };

}


#endif
