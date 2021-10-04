/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibHole_E.h"

namespace MuonCalib {

    MuonCalibHole_E::MuonCalibHole_E(const MuonFixedId& id, const Amg::Vector3D& pos) : m_id(id), m_pos(pos) {}

    const MuonFixedId& MuonCalibHole_E::identify() const { return m_id; }
    const Amg::Vector3D& MuonCalibHole_E::position() const { return m_pos; }
    void MuonCalibHole_E::setId(const MuonFixedId& id) { m_id = id; }
    void MuonCalibHole_E::setPosition(const Amg::Vector3D& pos) { m_pos = pos; }
}  // namespace MuonCalib
