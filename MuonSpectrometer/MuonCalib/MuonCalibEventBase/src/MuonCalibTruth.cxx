/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibTruth.h"

#include <iostream>

namespace MuonCalib {

    MuonCalibTruth::MuonCalibTruth(const Amg::Vector3D& pos, const Amg::Vector3D& momentum, double kinEnergy, int code, int barCode) :
        m_position{pos}, m_momentum{momentum}, m_kinEnergy{kinEnergy}, m_PDGCode{code}, m_barCode{barCode} {}

    void MuonCalibTruth::setPosition(const Amg::Vector3D& pos) { m_position = pos; }

    void MuonCalibTruth::setMomentum(const Amg::Vector3D& mom) { m_momentum = mom; }

    void MuonCalibTruth::setKinEnergy(double kinEnergy) { m_kinEnergy = kinEnergy; }

    void MuonCalibTruth::setPDGCode(int code) { m_PDGCode = code; }

    void MuonCalibTruth::setBarCode(int barCode) { m_barCode = barCode; }

    const Amg::Vector3D& MuonCalibTruth::position() const { return m_position; }

    const Amg::Vector3D& MuonCalibTruth::momentum() const { return m_momentum; }

    double MuonCalibTruth::kinEnergy() const { return m_kinEnergy; }

    int MuonCalibTruth::PDGCode() const { return m_PDGCode; }

    int MuonCalibTruth::barCode() const { return m_barCode; }
    std::ostream& MuonCalibTruth::dump(std::ostream& stream) const {
        stream << "MuonCalibTruth with" << std::endl;
        stream << "   position " << position() << std::endl;
        stream << "   momentum " << momentum() << std::endl;
        stream << "   kinEnergy " << kinEnergy() << " , PDGCode " << PDGCode() << " and barCode " << barCode() << std::endl;

        return stream;
    }

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibTruth& truth) { return truth.dump(stream); }
