/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Input Event Data Model
 ***************************************************************************/

// this package
#include "MuonCalibEventBase/RpcCalibHitBase.h"

namespace MuonCalib {

    void RpcCalibHitBase::setIdentifier(const MuonFixedId &id) { m_id = id; }
    void RpcCalibHitBase::setNStrips(int nStrips) { m_nStrips = nStrips; }
    void RpcCalibHitBase::setStripWidth(double stripWidth) { m_stripWidth = stripWidth; }
    void RpcCalibHitBase::setStripLength(double stripLength) { m_stripLength = stripLength; }
    void RpcCalibHitBase::setTime(double time) { m_time = time; }
    void RpcCalibHitBase::setError(double error) { m_error = error; }
    void RpcCalibHitBase::setGlobalPos(const Amg::Vector3D &globalPos) { m_globalPosition = globalPos; }
    void RpcCalibHitBase::setLocalPos(const Amg::Vector3D &localPos) { m_localPosition = localPos; }
    void RpcCalibHitBase::setDistanceToRO(const double distance) { m_distanceToRO = distance; }
    const MuonFixedId &RpcCalibHitBase::identify() const { return m_id; }
    int RpcCalibHitBase::nStrips() const { return m_nStrips; }
    double RpcCalibHitBase::stripWidth() const { return m_stripWidth; }
    double RpcCalibHitBase::stripLength() const { return m_stripLength; }
    double RpcCalibHitBase::time() const { return m_time; }
    double RpcCalibHitBase::error() const { return m_error; }
    const Amg::Vector3D &RpcCalibHitBase::globalPosition() const { return m_globalPosition; }
    const Amg::Vector3D &RpcCalibHitBase::localPosition() const { return m_localPosition; }
    double RpcCalibHitBase::distanceToRO() const { return m_distanceToRO; }
    RpcCalibHitBase::RpcCalibHitBase(int nStrips, double stripWidth, double time, double error, const Amg::Vector3D &globalPos,
                                     const Amg::Vector3D &localPos) :
        m_nStrips{nStrips},
        m_stripWidth{stripWidth},
        m_time{time},
        m_error{error},
        m_globalPosition{globalPos},
        m_localPosition{localPos} {}

    RpcCalibHitBase::RpcCalibHitBase(int nStrips, double stripWidth, double stripLength, double time, double error,
                                     const Amg::Vector3D &globalPos, const Amg::Vector3D &localPos) :
        m_nStrips{nStrips},
        m_stripWidth{stripWidth},
        m_stripLength{stripLength},
        m_time{time},
        m_error{error},
        m_globalPosition{globalPos},
        m_localPosition{localPos} {}

    std::ostream &RpcCalibHitBase::dump(std::ostream &stream) const {
        stream << "RpcCalibHitBase: " << std::endl;
        stream << "   Number of Strips " << nStrips() << "   Strip width " << stripWidth() << "   Strip length " << stripLength()
               << "   Time " << time() << "   Error " << error() << "   Global position " << globalPosition() << "   local position "
               << localPosition() << "   distance to readout " << distanceToRO() << std::endl;
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const RpcCalibHitBase &hit) { return hit.dump(stream); }

}  // namespace MuonCalib
