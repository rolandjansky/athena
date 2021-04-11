/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"

#include <iostream>

namespace MuonCalib {

    MuonCalibRawMdtHit::MuonCalibRawMdtHit(const MuonCalibRawMdtHit &hit) {
        m_id = hit.identify();
        m_Lpos = hit.localPosition();
        m_Gpos = hit.globalPosition();
        m_adc = hit.adcCount();
        m_tdc = hit.tdcCount();
        m_occupancy = hit.occupancy();
        m_t = hit.driftTime();
        m_r = hit.driftRadius();
        m_dr = hit.driftRadiusError();
    }

    MuonCalibRawMdtHit &MuonCalibRawMdtHit::operator=(const MuonCalibRawMdtHit &rhs) {
        if (this != &rhs) {
            m_id = rhs.identify();
            m_Lpos = rhs.localPosition();
            m_Gpos = rhs.globalPosition();
            m_adc = rhs.adcCount();
            m_tdc = rhs.tdcCount();
            m_occupancy = rhs.occupancy();
            m_t = rhs.driftTime();
            m_r = rhs.driftRadius();
            m_dr = rhs.driftRadiusError();
        }
        return (*this);
    }

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
