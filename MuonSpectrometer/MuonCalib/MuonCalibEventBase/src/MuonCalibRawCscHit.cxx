/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawCscHit.h"

#include <iostream>

namespace MuonCalib {
    MuonCalibRawCscHit::MuonCalibRawCscHit(const MuonFixedId &id, const Amg::Vector3D &glob, int occ, float t, float width, float charge) :
        m_id{id}, m_Gpos{glob}, m_occupancy{occ}, m_t{t}, m_width{width}, m_charge{charge} {}

    const MuonFixedId &MuonCalibRawCscHit::identify() const { return m_id; }
    const Amg::Vector3D &MuonCalibRawCscHit::globalPosition() const { return m_Gpos; }
    int MuonCalibRawCscHit::occupancy() const { return m_occupancy; }
    float MuonCalibRawCscHit::t() const { return m_t; }
    float MuonCalibRawCscHit::width() const { return m_width; }
    float MuonCalibRawCscHit::charge() const { return m_charge; }

    void MuonCalibRawCscHit::setId(MuonFixedId Id) { m_id = Id; }
    void MuonCalibRawCscHit::setGlobalPosition(const Amg::Vector3D &glob) { m_Gpos = glob; }
    void MuonCalibRawCscHit::setOccupancy(int occ) { m_occupancy = occ; }
    void MuonCalibRawCscHit::setT(float t) { m_t = t; }
    void MuonCalibRawCscHit::setWidth(float width) { m_width = width; }
    void MuonCalibRawCscHit::setCharge(float charge) { m_charge = charge; }

    std::ostream &MuonCalibRawCscHit::dump(std::ostream &stream) const {
        stream << "MuonCalibRawCscHit with" << std::endl;
        stream << "   identifier " << identify() << std::endl;
        stream << "   global position " << globalPosition() << std::endl;
        stream << "   occupancy " << occupancy() << std::endl;
        stream << "   time " << t() << std::endl;
        stream << "   width " << width() << std::endl;
        stream << "   charge " << charge() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibRawCscHit &hit) { return hit.dump(stream); }
