/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibTgcTruthHit.h"

namespace MuonCalib {

    MuonCalibTgcTruthHit::MuonCalibTgcTruthHit() : m_ID(0), m_barCode(0), m_time(0.) {}

    MuonCalibTgcTruthHit::MuonCalibTgcTruthHit(MuonFixedId id, int barCode, double time) : m_ID(id), m_barCode(barCode), m_time(time) {}

    MuonCalibTgcTruthHit::MuonCalibTgcTruthHit(const MuonCalibTgcTruthHit &truth) :
        m_ID(truth.identify()), m_barCode(truth.barCode()), m_time(truth.time()) {}

    MuonCalibTgcTruthHit &MuonCalibTgcTruthHit::operator=(const MuonCalibTgcTruthHit &rhs) {
        if (this != &rhs) {
            m_ID = rhs.identify();
            m_barCode = rhs.barCode();
            m_time = rhs.time();
        }
        return (*this);
    }

    std::ostream &MuonCalibTgcTruthHit::dump(std::ostream &stream) const {
        stream << "MuonCalibTgcTruthHit: " << std::endl;
        stream << "                 ID = " << identify() << std::endl;
        stream << "            barCode = " << barCode() << std::endl;
        stream << "        Time = " << time() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibTgcTruthHit &truth) { return truth.dump(stream); }
