/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibTruth.h"

#include <iostream>

namespace MuonCalib {

    MuonCalibTruth::MuonCalibTruth(const MuonCalibTruth &truth) {
        m_position = truth.position();
        m_momentum = truth.momentum();
        m_kinEnergy = truth.kinEnergy();
        m_PDGCode = truth.PDGCode();
        m_barCode = truth.barCode();
    }

    MuonCalibTruth &MuonCalibTruth::operator=(const MuonCalibTruth &rhs) {
        if (this != &rhs) {
            m_position = rhs.position();
            m_momentum = rhs.momentum();
            m_kinEnergy = rhs.kinEnergy();
            m_PDGCode = rhs.PDGCode();
            m_barCode = rhs.barCode();
        }
        return (*this);
    }

    std::ostream &MuonCalibTruth::dump(std::ostream &stream) const {
        stream << "MuonCalibTruth with" << std::endl;
        stream << "   position " << position() << std::endl;
        stream << "   momentum " << momentum() << std::endl;
        stream << "   kinEnergy " << kinEnergy() << " , PDGCode " << PDGCode() << " and barCode " << barCode() << std::endl;

        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibTruth &truth) { return truth.dump(stream); }
