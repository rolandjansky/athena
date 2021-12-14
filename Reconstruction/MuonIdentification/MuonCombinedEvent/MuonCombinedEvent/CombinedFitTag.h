/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_COMBINEDFITTAG_H
#define MUONCOMBINEDEVENT_COMBINEDFITTAG_H

#include <vector>

#include "MuidEvent/FieldIntegral.h"
#include "MuonCombinedEvent/TagBase.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkTrack/TrackCollection.h"

namespace Trk {
    class Track;
}

namespace MuonCombined {

    class MuonCandidate;

    /** TagBase implementation for a combined fit */
    class CombinedFitTag : public TagBase {
    public:
        /** Constructor taking a MuonCandidate as input
            Users should ensure that the life time of the MuonCandidate
            The class takes ownership of the track score
        */
        CombinedFitTag(xAOD::Muon::Author author, const MuonCandidate& muonCandidate, const Trk::TrackScore& score);

        /** destructor */
        ~CombinedFitTag();

        /** access combined track */
        const Trk::Track* combinedTrack() const;

        /*ElementLink to combined track*/
        ElementLink<TrackCollection> combinedTrackLink() const;

        /** set combined track ElementLink*/
        void setCombinedTrackLink(ElementLink<TrackCollection> combLink);

        /** access to MuonCandidate */
        const MuonCandidate& muonCandidate() const;

        /** access update extrapolated track, returns zero if none are available */
        const Trk::Track* updatedExtrapolatedTrack() const;

        /* ElementLink to ME track*/
        ElementLink<TrackCollection> updatedExtrapolatedTrackLink() const;

        /** set update extrapolated track ElementLink */
        void setUpdatedExtrapolatedTrackLink(ElementLink<TrackCollection> MELink);

        /** store inner match quality info */
        void innerMatch(double chi2, int dof, double prob);

        /** access the inner matching chi2*/
        double matchChi2() const;

        /** access the inner matching DoF*/
        int matchDoF() const;

        /** access the inner matching probability*/
        double matchProb() const;

        /** set field integral measurements*/
        void fieldIntegral(Rec::FieldIntegral fieldI);

        /** get field integral measurements*/
        Rec::FieldIntegral fieldIntegral() const;

        /** set momentum balance significance*/
        void momentumBalanceSignificance(double m);

        /** get momentum balance significance*/
        double momentumBalanceSignificance() const;

        /** get track score */
        const Trk::TrackScore& trackScore() const;

        /** name string */
        std::string name() const override;

        /** print content to string */
        std::string toString() const override;

        /** access to primary muon system track, zero if non available */
        const Trk::Track* primaryTrack() const override;

        /** access to associated segments, empty vector if non available */
        std::vector<const Muon::MuonSegment*> associatedSegments() const override;
        
         /** Returns whether the muon belongs to the comissioning chain **/
        bool isComissioning() const override;


    private:
        /** block copy and assignment */
        CombinedFitTag(const CombinedFitTag&) = delete;
        CombinedFitTag& operator=(const CombinedFitTag&) = delete;

        /** data content */
        const MuonCandidate* m_muonCandidate{nullptr};  /// MuonCandidate
        ElementLink<TrackCollection> m_combLink{};      // link to combined track
        Trk::TrackScore m_trackScore;
        ElementLink<TrackCollection> m_MELink{};  // link to ME track

        Rec::FieldIntegral m_fieldIntegral;

        double m_matchChi2{0.};
        int m_matchDoF{0};
        double m_matchProb{0.};
        double m_momentumBalanceSignificance{0.};
    };

    inline bool operator<(const CombinedFitTag& t1, const CombinedFitTag& t2) { return t1.trackScore() < t2.trackScore(); }
}  // namespace MuonCombined

#endif
