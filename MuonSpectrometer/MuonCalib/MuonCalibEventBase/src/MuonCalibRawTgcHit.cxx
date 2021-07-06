/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"

#include <iostream>

namespace MuonCalib {

    MuonCalibRawTgcHit::MuonCalibRawTgcHit(const MuonCalibRawTgcHit& hit) {
        m_id = hit.identify();
        m_Gpos = hit.globalPosition();
        m_occupancy = hit.occupancy();
        m_station = hit.station();
        m_eta = hit.eta();
        m_phi = hit.phi();
        m_gasGap = hit.gasGap();
        m_isStrip = hit.isStrip();
        m_channel = hit.channel();
        m_bcTag = hit.bcTag();
        m_width = hit.width();
        m_shortWidth = hit.shortWidth();
        m_longWidth = hit.longWidth();
        m_length = hit.length();
    }

    MuonCalibRawTgcHit& MuonCalibRawTgcHit::operator=(const MuonCalibRawTgcHit& rhs) {
        if (this != &rhs) {
            m_id = rhs.identify();
            m_Gpos = rhs.globalPosition();
            m_occupancy = rhs.occupancy();
            m_station = rhs.station();
            m_eta = rhs.eta();
            m_phi = rhs.phi();
            m_gasGap = rhs.gasGap();
            m_isStrip = rhs.isStrip();
            m_channel = rhs.channel();
            m_bcTag = rhs.bcTag();
            m_width = rhs.width();
            m_shortWidth = rhs.shortWidth();
            m_longWidth = rhs.longWidth();
            m_length = rhs.length();
        }
        return (*this);
    }

    std::ostream& MuonCalibRawTgcHit::dump(std::ostream& stream) const {
        stream << "MuonCalibRawTgcHit with" << std::endl;
        stream << "   identifier " << identify() << std::endl;
        stream << "   global position " << globalPosition() << std::endl;
        stream << "   occupancy " << occupancy() << std::endl;
        stream << "   station " << station() << std::endl;
        stream << "   eta " << eta() << std::endl;
        stream << "   phi " << phi() << std::endl;
        stream << "   gasGap " << gasGap() << std::endl;
        stream << "   isStrip " << isStrip() << std::endl;
        stream << "   channel " << channel() << std::endl;
        stream << "   bcTag " << bcTag() << std::endl;
        stream << "   width " << width() << std::endl;
        stream << "   shortWidth " << shortWidth() << std::endl;
        stream << "   longWidth " << longWidth() << std::endl;
        stream << "   length " << length() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawTgcHit& hit) { return hit.dump(stream); }

//} //namespace MuonCalib
