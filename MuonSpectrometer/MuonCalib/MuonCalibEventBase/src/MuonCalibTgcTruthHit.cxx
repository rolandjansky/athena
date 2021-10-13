/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibTgcTruthHit.h"

namespace MuonCalib {

    const MuonFixedId &MuonCalibTgcTruthHit::identify() const { return m_ID; }
    int MuonCalibTgcTruthHit::barCode() const { return m_barCode; }
    double MuonCalibTgcTruthHit::time() const { return m_time; }

    void MuonCalibTgcTruthHit::setIdentifier(MuonFixedId id) { m_ID = id; }
    void MuonCalibTgcTruthHit::setBarCode(int barCode) { m_barCode = barCode; }
    void MuonCalibTgcTruthHit::setTime(double time) { m_time = time; }

    MuonCalibTgcTruthHit::MuonCalibTgcTruthHit(MuonFixedId id, int barCode, double time) : m_ID{id}, m_barCode{barCode}, m_time{time} {}

    std::ostream &MuonCalibTgcTruthHit::dump(std::ostream &stream) const {
        stream << "MuonCalibTgcTruthHit: " << std::endl;
        stream << "                 ID = " << identify() << std::endl;
        stream << "            barCode = " << barCode() << std::endl;
        stream << "        Time = " << time() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibTgcTruthHit &truth) { return truth.dump(stream); }
