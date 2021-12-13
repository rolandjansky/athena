/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUGIRLTAG_H
#define MUONCOMBINEDEVENT_MUGIRLTAG_H

#include <vector>

#include "MuonCombinedEvent/TagBase.h"

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
        MuGirlTag(const ElementLink<TrackCollection>& comblink, const std::vector<ElementLink<Trk::SegmentCollection> >& segLinks);

        /** Constructor taking a list of segments
         */
        MuGirlTag(const std::vector<ElementLink<Trk::SegmentCollection> >& segLinks);

        /** destructor */
        ~MuGirlTag();

        /** access combined track */
        const Trk::Track* combinedTrack() const;

        /*Element Link to the combined track*/
        ElementLink<TrackCollection> combinedTrackLink() const;

        /** access update extrapolated track, returns zero if none are available */
        const Trk::Track* updatedExtrapolatedTrack() const;

        /* ElementLink to ME track*/
        ElementLink<TrackCollection> updatedExtrapolatedTrackLink() const;

        /** set update extrapolated track*/
        void setUpdatedExtrapolatedTrack(ElementLink<TrackCollection> melink);

        /** access segment ElementLinks */
        const std::vector<ElementLink<Trk::SegmentCollection> >& segments() const;

        /** name string */
        std::string name() const override;

        /** print content to string */
        std::string toString() const override;

        /** access to primary muon system track, zero if non available */
        const Trk::Track* primaryTrack() const override;
        
        /** Sets ths comissioning flag */
        void setComissioning(bool b);
        /** Returns whether the muon belongs to the comissioning chain **/
        bool isComissioning() const override;

    private:
        /** block copy and assignment */
        MuGirlTag(const MuGirlTag&) = delete;
        MuGirlTag& operator=(const MuGirlTag&) = delete;

        /** data content */
        ElementLink<TrackCollection> m_combLink;
        ElementLink<TrackCollection> m_meLink;
        std::vector<ElementLink<Trk::SegmentCollection> > m_segments;  /// list of segments

        bool m_isComissioning{false};
    };
    bool operator<(const MuGirlTag& t1, const MuGirlTag& t2);
}  // namespace MuonCombined

#endif
