/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/MuonCandidate.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"

namespace MuonCombined {

    MuonCandidate::MuonCandidate(const Trk::Track& msTrack) : m_muonSpectrometerTrack(&msTrack) {}

    MuonCandidate::MuonCandidate(const Trk::Track& msTrack, std::unique_ptr<Trk::Track> extrapolatedTrack) :
        m_extrapolatedTrack(std::move(extrapolatedTrack)), m_muonSpectrometerTrack(&msTrack) {}

    MuonCandidate::MuonCandidate(const ElementLink<xAOD::TrackParticleContainer>& trackLink,
                                 std::unique_ptr<Trk::Track> extrapolatedTrack) :
        m_muonSpectrometerTrackLink(trackLink),
        m_extrapolatedTrack(std::move(extrapolatedTrack)),
        m_muonSpectrometerTrack((*m_muonSpectrometerTrackLink)->track()) {}

    MuonCandidate::MuonCandidate(const ElementLink<xAOD::TrackParticleContainer>& trackLink,
                                 const ElementLink<TrackCollection>& saTrackLink) :
        m_muonSpectrometerTrackLink(trackLink),
        m_extrapolatedTrackLink(saTrackLink),
        m_muonSpectrometerTrack((*m_muonSpectrometerTrackLink)->track()) {}

    MuonCandidate::~MuonCandidate() = default;

    std::string MuonCandidate::toString() const {
        const Trk::Track* track = extrapolatedTrack() ? extrapolatedTrack() : &muonSpectrometerTrack();
        const Trk::Perigee* perigee = track->perigeeParameters();
        std::ostringstream sout;
        if (!perigee)
            sout << " Track has no perigee parameters!";
        else
            sout << " pt " << perigee->momentum().perp() << " eta " << perigee->momentum().eta() << " phi " << perigee->momentum().phi();
        sout << " hasExtrapolatedTrack " << (m_extrapolatedTrack ? "yes" : "no") << std::endl;
        return sout.str();
    }

    const Trk::Track& MuonCandidate::muonSpectrometerTrack() const { return *m_muonSpectrometerTrack; }

    const ElementLink<xAOD::TrackParticleContainer>& MuonCandidate::muonSpectrometerTrackLink() const {
        return m_muonSpectrometerTrackLink;
    }

    void MuonCandidate::updateExtrapolatedTrack(std::unique_ptr<Trk::Track> extrapolatedTrack) {
        m_extrapolatedTrack = std::move(extrapolatedTrack);
    }

    const Trk::Track* MuonCandidate::extrapolatedTrack() const {
        if (m_extrapolatedTrack) return m_extrapolatedTrack.get();
        return m_extrapolatedTrackLink.isValid() ? *m_extrapolatedTrackLink : nullptr;
    }
    const Trk::Track* MuonCandidate::primaryTrack() const {
        const Trk::Track* ext_trk = extrapolatedTrack();
        return ext_trk ? ext_trk : &muonSpectrometerTrack();
    }

    std::unique_ptr<Trk::Track> MuonCandidate::releaseExtrapolatedTrack() { return std::move(m_extrapolatedTrack); }

    const ElementLink<TrackCollection>& MuonCandidate::extrapolatedTrackLink() const { return m_extrapolatedTrackLink; }

    void MuonCandidate::setComissioning(bool b) { m_isComissioning = b; }
    bool MuonCandidate::isComissioning() const { return m_isComissioning; }

    void MuonCandidate::setSegments(std::vector<ElementLink<xAOD::MuonSegmentContainer>>&& segments) { m_assoc_segments = std::move(segments);}
    const std::vector<ElementLink<xAOD::MuonSegmentContainer>>& MuonCandidate::getSegments() const { return m_assoc_segments; }

}  // namespace MuonCombined
