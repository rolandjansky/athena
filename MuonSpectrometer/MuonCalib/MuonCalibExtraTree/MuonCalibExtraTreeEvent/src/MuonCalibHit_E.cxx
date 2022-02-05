/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"

namespace MuonCalib {
    MuonCalibHit_E::MuonCalibHit_E(const MuonFixedId& id, definePars pars) : m_id{id}, m_pars{std::move(pars)} {}

    const MuonFixedId& MuonCalibHit_E::identify() const { return m_id; }
    const Amg::Vector3D& MuonCalibHit_E::position() const { return m_pars.pos; }
    float MuonCalibHit_E::driftRadius() const { return m_pars.driftRadius; }
    float MuonCalibHit_E::error() const { return m_pars.error; }
    float MuonCalibHit_E::residual() const { return m_pars.resi; }
    float MuonCalibHit_E::pull() const { return m_pars.pull; }
    int MuonCalibHit_E::type() const { return m_pars.measType; }

    void MuonCalibHit_E::setId(const MuonFixedId& id) { m_id = id; }
    void MuonCalibHit_E::setPosition(const Amg::Vector3D& pos) { m_pars.pos = pos; }

    void MuonCalibHit_E::setDriftRadius(float driftRadius) { m_pars.driftRadius = driftRadius; }
    void MuonCalibHit_E::setError(float error) { m_pars.error = error; }
    void MuonCalibHit_E::setResidual(float res) { m_pars.resi = res; }

    void MuonCalibHit_E::setPull(float pull) { m_pars.pull = pull; }
    void MuonCalibHit_E::setType(int type) { m_pars.measType = type; }

}  // namespace MuonCalib
