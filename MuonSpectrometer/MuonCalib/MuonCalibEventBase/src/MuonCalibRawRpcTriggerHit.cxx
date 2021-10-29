/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawRpcTriggerHit.h"

#include <iostream>

namespace MuonCalib {

    int MuonCalibRawRpcTriggerHit::sector() const { return m_pars.sector; }
    int MuonCalibRawRpcTriggerHit::padId() const { return m_pars.padId; }
    int MuonCalibRawRpcTriggerHit::status() const { return m_pars.status; }
    int MuonCalibRawRpcTriggerHit::errorCode() const { return m_pars.errorCode; }
    int MuonCalibRawRpcTriggerHit::cmaId() const { return m_pars.cmaId; }
    int MuonCalibRawRpcTriggerHit::fel1Id() const { return m_pars.fel1Id; }
    int MuonCalibRawRpcTriggerHit::febcId() const { return m_pars.febcId; }
    int MuonCalibRawRpcTriggerHit::crc() const { return m_pars.crc; }
    int MuonCalibRawRpcTriggerHit::bcId() const { return m_pars.bcId; }
    int MuonCalibRawRpcTriggerHit::time() const { return m_pars.time; }
    int MuonCalibRawRpcTriggerHit::ijk() const { return m_pars.ijk; }
    int MuonCalibRawRpcTriggerHit::channel() const { return m_pars.channel; }
    int MuonCalibRawRpcTriggerHit::overlap() const { return m_pars.overlap; }
    int MuonCalibRawRpcTriggerHit::threshold() const { return m_pars.threshold; }

    MuonCalibRawRpcTriggerHit::MuonCalibRawRpcTriggerHit(const MuonCalibRawRpcTriggerHit::defineParams& pars) : m_pars{pars} {}

    std::ostream& MuonCalibRawRpcTriggerHit::dump(std::ostream& stream) const {
        stream << "MuonCalibRawRpcTriggerHit with" << std::endl;
        stream << "   sector " << sector() << std::endl;
        stream << "   padId " << padId() << std::endl;
        stream << "   status " << status() << std::endl;
        stream << "   errorCode " << errorCode() << std::endl;
        stream << "   cmaId " << cmaId() << std::endl;
        stream << "   fel1Id " << fel1Id() << std::endl;
        stream << "   febcId " << febcId() << std::endl;
        stream << "   crc " << crc() << std::endl;
        stream << "   bcId " << bcId() << std::endl;
        stream << "   time " << time() << std::endl;
        stream << "   ijk " << ijk() << std::endl;
        stream << "   channel " << channel() << std::endl;
        stream << "   overlap " << overlap() << std::endl;
        stream << "   threshold " << threshold() << std::endl;

        return stream;
    }

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawRpcTriggerHit& hit) { return hit.dump(stream); }
