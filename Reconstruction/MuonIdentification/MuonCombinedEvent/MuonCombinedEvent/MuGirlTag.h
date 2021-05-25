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
    
    /** Constructor a link to a combined track and a list of segment ElementLinks as input
    */
    MuGirlTag( const ElementLink<TrackCollection>& comblink, const std::vector<ElementLink<Trk::SegmentCollection> >& segLinks );

    /** Constructor taking a list of segments
    */
    MuGirlTag( const std::vector<ElementLink<Trk::SegmentCollection> >& segLinks );

    /** destructor */
    ~MuGirlTag();

    /** access combined track */
    const Trk::Track* combinedTrack() const;

    /*Element Link to the combined track*/
    ElementLink<TrackCollection> combinedTrackLink() const {return m_combLink;}

    /** access update extrapolated track, returns zero if none are available */
    const Trk::Track* updatedExtrapolatedTrack() const;

    /* ElementLink to ME track*/
    ElementLink<TrackCollection> updatedExtrapolatedTrackLink() const {return m_meLink;}

    /** set update extrapolated track*/
    void setUpdatedExtrapolatedTrack(ElementLink<TrackCollection> melink) {m_meLink=melink;}

    /** access segment ElementLinks */
    const std::vector<ElementLink<Trk::SegmentCollection> >& segments() const ;

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
    ElementLink<TrackCollection> m_combLink;
    ElementLink<TrackCollection> m_meLink;
    std::vector<ElementLink<Trk::SegmentCollection> > m_segments; /// list of segments

  };
  inline bool operator<( const MuGirlTag& t1,  const MuGirlTag& t2 ){
    const Trk::FitQuality* t1FQ = (t1.combinedTrack()!=nullptr)? t1.combinedTrack()->fitQuality() : 0;
    const Trk::FitQuality* t2FQ = (t2.combinedTrack()!=nullptr)? t2.combinedTrack()->fitQuality() : 0;
    if (t1FQ!=NULL && t2FQ!=NULL)
    {
        return t1FQ->chiSquared() < t2FQ->chiSquared();
    }
    return t1.segments().size() < t2.segments().size(); 
  }

  inline const Trk::Track* MuGirlTag::combinedTrack() const { 
    return m_combLink.isValid() ? *m_combLink : nullptr; }

  inline const Trk::Track* MuGirlTag::updatedExtrapolatedTrack() const { return m_meLink.isValid() ? *m_meLink : nullptr; }

  inline const std::vector<ElementLink<Trk::SegmentCollection> >& MuGirlTag::segments() const { return m_segments; }

  inline const Trk::Track* MuGirlTag::primaryTrack() const {
    return combinedTrack();
  }

}


#endif
