/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUONCANDIDATE_H
#define MUONCOMBINEDEVENT_MUONCANDIDATE_H

#include <memory>
#include <vector>

#include "AthLinks/ElementLink.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"

namespace Trk {
    class Track;
}

namespace MuonCombined {

    class MuonCandidate {
    public:
        /** constructor taking an Trk::Track reference to the spectromter track
            Users should ensure that the element link is valid and the lifetime of the Track object is longer that the MuonCandidate
            as it internally caches a pointer to it.
        */
        MuonCandidate(const Trk::Track& msTrack);

        /** constructor taking an Trk::Track reference to the spectromter track
            Users should ensure that the element link is valid and the lifetime of the Track object is longer that the MuonCandidate
            as it internally caches a pointer to it. Class takes ownership of the extrapolatedTrack.
        */
        MuonCandidate(const Trk::Track& msTrack, std::unique_ptr<Trk::Track> extrapolatedTrack);

        /** constructor taking an ElementLink to a xAOD::TrackParicle&
            Users should ensure that the element link is valid and the lifetime of the Track object is longer that the MuonCandidate
            as it internally caches a pointer to it.
        */
        MuonCandidate(const ElementLink<xAOD::TrackParticleContainer>& msTrackLink);

        /** constructor taking an ElementLink to a xAOD::TrackParicle&
            Users should ensure that the element link is valid and the lifetime of the Track object is longer that the MuonCandidate
            as it internally caches a pointer to it. Class takes ownership of the extrapolatedTrack.
        */
        MuonCandidate(const ElementLink<xAOD::TrackParticleContainer>& msTrackLink, std::unique_ptr<Trk::Track> extrapolatedTrack);

        /*Constructor taking two ElementLinks, should be the new default (second may be empty)*/
        MuonCandidate(const ElementLink<xAOD::TrackParticleContainer>& msTrackLink, const ElementLink<TrackCollection>& saTrackLink);

        /** destructor */
        ~MuonCandidate();

        /** access spectrometer track, always there */
        const Trk::Track& muonSpectrometerTrack() const;

        /** access spectrometer track, always there */
        const ElementLink<xAOD::TrackParticleContainer>& muonSpectrometerTrackLink() const;

        /** add extrapolated track to the MuonCandidate, takes ownership of the track and delete the current one if present */
        void updateExtrapolatedTrack(std::unique_ptr<Trk::Track> extrapolatedTrack);

        /** access extrapolated track, can be zero if back extrapolation failed */
        const Trk::Track* extrapolatedTrack() const;
        /** Returns the extrapolated track otherwise the muonSpectrometer
         */
        const Trk::Track* primaryTrack() const;

        /** access extrapolated track, can be zero if back extrapolation failed */
        std::unique_ptr<Trk::Track> releaseExtrapolatedTrack();

        /** access extrapolated track element link*/
        const ElementLink<TrackCollection>& extrapolatedTrackLink() const;

        /** print candidate to string */
        std::string toString() const;
       
        /** Sets ths comissioning flag */
        void setComissioning(bool b);
        /** Returns whether the muon belongs to the comissioning chain **/
        bool isComissioning() const;

        /** set the vector of associated segments to the candidate **/
        void setSegments(std::vector<ElementLink<xAOD::MuonSegmentContainer>>&& segments);
        /** returns the vector of associated muon segments **/
        const std::vector<ElementLink<xAOD::MuonSegmentContainer>>& getSegments() const;

    private:
        /** element link to spectrometer track */
        const ElementLink<xAOD::TrackParticleContainer> m_muonSpectrometerTrackLink;

        /** element link to extrapolated track */
        const ElementLink<TrackCollection> m_extrapolatedTrackLink;

        /** pointer to extrapolated track, may be owned */
        std::unique_ptr<Trk::Track> m_extrapolatedTrack{nullptr};

        /** pointer to spectrometer track, not owned */
        const Trk::Track* m_muonSpectrometerTrack;

        /** flag whether the track belongs to the comissioning stream **/
        bool m_isComissioning{false};

        /** Segments associated with the candidate **/
        std::vector<ElementLink<xAOD::MuonSegmentContainer>> m_assoc_segments;
    };

}  // namespace MuonCombined

#endif
