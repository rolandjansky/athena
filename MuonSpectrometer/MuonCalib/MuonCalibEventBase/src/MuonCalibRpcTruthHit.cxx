/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRpcTruthHit.h"

namespace MuonCalib {

    const MuonFixedId &MuonCalibRpcTruthHit::identify() const { return m_ID; }
    int MuonCalibRpcTruthHit::barCode() const { return m_barCode; }
    double MuonCalibRpcTruthHit::time() const { return m_time; }

    void MuonCalibRpcTruthHit::setIdentifier(MuonFixedId id) { m_ID = id; }
    void MuonCalibRpcTruthHit::setBarCode(int barCode) { m_barCode = barCode; }
    void MuonCalibRpcTruthHit::setTime(double time) { m_time = time; }

    MuonCalibRpcTruthHit::MuonCalibRpcTruthHit(MuonFixedId id, int barCode, double time) : m_ID{id}, m_barCode{barCode}, m_time{time} {}
    std::ostream &MuonCalibRpcTruthHit::dump(std::ostream &stream) const {
        stream << "MuonCalibRpcTruthHit: " << std::endl;
        stream << "                 ID = " << identify() << std::endl;
        stream << "            barCode = " << barCode() << std::endl;
        stream << "        Time = " << time() << std::endl;
        return stream;
    }

}  // end namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibRpcTruthHit &truth) { return truth.dump(stream); }
