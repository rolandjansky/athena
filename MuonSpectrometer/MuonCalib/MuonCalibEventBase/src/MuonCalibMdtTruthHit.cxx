/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"

namespace MuonCalib {

    MuonFixedId MuonCalibMdtTruthHit::identify() const { return m_ID; }
    int MuonCalibMdtTruthHit::barCode() const { return m_barCode; }
    double MuonCalibMdtTruthHit::driftRadius() const { return m_driftRadius; }
    double MuonCalibMdtTruthHit::positionAlongTube() const { return m_positionAlongTube; }
    double MuonCalibMdtTruthHit::gpositionX() const { return m_pos[0]; }
    double MuonCalibMdtTruthHit::gpositionY() const { return m_pos[1]; }
    double MuonCalibMdtTruthHit::gpositionZ() const { return m_pos[2]; }
    double MuonCalibMdtTruthHit::time() const { return m_time; }
    void MuonCalibMdtTruthHit::setIdentifier(MuonFixedId id) { m_ID = id; }
    void MuonCalibMdtTruthHit::setBarCode(int barCode) { m_barCode = barCode; }
    void MuonCalibMdtTruthHit::setDriftRadius(double driftRadius) { m_driftRadius = driftRadius; }
    void MuonCalibMdtTruthHit::setPositionAlongTube(double positionAlongTube) { m_positionAlongTube = positionAlongTube; }
    void MuonCalibMdtTruthHit::setgPositionX(double gpositionX) { m_pos[0] = gpositionX; }
    void MuonCalibMdtTruthHit::setgPositionY(double gpositionY) { m_pos[1] = gpositionY; }
    void MuonCalibMdtTruthHit::setgPositionZ(double gpositionZ) { m_pos[2] = gpositionZ; }
    void MuonCalibMdtTruthHit::setTime(double time) { m_time = time; }
    void MuonCalibMdtTruthHit::setgPosition(Amg::Vector3D newPos) { m_pos = std::move(newPos); }
    MuonCalibMdtTruthHit::MuonCalibMdtTruthHit(MuonFixedId id, int barCode, double driftRadius, double positionAlongTube,
                                               Amg::Vector3D globPos, double time) :
        m_ID{id}, m_barCode{barCode}, m_driftRadius{driftRadius}, m_positionAlongTube{positionAlongTube}, m_pos{std::move(globPos)}, m_time{time} {}

    std::ostream &MuonCalibMdtTruthHit::dump(std::ostream &stream) const {
        stream << "MuonCalibMdtTruthHit: " << std::endl;
        stream << "                 ID = " << identify() << std::endl;
        stream << "            barCode = " << barCode() << std::endl;
        stream << "        driftRadius = " << driftRadius() << std::endl;
        stream << "  positionAlongTube = " << positionAlongTube() << std::endl;
        stream << "  gpositionX = " << gpositionX() << std::endl;
        stream << "  gpositionY = " << gpositionY() << std::endl;
        stream << "  gpositionZ = " << gpositionZ() << std::endl;
        stream << "        Time = " << time() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibMdtTruthHit &truth) { return truth.dump(stream); }
