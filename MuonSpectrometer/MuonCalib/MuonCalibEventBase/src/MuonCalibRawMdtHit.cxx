/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"

#include <iostream>

namespace MuonCalib {

    const MuonFixedId &MuonCalibRawMdtHit::identify() const { return m_id; }
    const Amg::Vector3D &MuonCalibRawMdtHit::localPosition() const { return m_Lpos; }
    const Amg::Vector3D &MuonCalibRawMdtHit::globalPosition() const { return m_Gpos; }
    int MuonCalibRawMdtHit::adcCount() const { return m_adc; }
    int MuonCalibRawMdtHit::tdcCount() const { return m_tdc; }
    int MuonCalibRawMdtHit::occupancy() const { return m_occupancy; }
    double MuonCalibRawMdtHit::driftTime() const { return m_t; }
    double MuonCalibRawMdtHit::driftRadius() const { return m_r; }
    double MuonCalibRawMdtHit::driftRadiusError() const { return m_dr; }
    void MuonCalibRawMdtHit::setId(MuonFixedId Id) { m_id = Id; }
    void MuonCalibRawMdtHit::setLocalPosition(const Amg::Vector3D &loc) { m_Lpos = loc; }
    void MuonCalibRawMdtHit::setGlobalPosition(const Amg::Vector3D &glob) { m_Gpos = glob; }
    void MuonCalibRawMdtHit::setAdc(int adc) { m_adc = adc; }
    void MuonCalibRawMdtHit::setTdc(int tdc) { m_tdc = tdc; }
    void MuonCalibRawMdtHit::setOccupancy(int occ) { m_occupancy = occ; }
    void MuonCalibRawMdtHit::setDriftTime(double t) { m_t = t; }
    void MuonCalibRawMdtHit::setDriftRadius(double r) { m_r = r; }
    void MuonCalibRawMdtHit::setDriftRadiusError(double dr) { m_dr = dr; }

    MuonCalibRawMdtHit::MuonCalibRawMdtHit(const MuonFixedId &id, const Amg::Vector3D &loc, const Amg::Vector3D &glob, int occ) :
        m_id{id}, m_Lpos{loc}, m_Gpos{glob}, m_occupancy{occ} {}

    std::ostream &MuonCalibRawMdtHit::dump(std::ostream &stream) const {
        stream << "MuonCalibRawMdtHit with" << std::endl;
        stream << "   identifier " << identify() << std::endl;
        stream << "   local position " << localPosition() << std::endl;
        stream << "   global position " << globalPosition() << std::endl;
        stream << "   adc " << adcCount() << std::endl;
        stream << "   tdc " << tdcCount() << std::endl;
        stream << "   occupancy " << occupancy() << std::endl;
        stream << "   driftTime " << driftTime() << std::endl;
        stream << "   driftRadius " << driftRadius() << std::endl;
        stream << "   driftRadiusError " << driftRadiusError() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibRawMdtHit &hit) { return hit.dump(stream); }
