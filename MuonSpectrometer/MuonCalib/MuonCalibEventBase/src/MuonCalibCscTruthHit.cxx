/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibCscTruthHit.h"

namespace MuonCalib {

    MuonCalibCscTruthHit::MuonCalibCscTruthHit(MuonFixedId id, int barCode, double time) : m_ID(id), m_barCode(barCode), m_time(time) {}

    std::ostream &MuonCalibCscTruthHit::dump(std::ostream &stream) const {
        stream << "MuonCalibCscTruthHit: " << std::endl;
        stream << "                 ID = " << identify() << std::endl;
        stream << "            barCode = " << barCode() << std::endl;
        stream << "        Time = " << time() << std::endl;
        return stream;
    }
    MuonFixedId MuonCalibCscTruthHit::identify() const { return m_ID; }
    int MuonCalibCscTruthHit::barCode() const { return m_barCode; }
    double MuonCalibCscTruthHit::time() const { return m_time; }

    void MuonCalibCscTruthHit::setIdentifier(MuonFixedId id) { m_ID = id; }
    void MuonCalibCscTruthHit::setBarCode(int barCode) { m_barCode = barCode; }
    void MuonCalibCscTruthHit::setTime(double time) { m_time = time; }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibCscTruthHit &truth) { return truth.dump(stream); }
