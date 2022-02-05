/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignErrorBase/AlignmentRotationDeviation.h"

#include <iostream>
#include <utility>

using namespace MuonAlign;

AlignmentRotationDeviation::AlignmentRotationDeviation(Amg::Vector3D center, Amg::Vector3D axis, double sigma,
                                                       const std::vector<const Trk::RIO_OnTrack*>& hits) :
    m_center(std::move(center)), m_axis(std::move(axis)), m_sigma(sigma) {
    m_hits = hits;
    m_hitshash = 0;
    m_hitshashdone = false;
}

int AlignmentRotationDeviation::nPar() const { return 1; }

double AlignmentRotationDeviation::getCovariance(int, int) const { return m_sigma * m_sigma; }

Amg::Transform3D AlignmentRotationDeviation::getTransform(const std::vector<double>& parameters) const {
    Amg::Translation3D transl1(-m_center);
    Amg::Translation3D transl2(m_center);
    Amg::AngleAxis3D angleaxis = Eigen::AngleAxis<double>(parameters[0], m_axis);
    Amg::Transform3D t = transl2 * angleaxis * transl1;

    return t;
}

void AlignmentRotationDeviation::print(std::ostream& out) const {
    out << "A rotation around center = (" << m_center.x() << ", " << m_center.y() << ", " << m_center.z() << ") and axis = (" << m_axis.x()
        << ", " << m_axis.y() << ", " << m_axis.z() << ") with sigma=" << m_sigma << " mrad applied to " << m_hits.size() << " hits"
        << std::endl;
}
