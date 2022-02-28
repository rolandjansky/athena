/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignErrorBase/AlignmentTranslationDeviation.h"

#include <iostream>

using namespace MuonAlign;

AlignmentTranslationDeviation::AlignmentTranslationDeviation(const Amg::Vector3D& u, double sigma,
                                                             const std::vector<const Trk::RIO_OnTrack*>& hits) :
    m_u(u.unit()), m_sigma(sigma) {
    m_hits = hits;
    m_hitshash = 0;
    m_hitshashdone = false;
}

int AlignmentTranslationDeviation::nPar() const { return 1; }

double AlignmentTranslationDeviation::getCovariance(int, int) const { return m_sigma * m_sigma; }

Amg::Transform3D AlignmentTranslationDeviation::getTransform(const std::vector<double>& parameters) const {
    Amg::Transform3D t(Amg::Translation3D(parameters[0] * m_u));
    return t;
}

void AlignmentTranslationDeviation::print(std::ostream& out) const {
    out << "A translation along (" << m_u.x() << ", " << m_u.y() << ", " << m_u.z() << ") with sigma=" << m_sigma << " mm applied to "
        << m_hits.size() << " hits" << std::endl;
}
