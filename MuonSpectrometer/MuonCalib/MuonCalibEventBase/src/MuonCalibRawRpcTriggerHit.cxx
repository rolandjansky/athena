/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawRpcTriggerHit.h"

#include <iostream>

namespace MuonCalib {

    MuonCalibRawRpcTriggerHit::MuonCalibRawRpcTriggerHit(const MuonCalibRawRpcTriggerHit& hit) :
        m_sector(hit.sector()),
        m_padId(hit.padId()),
        m_status(hit.status()),
        m_errorCode(hit.errorCode()),
        m_cmaId(hit.cmaId()),
        m_fel1Id(hit.fel1Id()),
        m_febcId(hit.febcId()),
        m_crc(hit.crc()),
        m_bcId(hit.bcId()),
        m_time(hit.time()),
        m_ijk(hit.ijk()),
        m_channel(hit.channel()),
        m_overlap(hit.overlap()),
        m_threshold(hit.threshold()) {}
    MuonCalibRawRpcTriggerHit::MuonCalibRawRpcTriggerHit(int sector, int padId, int status, int errorCode, int cmaId, int fel1Id,
                                                         int febcId, int crc, int bcId, int time, int ijk, int channel, int overlap,
                                                         int threshold) :
        m_sector(sector),
        m_padId(padId),
        m_status(status),
        m_errorCode(errorCode),
        m_cmaId(cmaId),
        m_fel1Id(fel1Id),
        m_febcId(febcId),
        m_crc(crc),
        m_bcId(bcId),
        m_time(time),
        m_ijk(ijk),
        m_channel(channel),
        m_overlap(overlap),
        m_threshold(threshold) {}

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
