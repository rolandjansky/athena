/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MSVertexUtils/MSVertex.h"

#include <math.h>

#include <sstream>

MSVertex::MSVertex() : m_author(0), m_position(), m_tracks(0), m_chi2prob(-1.), m_chi2(-1.), m_nMDT(-1), m_nRPC(-1), m_nTGC(-1) {}

MSVertex::MSVertex(const MSVertex& vertex) :
    m_author(vertex.getAuthor()),
    m_position(vertex.getPosition()),
    m_tracks(0),
    m_chi2prob(vertex.getChi2Probability()),
    m_chi2(vertex.getChi2()),
    m_nMDT(vertex.getNMDT()),
    m_nRPC(vertex.getNRPC()),
    m_nTGC(vertex.getNTGC()) {
    for (std::vector<xAOD::TrackParticle*>::const_iterator i = vertex.getTracks()->begin(); i != vertex.getTracks()->end(); ++i) {
        m_tracks.push_back(new xAOD::TrackParticle(**i));
    }
}

MSVertex::MSVertex(int author, const Amg::Vector3D& position, float chi2prob, float chi2, int nMDT, int nRPC, int nTGC) :
    m_author(author), m_position(position), m_tracks(0), m_chi2prob(chi2prob), m_chi2(chi2), m_nMDT(nMDT), m_nRPC(nRPC), m_nTGC(nTGC) {}

MSVertex::MSVertex(int author, const Amg::Vector3D& position, const std::vector<xAOD::TrackParticle*>& tracks, float chi2prob, float chi2,
                   int nMDT, int nRPC, int nTGC) :
    m_author(author),
    m_position(position),
    m_tracks(tracks),
    m_chi2prob(chi2prob),
    m_chi2(chi2),
    m_nMDT(nMDT),
    m_nRPC(nRPC),
    m_nTGC(nTGC) {}

MSVertex::~MSVertex() {
    for (std::vector<xAOD::TrackParticle*>::iterator i = m_tracks.begin(); i != m_tracks.end(); ++i) {
        if ((*i)) {
            delete (*i);
            (*i) = 0;
        }
    }
    m_tracks.clear();
}

MSVertex& MSVertex::operator=(const MSVertex& msvx) {
    if (this != &msvx) {
        m_author = msvx.getAuthor();
        m_position = msvx.getPosition();
        m_chi2prob = msvx.getChi2Probability();
        m_chi2 = msvx.getChi2();
        m_nMDT = msvx.getNMDT();
        m_nRPC = msvx.getNRPC();
        m_nTGC = msvx.getNTGC();

        for (std::vector<xAOD::TrackParticle*>::const_iterator i = msvx.getTracks()->begin(); i != msvx.getTracks()->end(); ++i) {
            m_tracks.push_back(new xAOD::TrackParticle(**i));
        }
    }
    return *this;
}

MSVertex* MSVertex::clone() {
    std::vector<xAOD::TrackParticle*> trk;
    for (std::vector<xAOD::TrackParticle*>::iterator i = m_tracks.begin(); i != m_tracks.end(); ++i) {
        trk.push_back(new xAOD::TrackParticle(**i));
    }
    return new MSVertex(m_author, m_position, trk, m_chi2prob, m_chi2, m_nMDT, m_nRPC, m_nTGC);
}

void MSVertex::setPosition(const Amg::Vector3D& position) { m_position = position; }

const Amg::Vector3D& MSVertex::getPosition() const { return m_position; }

const std::vector<xAOD::TrackParticle*>* MSVertex::getTracks(void) const { return &m_tracks; }

void MSVertex::setAuthor(const int author) { m_author = author; }

int MSVertex::getAuthor() const { return m_author; }
float MSVertex::getChi2Probability() const { return m_chi2prob; }
float MSVertex::getChi2() const { return m_chi2; }

int MSVertex::getNTracks() const {
    if (getTracks())
        return getTracks()->size();
    else
        return 0;
}

void MSVertex::setNMDT(const int nMDT) { m_nMDT = nMDT; }
void MSVertex::setNRPC(const int nRPC) { m_nRPC = nRPC; }
void MSVertex::setNTGC(const int nTGC) { m_nTGC = nTGC; }

int MSVertex::getNMDT() const { return m_nMDT; }
int MSVertex::getNRPC() const { return m_nRPC; }
int MSVertex::getNTGC() const { return m_nTGC; }

std::string str(const MSVertex& a) {
    std::stringstream ss;

    ss << "author = " << a.getAuthor() << "; x = " << a.getPosition().x() << "; y = " << a.getPosition().y()
       << "; z = " << a.getPosition().z() << "; phi = " << a.getPosition().phi() << "; eta = " << a.getPosition().eta()
       << "; chi2 prob. = " << a.getChi2Probability() << "; # tracks = " << a.getNTracks() << "; # MDT hits = " << a.getNMDT()
       << "; # RPC hits = " << a.getNRPC() << "; # TGC hits = " << a.getNTGC();

    return ss.str();
}

MsgStream& operator<<(MsgStream& m, const MSVertex& a) { return (m << str(a)); }

bool operator==(const MSVertex& a, const MSVertex& b) {
    //* distance used to compare floats *//
    const double DELTA = 1e-3;

    if (std::abs(a.getPosition().x() - b.getPosition().x()) > DELTA) return false;
    if (std::abs(a.getPosition().y() - b.getPosition().y()) > DELTA) return false;
    if (std::abs(a.getPosition().z() - b.getPosition().z()) > DELTA) return false;
    if (std::abs(a.getPosition().eta() - b.getPosition().eta()) > DELTA) return false;
    if (std::abs(a.getPosition().phi() - b.getPosition().phi()) > DELTA) return false;
    if (std::abs(a.getChi2Probability() - b.getChi2Probability()) > DELTA) return false;

    if (a.getAuthor() - b.getAuthor() != 0) return false;
    if (a.getNTracks() - b.getNTracks() != 0) return false;
    if (a.getNMDT() - b.getNMDT() != 0) return false;
    if (a.getNRPC() - b.getNRPC() != 0) return false;
    if (a.getNTGC() - b.getNTGC() != 0) return false;

    return true;
}
