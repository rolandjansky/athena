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
     
        /** constructor taking an ElementLink to a xAOD::TrackParicle&
            Users should ensure that the element link is valid and the lifetime of the Track object is longer that the MuonCandidate
            as it internally caches a pointer to it.
        */
        MuonCandidate(const ElementLink<xAOD::TrackParticleContainer>& msTrackLink);

        /*Constructor taking two ElementLinks, and the index location of the extrapolated track in the container*/
        MuonCandidate(const ElementLink<xAOD::TrackParticleContainer>& msTrackLink, const ElementLink<TrackCollection>& saTrackLink, 
                      size_t container_idx);
        
        /* Update constructor to insert the element link to the extrapolated MSOE xAOD TrackParticle */
        MuonCandidate(const MuonCandidate& oldCandidate,
                      const ElementLink<xAOD::TrackParticleContainer>& msoeTrackLink);
        
        /** destructor */
        ~MuonCandidate();

        /** access spectrometer track, always there */
        const Trk::Track& muonSpectrometerTrack() const;

        /** access spectrometer track, always there */
        const ElementLink<xAOD::TrackParticleContainer>& muonSpectrometerTrackLink() const;

        /** access extrapolated track, can be zero if back extrapolation failed */
        const Trk::Track* extrapolatedTrack() const;
        
        /** Returns the extrapolated track otherwise the muonSpectrometer */
        const Trk::Track* primaryTrack() const;

        /** access extrapolated track element link*/
        const ElementLink<TrackCollection>& extrapolatedTrackLink() const;
        /** returns the index of the container where the extrapolated track is located */
        size_t extrapolatedElementID() const;
        /** access extrapolated track particle element link*/
        const ElementLink<xAOD::TrackParticleContainer>& extrapolatedParticleLink() const;


        /** print candidate to string */
        std::string toString() const;
       
        /** Sets ths comissioning flag */
        void setCommissioning(bool b);
        /** Returns whether the muon belongs to the comissioning chain **/
        bool isCommissioning() const;

        /** set the vector of associated segments to the candidate **/
        void setSegments(std::vector<ElementLink<xAOD::MuonSegmentContainer>>&& segments);
        /** returns the vector of associated muon segments **/
        const std::vector<ElementLink<xAOD::MuonSegmentContainer>>& getSegments() const;

    private:
        /** element link to spectrometer track */
        const ElementLink<xAOD::TrackParticleContainer> m_muonSpectrometerTrackLink{};
        /** element link to the extrapolated muon track particle */
        const ElementLink<xAOD::TrackParticleContainer> m_extrapolatedParticleLink{};
        /** element link to extrapolated track */
        const ElementLink<TrackCollection> m_extrapolatedTrackLink{};
     
        /** pointer to spectrometer track, not owned */
        const Trk::Track* m_muonSpectrometerTrack{nullptr};

        /** flag whether the track belongs to the comissioning stream **/
        bool m_isCommissioning{false};

        /** Segments associated with the candidate **/
        std::vector<ElementLink<xAOD::MuonSegmentContainer>> m_assoc_segments;

        /// Index of the MSOE track in the output container
        size_t m_extContIdx{0};
    };

}  // namespace MuonCombined

#endif
