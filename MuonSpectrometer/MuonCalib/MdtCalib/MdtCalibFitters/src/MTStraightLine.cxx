/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibFitters/MTStraightLine.h"

#include <iostream>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "cmath"
using namespace MuonCalib;

void MTStraightLine::init() {
    m_position = Amg::Vector3D(0.0, 0.0, 0.0);
    m_direction = Amg::Vector3D(0.0, 0.0, 0.0);
    m_position_error = Amg::Vector3D(0.0, 0.0, 0.0);
    m_direction_error = Amg::Vector3D(0.0, 0.0, 0.0);

    return;
}

void MTStraightLine::init(const Amg::Vector3D& r_position, const Amg::Vector3D& r_direction, const Amg::Vector3D& r_position_error,
                          const Amg::Vector3D& r_direction_error) {
    m_position = r_position;
    m_direction = r_direction;
    m_position_error = r_position_error;
    m_direction_error = r_direction_error;

    return;
}

void MTStraightLine::init(const double& r_a_x1, const double& r_b_x1, const double& r_a_x2, const double& r_b_x2, const double& r_a_x1_err,
                          const double& r_b_x1_err, const double& r_a_x2_err, const double& r_b_x2_err) {
    m_position = Amg::Vector3D(r_b_x1, r_b_x2, 0.0);
    m_direction = Amg::Vector3D(r_a_x1, r_a_x2, 1.0);
    m_position_error = Amg::Vector3D(r_b_x1_err, r_b_x2_err, 0.0);
    m_direction_error = Amg::Vector3D(r_a_x1_err, r_a_x2_err, 1.0);

    return;
}
Amg::Vector3D MTStraightLine::positionVector() const { return m_position; }
Amg::Vector3D MTStraightLine::directionVector() const { return m_direction; }
Amg::Vector3D MTStraightLine::positionError() const { return m_position_error; }
Amg::Vector3D MTStraightLine::directionError() const { return m_direction_error; }
double MTStraightLine::a_x1() const { return (m_direction.z() != 0.) ? m_direction.x() / m_direction.z() : 0.; }

double MTStraightLine::a_x1_error() const {
    return m_direction.z() != 0.
               ? std::hypot(m_direction_error.x() / m_direction_error.z(), m_direction_error.y() * a_x1() / m_direction_error.z())
               : 0.;
}
double MTStraightLine::b_x1() const {
    if (m_direction.z() == 0.0) {
        MsgStream log(Athena::getMessageSvc(), "MTStraightLine");
        log << MSG::WARNING << "Class MTStraightLine, method b_x1: b_x1 not uniquely defined." << endmsg;
        return m_position.x();
    }
    return (m_position.x() - m_position.z() * a_x1());
}

double MTStraightLine::b_x1_error() const {
    return std::hypot(m_position_error.x(), a_x1() * m_position_error.z(), m_position.z() * a_x1_error());
}
double MTStraightLine::a_x2() const { return (m_direction.z() == 0.0) ? 0. : m_direction.y() / m_direction.z(); }
double MTStraightLine::a_x2_error() const {
    return (m_direction.z() == 0)
               ? 0.0
               :

               std::hypot(m_direction_error.y() / m_direction_error.z(), m_direction_error.y() * a_x2() / m_direction_error.z());
}

double MTStraightLine::b_x2() const {
    if (m_direction.z() == 0.0) {
        MsgStream log(Athena::getMessageSvc(), "MTStraightLine");
        log << MSG::WARNING << "Class MTStraightLine, method b_x2: b_x2 not uniquely defined." << endmsg;
        return m_position.x();
    }
    return (m_position.y() - m_position.z() * a_x2());
}

double MTStraightLine::b_x2_error() const {
    return std::hypot(m_position_error.y(), a_x2() * m_position_error.z(), m_position.z() * a_x2_error());
}

Amg::Vector3D MTStraightLine::pointOnLine(const double& lambda) const { return m_position + lambda * m_direction; }

double MTStraightLine::signDistFrom(const MTStraightLine& h) const {
    //:::::::::::::::::::::::::
    //:: AUXILIARY VARIABLES ::
    //:::::::::::::::::::::::::

    Amg::Vector3D a = m_position, u = m_direction;
    // position and direction vectors of g
    Amg::Vector3D b = h.positionVector(), v = h.directionVector();
    // position and direction vectors of h
    Amg::Vector3D n;  // normal vector of plane spanned by g and h
    Amg::Vector3D d;  // distance vector

    //:::::::::::::
    //:: PROGRAM ::
    //:::::::::::::

    //::::::::::::::::::::::::
    //:: collinearity check ::
    //::::::::::::::::::::::::

    n = u.cross(v);
    d = a - b;
    if (n.dot(n) == 0.0) {
        // straight lines are parallel
        // no sign given
        return std::sqrt(d.dot(d) - (u.unit().dot(d)) * (u.unit().dot(d)));
    }

    return (d.dot(n.unit()));
}

double MTStraightLine::distFromLine(const Amg::Vector3D& point) const {
    //:::::::::::::::::::::::::
    //:: AUXILIARY VARIABLES ::
    //:::::::::::::::::::::::::

    Amg::Vector3D u = m_direction;

    //::::::::::::::::::::::::
    //:: CALCULATE DISTANCE ::
    //::::::::::::::::::::::::

    return std::sqrt((point - m_position).dot(point - m_position) -
                     ((point - m_position).dot(u.unit())) * ((point - m_position).dot(u.unit())));
}
void MTStraightLine::setChi2(double chi2) { m_chi2 = std::isnan(chi2) ? -1 : chi2; }
double MTStraightLine::chi2() const { return m_chi2; }
void MTStraightLine::setNumberOfTrackHits(unsigned int n_hits) { m_numTrkHits = n_hits; }
unsigned int MTStraightLine::numberOfTrackHits() const { return m_numTrkHits; }

double MTStraightLine::chi2PerDegreesOfFreedom() const { return m_chi2 / (m_numTrkHits > 2 ? m_numTrkHits - 2 : 0.01); }

void MTStraightLine::setUsedHits(const MdtHitVec& hits) { m_used_hits = hits; }
const MTStraightLine::MdtHitVec& MTStraightLine::trackHits() const { return m_used_hits; }
