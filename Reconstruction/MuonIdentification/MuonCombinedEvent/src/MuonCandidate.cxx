/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/MuonCandidate.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"

namespace MuonCombined {

    MuonCandidate::MuonCandidate(const ElementLink<xAOD::TrackParticleContainer>& trackLink) :
         m_muonSpectrometerTrackLink(trackLink),
          m_muonSpectrometerTrack((*m_muonSpectrometerTrackLink)->track()){}
    MuonCandidate::MuonCandidate(const ElementLink<xAOD::TrackParticleContainer>& trackLink,
                                 const ElementLink<TrackCollection>& saTrackLink,
                                 size_t container_idx) :
        m_muonSpectrometerTrackLink(trackLink),
        m_extrapolatedTrackLink(saTrackLink),
        m_muonSpectrometerTrack((*m_muonSpectrometerTrackLink)->track()),
        m_extContIdx(container_idx) {}

    MuonCandidate::~MuonCandidate() = default;

    std::string MuonCandidate::toString() const {
        const Trk::Track* track = extrapolatedTrack() ? extrapolatedTrack() : &muonSpectrometerTrack();
        const Trk::Perigee* perigee = track->perigeeParameters();
        std::ostringstream sout;
        if (!perigee)
            sout << " Track has no perigee parameters!";
        else
            sout << " pt " << perigee->momentum().perp() << " eta " << perigee->momentum().eta() << " phi " << perigee->momentum().phi();
        sout << " hasExtrapolatedTrack " << (extrapolatedTrack() ? "yes" : "no") << std::endl;
        return sout.str();
    }

    const Trk::Track& MuonCandidate::muonSpectrometerTrack() const { return *m_muonSpectrometerTrack; }

    const ElementLink<xAOD::TrackParticleContainer>& MuonCandidate::muonSpectrometerTrackLink() const {
        return m_muonSpectrometerTrackLink;
    }
    const Trk::Track* MuonCandidate::extrapolatedTrack() const {
        return m_extrapolatedTrackLink.isValid() ? *m_extrapolatedTrackLink : nullptr;
    }
    const Trk::Track* MuonCandidate::primaryTrack() const {
        const Trk::Track* ext_trk = extrapolatedTrack();
        return ext_trk ? ext_trk : &muonSpectrometerTrack();
    }

    const ElementLink<TrackCollection>& MuonCandidate::extrapolatedTrackLink() const { return m_extrapolatedTrackLink; }
    size_t MuonCandidate::extrapolatedElementID() const {return m_extrapolatedTrackLink.isValid() ? m_extContIdx : -1;}
    void MuonCandidate::setComissioning(bool b) { m_isComissioning = b; }
    bool MuonCandidate::isComissioning() const { return m_isComissioning; }

    void MuonCandidate::setSegments(std::vector<ElementLink<xAOD::MuonSegmentContainer>>&& segments) { m_assoc_segments = std::move(segments);}
    const std::vector<ElementLink<xAOD::MuonSegmentContainer>>& MuonCandidate::getSegments() const { return m_assoc_segments; }

}  // namespace MuonCombined
