/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawCscHit.h"

#include <iostream>

namespace MuonCalib {

    MuonCalibRawCscHit::MuonCalibRawCscHit(const MuonCalibRawCscHit &hit) {
        m_id = hit.identify();
        m_Gpos = hit.globalPosition();
        m_occupancy = hit.occupancy();
        m_t = hit.t();
        m_width = hit.width();
        m_charge = hit.charge();
    }

    MuonCalibRawCscHit &MuonCalibRawCscHit::operator=(const MuonCalibRawCscHit &rhs) {
        if (this != &rhs) {
            m_id = rhs.identify();
            m_Gpos = rhs.globalPosition();
            m_occupancy = rhs.occupancy();
            m_t = rhs.t();
            m_width = rhs.width();
            m_charge = rhs.charge();
        }
        return (*this);
    }

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
