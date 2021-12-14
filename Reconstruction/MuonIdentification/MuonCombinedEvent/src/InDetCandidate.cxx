/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/InDetCandidate.h"

#include <iostream>



namespace MuonCombined {

    InDetCandidate::InDetCandidate(const xAOD::TrackParticle& idTrack) : m_idTrackParticle{&idTrack} {}

    InDetCandidate::InDetCandidate(const ElementLink<xAOD::TrackParticleContainer>& idTrackLink) : m_idTrackParticleLink{idTrackLink} {}

    InDetCandidate::~InDetCandidate() = default;

    std::string InDetCandidate::toString() const {
        std::ostringstream sout;
        sout << " pt: " << indetTrackParticle().pt() *1.e-3<< " GeV, eta: " << indetTrackParticle().eta() << ", phi: " << indetTrackParticle().phi();
        sout <<", d0: "<<indetTrackParticle().d0()<<", z0: "<<indetTrackParticle().z0()<<", q: "<<indetTrackParticle().charge()<<" ";
        if (isSiliconAssociated()) sout << "  is SAF.";
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
    void InDetCandidate::setExtension(std::unique_ptr<Trk::CaloExtension>& extension) { 
        m_calo_extension = std::move(extension); 
        m_calo_extension_ptr = m_calo_extension.get();
    }
    void InDetCandidate::setExtension(const Trk::CaloExtension* extension) { m_calo_extension_ptr =extension; }

    const Muon::MuonSystemExtension* InDetCandidate::getExtension() const { return m_extension.get(); }
    const Trk::CaloExtension* InDetCandidate::getCaloExtension() const { return m_calo_extension_ptr; }

    bool InDetCandidate::operator==(const InDetCandidate& other ) const { return &other.indetTrackParticle() == &indetTrackParticle(); }
       

}  // namespace MuonCombined
