/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibCscTruthHit.h"

namespace MuonCalib {

    MuonCalibCscTruthHit::MuonCalibCscTruthHit() : m_ID(0), m_barCode(0), m_time(0.) {}

    MuonCalibCscTruthHit::MuonCalibCscTruthHit(MuonFixedId id, int barCode, double time) : m_ID(id), m_barCode(barCode), m_time(time) {}

    MuonCalibCscTruthHit::MuonCalibCscTruthHit(const MuonCalibCscTruthHit &truth) :
        m_ID(truth.identify()), m_barCode(truth.barCode()), m_time(truth.time()) {}

    MuonCalibCscTruthHit &MuonCalibCscTruthHit::operator=(const MuonCalibCscTruthHit &rhs) {
        if (this != &rhs) {
            m_ID = rhs.identify();
            m_barCode = rhs.barCode();
            m_time = rhs.time();
        }
        return (*this);
    }

    std::ostream &MuonCalibCscTruthHit::dump(std::ostream &stream) const {
        stream << "MuonCalibCscTruthHit: " << std::endl;
        stream << "                 ID = " << identify() << std::endl;
        stream << "            barCode = " << barCode() << std::endl;
        stream << "        Time = " << time() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibCscTruthHit &truth) { return truth.dump(stream); }
