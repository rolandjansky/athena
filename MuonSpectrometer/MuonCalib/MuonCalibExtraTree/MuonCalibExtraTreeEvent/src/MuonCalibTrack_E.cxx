/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"

#include <algorithm>
#include <utility>

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHole_E.h"
#include "MuonCalibExtraUtils/MuonCalibSLPropagator.h"
#include "MuonCalibStl/DeleteObject.h"

namespace {
    constexpr float qOverP_cutOff = 1.e-9;
}
namespace MuonCalib {

    const MuonCalibTrack_E::HitVector& MuonCalibTrack_E::hits() const { return m_hits; }
    MuonCalibTrack_E::HitVector& MuonCalibTrack_E::hits() { return m_hits; }

    const MuonCalibTrack_E::HoleVector& MuonCalibTrack_E::holes() const { return m_holes; }
    MuonCalibTrack_E::HoleVector& MuonCalibTrack_E::holes() { return m_holes; }

    const MuonCalibTrack_E::SegmentVector& MuonCalibTrack_E::segmetnsOnTrack() const { return m_segments_on_track; }
    MuonCalibTrack_E::SegmentVector& MuonCalibTrack_E::segmetnsOnTrack() { return m_segments_on_track; }

    void MuonCalibTrack_E::addSegmentOnTrack(const CalibSegPtr& s) { m_segments_on_track.emplace_back(s); }

    const Amg::Vector3D& MuonCalibTrack_E::position() const { return m_pos; }
    const Amg::Vector3D& MuonCalibTrack_E::direction() const { return m_dir; }

    void MuonCalibTrack_E::setChi2(float chi2) { m_params.chi2 = chi2; }
    void MuonCalibTrack_E::setNdof(int ndof) { m_params.ndof = ndof; }

    float MuonCalibTrack_E::x0() const { return m_params.x0; }
    float MuonCalibTrack_E::y0() const { return m_params.y0; }
    float MuonCalibTrack_E::z0() const { return m_params.z0; }
    float MuonCalibTrack_E::phi() const { return m_params.phi; }
    float MuonCalibTrack_E::theta() const { return m_params.theta; }
    float MuonCalibTrack_E::qOverP() const { return m_params.qOverP; }
    float MuonCalibTrack_E::p() const { return 1. / std::max(qOverP_cutOff, std::abs(qOverP())); }
    float MuonCalibTrack_E::pt() const { return std::sin(theta()) * p(); }
    float MuonCalibTrack_E::errInvP() const { return m_params.cov(4, 4); }
    int MuonCalibTrack_E::author() const { return m_params.author; }
    float MuonCalibTrack_E::chi2() const { return m_params.chi2; }
    int MuonCalibTrack_E::ndof() const { return m_params.ndof; }

    void MuonCalibTrack_E::setX0(float x0) { m_params.x0 = x0; }
    void MuonCalibTrack_E::setY0(float y0) { m_params.y0 = y0; }
    void MuonCalibTrack_E::setZ0(float z0) { m_params.z0 = z0; }
    void MuonCalibTrack_E::setPhi(float phi) { m_params.phi = phi; }
    void MuonCalibTrack_E::setTheta(float theta) { m_params.theta = theta; }
    void MuonCalibTrack_E::setQOverP(float qOverP) { m_params.qOverP = qOverP; }
    void MuonCalibTrack_E::setAuthor(int author) { m_params.author = author; }

    MuonCalibTrack_E::MuonCalibTrack_E(defineParams pars) : m_params{std::move(pars)}, m_pos{x0(), y0(), z0()} {
        Amg::setThetaPhi(m_dir, theta(), phi());
    }

    void MuonCalibTrack_E::addHit(const CalibHitPtr& hit) {
        if (hit) m_hits.emplace_back(hit);
    }

    void MuonCalibTrack_E::addHole(const CalibHolePtr& hole) {
        if (hole) m_holes.emplace_back(hole);
    }

    float MuonCalib::MuonCalibTrack_E::z0ip() const {
        // extrapolate z to IP
        MuonCalib::MuonCalibSLPropagator slProp;
        Amg::Vector3D posAtIp = slProp.propagateToPerigee(m_pos, m_dir);
        return posAtIp.z();
    }
    float MuonCalib::MuonCalibTrack_E::d0() const { return -x0() * std::sin(phi()) + y0() * std::cos(phi()); }

}  // namespace MuonCalib
