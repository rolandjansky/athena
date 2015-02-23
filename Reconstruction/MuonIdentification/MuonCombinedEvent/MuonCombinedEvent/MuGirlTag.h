/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUGIRLTAG_H
#define MUONCOMBINEDEVENT_MUGIRLTAG_H


#include "MuonCombinedEvent/TagBase.h"
#include <vector>

namespace Trk {
  class Track;
 
}

namespace Muon {
  class MuonSegment;
}

namespace MuonCombined {
  
  /** TagBase implementation for a combined fit */
  class MuGirlTag : public TagBase {
  public:
    
    /** Constructor a combined track and a list of segments as input
	The class takes ownership of the combined track and the segments
    */
    MuGirlTag( const Trk::Track* combinedTrack, const std::vector<const Muon::MuonSegment*>& segments );

    /** Constructor taking a list of 
	Users should ensure that the life time of the MuonCandidate 
	The class takes ownership of the combined track 
    */
    MuGirlTag( const std::vector<const Muon::MuonSegment*>& segments );

    /** destructor */
    ~MuGirlTag();

    /** access combined track */
    const Trk::Track* getCombinedTrack() const;
    const Trk::Track& combinedTrack() const;

    /** release combined track, user gets ownership */
    const Trk::Track* releaseCombinedTrack();

    /** access update extrapolated track, returns zero if none are available */
    const Trk::Track* updatedExtrapolatedTrack() const;

    /** set update extrapolated track, takes ownership */
    void setUpdatedExtrapolatedTrack(std::unique_ptr<const Trk::Track> track);

    /** release combined track, user gets ownership */
    std::unique_ptr<const Trk::Track> releaseUpdatedExtrapolatedTrack();

    /** access segments */
    const std::vector<const Muon::MuonSegment*>& segments() const ;

    /** release segments, user gets ownership */
    void releaseSegments( std::vector<const Muon::MuonSegment*>& segments );

    void releaseSegments();

    /** name string */
    std::string name() const { return "MuGirlTag"; }

    /** print content to string */
    std::string toString() const { return name(); }

    /** access to primary muon system track, zero if non available */
    const Trk::Track* primaryTrack() const;

  private:
    /** block copy and assignment */
    MuGirlTag(const MuGirlTag&) = delete;
    MuGirlTag& operator=(const MuGirlTag&) = delete;

    /** data content */
    const Trk::Track*    m_combinedTrack;  /// combined track 
    std::unique_ptr<const Trk::Track>    m_updatedExtrapolatedTrack; /// updated extrapolated track
    std::vector<const Muon::MuonSegment*> m_segments; /// list of segments

  };
  inline bool operator<( const MuGirlTag& t1,  const MuGirlTag& t2 ){
    const Trk::FitQuality* t1FQ = (t1.getCombinedTrack()!=NULL)? t1.getCombinedTrack()->fitQuality() : 0;
    const Trk::FitQuality* t2FQ = (t2.getCombinedTrack()!=NULL)? t2.getCombinedTrack()->fitQuality() : 0;
    if (t1FQ!=NULL && t2FQ!=NULL)
    {
        return t1FQ->chiSquared() < t2FQ->chiSquared();
    }
    return t1.segments().size() < t2.segments().size(); 
  }

  inline const Trk::Track* MuGirlTag::getCombinedTrack() const { return m_combinedTrack; }

  inline const Trk::Track& MuGirlTag::combinedTrack() const { return *m_combinedTrack; }

  inline const Trk::Track* MuGirlTag::releaseCombinedTrack() { 
    const Trk::Track* tmp = m_combinedTrack;   
    m_combinedTrack=0;
    return tmp;
  }
  
  inline const Trk::Track* MuGirlTag::updatedExtrapolatedTrack() const { return m_updatedExtrapolatedTrack.get(); }
  inline void MuGirlTag::setUpdatedExtrapolatedTrack(std::unique_ptr<const Trk::Track> track)
  {
    m_updatedExtrapolatedTrack = std::move(track);
  }

  inline std::unique_ptr<const Trk::Track> MuGirlTag::releaseUpdatedExtrapolatedTrack() { 
    return std::move(m_updatedExtrapolatedTrack);
  }

  inline void MuGirlTag::releaseSegments() {
    m_segments.clear();
  }


  inline const std::vector<const Muon::MuonSegment*>& MuGirlTag::segments() const { return m_segments; }

  inline void MuGirlTag::releaseSegments( std::vector<const Muon::MuonSegment*>& segments ) {
    std::swap(m_segments,segments);
  }

  inline const Trk::Track* MuGirlTag::primaryTrack() const {
    return &combinedTrack();
  }

}


#endif
