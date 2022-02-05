/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"

namespace MuonCalib {

    MuonCalibCaloHit::MuonCalibCaloHit(int id, const Amg::Vector3D& pos, float time, float charge) :
        m_id{id}, m_pos{pos}, m_time{time}, m_charge{charge} {}

    int MuonCalibCaloHit::identify() const { return m_id; }
    const Amg::Vector3D& MuonCalibCaloHit::position() const { return m_pos; }
    float MuonCalibCaloHit::time() const { return m_time; }
    float MuonCalibCaloHit::charge() const { return m_charge; }

}  // namespace MuonCalib
