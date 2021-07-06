/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"

#include <iostream>

namespace MuonCalib {

    MuonCalibRawRpcHit::MuonCalibRawRpcHit(const MuonCalibRawRpcHit &hit) {
        m_id = hit.identify();
        m_Gpos = hit.globalPosition();
        m_occupancy = hit.occupancy();
        m_t = hit.t();
        m_width = hit.width();
        m_length = hit.length();
    }

    MuonCalibRawRpcHit &MuonCalibRawRpcHit::operator=(const MuonCalibRawRpcHit &rhs) {
        if (this != &rhs) {
            m_id = rhs.identify();
            m_Gpos = rhs.globalPosition();
            m_occupancy = rhs.occupancy();
            m_t = rhs.t();
            m_width = rhs.width();
            m_length = rhs.length();
        }
        return (*this);
    }

    std::ostream &MuonCalibRawRpcHit::dump(std::ostream &stream) const {
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

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibRawRpcHit &hit) { return hit.dump(stream); }
