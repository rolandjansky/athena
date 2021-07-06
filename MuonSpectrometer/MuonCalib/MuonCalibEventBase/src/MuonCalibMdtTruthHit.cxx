/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"

namespace MuonCalib {

    MuonCalibMdtTruthHit::MuonCalibMdtTruthHit() :
        m_ID(0),
        m_barCode(0),
        m_driftRadius(0.),
        m_positionAlongTube(0.),
        m_gpositionX(0.),
        m_gpositionY(0.),
        m_gpositionZ(0.),
        m_time(0.) {}

    MuonCalibMdtTruthHit::MuonCalibMdtTruthHit(MuonFixedId id, int barCode, double driftRadius, double positionAlongTube, double gpositionX,
                                               double gpositionY, double gpositionZ, double time) :
        m_ID(id),
        m_barCode(barCode),
        m_driftRadius(driftRadius),
        m_positionAlongTube(positionAlongTube),
        m_gpositionX(gpositionX),
        m_gpositionY(gpositionY),
        m_gpositionZ(gpositionZ),
        m_time(time) {}

    MuonCalibMdtTruthHit::MuonCalibMdtTruthHit(const MuonCalibMdtTruthHit &truth) :
        m_ID(truth.identify()),
        m_barCode(truth.barCode()),
        m_driftRadius(truth.driftRadius()),
        m_positionAlongTube(truth.positionAlongTube()),
        m_gpositionX(truth.gpositionX()),
        m_gpositionY(truth.gpositionY()),
        m_gpositionZ(truth.gpositionZ()),
        m_time(truth.time()) {}

    MuonCalibMdtTruthHit &MuonCalibMdtTruthHit::operator=(const MuonCalibMdtTruthHit &rhs) {
        if (this != &rhs) {
            m_ID = rhs.identify();
            m_barCode = rhs.barCode();
            m_driftRadius = rhs.driftRadius();
            m_positionAlongTube = rhs.positionAlongTube();
            m_gpositionX = rhs.gpositionX();
            m_gpositionY = rhs.gpositionY();
            m_gpositionZ = rhs.gpositionZ();
            m_time = rhs.time();
        }
        return (*this);
    }

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
