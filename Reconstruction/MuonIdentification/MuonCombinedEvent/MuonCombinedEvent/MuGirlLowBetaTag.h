/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUGIRLLOWBETATAG_H
#define MUONCOMBINEDEVENT_MUGIRLLOWBETATAG_H

#include <vector>

#include "MuonCombinedEvent/CandidateSummary.h"
#include "MuonCombinedEvent/TagBase.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackCollection.h"

namespace Trk {
    class Track;
}

namespace Muon {
    class MuonSegment;
}

namespace MuonCombined {

    /** TagBase implementation for a combined fit */
    class MuGirlLowBetaTag : public TagBase {
    public:
        /** Constructor a combined track and a list of segments as input
         */
        MuGirlLowBetaTag(const ElementLink<TrackCollection>& combLink, const std::vector<ElementLink<Trk::SegmentCollection> >& segments);

        /** Constructor taking a list of segments
         */
        MuGirlLowBetaTag(const std::vector<ElementLink<Trk::SegmentCollection> >& segments);

        /** destructor */
        ~MuGirlLowBetaTag();

        /** access combined track */
        const Trk::Track* combinedTrack() const;

        /*ElementLink to the combined track*/
        ElementLink<TrackCollection> combinedTrackLink() const { return m_combLink; }

        /** access update extrapolated track, returns zero if none are available */
        const Trk::Track* updatedExtrapolatedTrack() const;

        /** set update extrapolated track*/
        void setUpdatedExtrapolatedTrack(ElementLink<TrackCollection> meLink);

        /** access segments */
        const std::vector<ElementLink<Trk::SegmentCollection> >& segments() const;
        std::vector<const Muon::MuonSegment*> associatedSegments() const override;

        /* stau Dressing */
        void setMuBeta(float muBeta);
        float getMuBeta();

        void setStauSummary(MuGirlNS::CandidateSummary* stauSummary);
        MuGirlNS::CandidateSummary* getStauSummary();

        void setStauExtras(MuGirlNS::StauExtras* stauExtras);
        MuGirlNS::StauExtras* getStauExtras();
        const MuGirlNS::StauExtras* getStauExtras() const;

        void setRHExtras(MuGirlNS::RHExtras* rhExtras);
        MuGirlNS::RHExtras* getRHExtras();

        /** name string */
        std::string name() const override { return "MuGirlLowBetaTag"; }

        /** print content to string */
        std::string toString() const override { return name(); }

        /** access to primary muon system track, zero if non available */
        const Trk::Track* primaryTrack() const override;

    private:
        /** block copy and assignment */
        MuGirlLowBetaTag(const MuGirlLowBetaTag&) = delete;
        MuGirlLowBetaTag& operator=(const MuGirlLowBetaTag&) = delete;

        /** data content */
        ElementLink<TrackCollection> m_combLink{};
        ElementLink<TrackCollection> m_meLink{};
        std::vector<ElementLink<Trk::SegmentCollection> > m_segments{};  /// list of segments

        // dressing
        float m_muBeta{-9999.};
        std::unique_ptr<MuGirlNS::CandidateSummary> m_stauSummary{};
        std::unique_ptr<MuGirlNS::StauExtras> m_stauExtras{};
        std::unique_ptr<MuGirlNS::RHExtras> m_rhExtras{};
    };
    inline bool operator<(const MuGirlLowBetaTag& t1, const MuGirlLowBetaTag& t2) {
        const Trk::FitQuality* t1FQ = t1.combinedTrack() ? t1.combinedTrack()->fitQuality() : nullptr;
        const Trk::FitQuality* t2FQ = t2.combinedTrack() ? t2.combinedTrack()->fitQuality() : nullptr;
        if (t1FQ && t2FQ) { return t1FQ->chiSquared() < t2FQ->chiSquared(); }
        return t1.segments().size() < t2.segments().size();
    }

}  // namespace MuonCombined

#endif
