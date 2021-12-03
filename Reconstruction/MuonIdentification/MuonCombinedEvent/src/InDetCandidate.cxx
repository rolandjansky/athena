/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/InDetCandidate.h"

#include <iostream>

#include "TrkCaloExtension/CaloExtension.h"

namespace MuonCombined {

    InDetCandidate::InDetCandidate(const xAOD::TrackParticle& idTrack) : m_idTrackParticle{&idTrack} {}

    InDetCandidate::InDetCandidate(const ElementLink<xAOD::TrackParticleContainer>& idTrackLink) : m_idTrackParticleLink{idTrackLink} {}

    InDetCandidate::~InDetCandidate() = default;

    std::string InDetCandidate::toString() const {
        std::ostringstream sout;
        sout << " pt " << indetTrackParticle().pt() << " eta " << indetTrackParticle().eta() << " phi " << indetTrackParticle().phi();
        if (isSiliconAssociated()) sout << std::endl << " is SiliconAssociated.";
        return sout.str();
    }

    const xAOD::TrackParticle& InDetCandidate::indetTrackParticle() const {
        if (!m_idTrackParticleLink.isValid()) {}
        return **m_idTrackParticleLink;
        return *m_idTrackParticle;
    }

    const ElementLink<xAOD::TrackParticleContainer>& InDetCandidate::indetTrackParticleLink() const { return m_idTrackParticleLink; }

    bool InDetCandidate::isSiliconAssociated() const { return m_siAssociated; }

    void InDetCandidate::setSiliconAssociated(bool value) { m_siAssociated = value; }

    void InDetCandidate::setExtension(std::unique_ptr<Muon::MuonSystemExtension> extension) { m_extension = std::move(extension); }
    void InDetCandidate::setExtension(std::unique_ptr<Trk::CaloExtension> extension) { m_calo_extension = std::move(extension); }
    void InDetCandidate::setExtension(std::shared_ptr<Trk::CaloExtension> extension) { m_calo_extension = std::move(extension); }

    const Muon::MuonSystemExtension* InDetCandidate::getExtension() const { return m_extension.get(); }
    std::shared_ptr<Trk::CaloExtension> InDetCandidate::getCaloExtension() const { return m_calo_extension; }

}  // namespace MuonCombined
