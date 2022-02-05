/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawRpcCoin.h"

#include <iostream>

namespace MuonCalib {

    unsigned short MuonCalibRawRpcCoin::ijk() const { return m_pars.ijk; }
    unsigned short MuonCalibRawRpcCoin::threshold() const { return m_pars.threshold; }
    unsigned short MuonCalibRawRpcCoin::overlap() const { return m_pars.overlap; }
    unsigned short MuonCalibRawRpcCoin::parentCmId() const { return m_pars.parentCmId; }
    unsigned short MuonCalibRawRpcCoin::parentPadId() const { return m_pars.parentPadId; }
    unsigned short MuonCalibRawRpcCoin::parentSectorId() const { return m_pars.parentSectorId; }
    bool MuonCalibRawRpcCoin::lowPtCm() const { return m_pars.lowPtCm; }
    MuonCalibRawRpcCoin::MuonCalibRawRpcCoin(const MuonCalibRawRpcCoin::defPars &pars) : MuonCalibRawRpcHit(pars), m_pars{pars} {}

    std::ostream &MuonCalibRawRpcCoin::dump(std::ostream &stream) const {
        stream << "MuonCalibRawRpcCoin with" << std::endl;
        stream << "   identifier " << identify() << std::endl;
        stream << "   global position " << globalPosition() << std::endl;
        stream << "   occupancy " << occupancy() << std::endl;
        stream << "   time " << t() << std::endl;
        stream << "   width " << width() << std::endl;
        stream << "   length " << length() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibRawRpcCoin &hit) { return hit.dump(stream); }
