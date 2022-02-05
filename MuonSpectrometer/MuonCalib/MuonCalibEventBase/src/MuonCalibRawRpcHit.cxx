/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"

#include <iostream>

namespace MuonCalib {
    const MuonFixedId& MuonCalibRawRpcHit::identify() const { return m_pars.id; }
    const Amg::Vector3D& MuonCalibRawRpcHit::globalPosition() const { return m_pars.glob_pos; }
    int MuonCalibRawRpcHit::occupancy() const { return m_pars.occupancy; }
    float MuonCalibRawRpcHit::t() const { return m_pars.t; }
    float MuonCalibRawRpcHit::width() const { return m_pars.width; }
    float MuonCalibRawRpcHit::length() const { return m_pars.length; }
    void MuonCalibRawRpcHit::setId(MuonFixedId Id) { m_pars.id = Id; }
    void MuonCalibRawRpcHit::setGlobalPosition(const Amg::Vector3D& glob) { m_pars.glob_pos = glob; }
    void MuonCalibRawRpcHit::setOccupancy(int occ) { m_pars.occupancy = occ; }
    void MuonCalibRawRpcHit::setT(float t) { m_pars.t = t; }
    void MuonCalibRawRpcHit::setWidth(float width) { m_pars.width = width; }
    void MuonCalibRawRpcHit::setLength(float length) { m_pars.length = length; }
    MuonCalibRawRpcHit::MuonCalibRawRpcHit(const MuonCalibRawRpcHit::defineParams& pars) : m_pars{pars} {}

    std::ostream& MuonCalibRawRpcHit::dump(std::ostream& stream) const {
        stream << "MuonCalibRawRpcHit with" << std::endl;
        stream << "   identifier " << identify() << std::endl;
        stream << "   global position " << globalPosition() << std::endl;
        stream << "   occupancy " << occupancy() << std::endl;
        stream << "   time " << t() << std::endl;
        stream << "   width " << width() << std::endl;
        stream << "   length " << length() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawRpcHit& hit) { return hit.dump(stream); }
