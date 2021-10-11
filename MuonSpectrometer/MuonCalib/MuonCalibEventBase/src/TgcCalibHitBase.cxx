/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Input Event Data Model
 * -----------------------------------------
 *
 * Author       : Zdenko van Kesteren
 * Creation Date: 17 October 2005
 ***************************************************************************/

// this package
#include "MuonCalibEventBase/TgcCalibHitBase.h"

namespace MuonCalib {

    void TgcCalibHitBase::setIdentifier(const MuonFixedId &id) { m_id = id; }

    void TgcCalibHitBase::setNStrips(int nStrips) { m_nStrips = nStrips; }

    void TgcCalibHitBase::setError(double error) { m_error = error; }

    void TgcCalibHitBase::setGlobalPos(const Amg::Vector3D &globalPos) { m_globalPosition = globalPos; }

    void TgcCalibHitBase::setLocalPos(const Amg::Vector3D &localPos) { m_localPosition = localPos; }

    void TgcCalibHitBase::setStripWidth(double stripWidth) { m_stripWidth = stripWidth; }

    void TgcCalibHitBase::setStripLength(double stripLength) { m_stripLength = stripLength; }

    const MuonFixedId &TgcCalibHitBase::identify() const { return m_id; }

    const Amg::Vector3D &TgcCalibHitBase::globalPosition() const { return m_globalPosition; }

    const Amg::Vector3D &TgcCalibHitBase::localPosition() const { return m_localPosition; }

    double TgcCalibHitBase::stripWidth() const { return m_stripWidth; }

    double TgcCalibHitBase::stripLength() const { return m_stripLength; }

    int TgcCalibHitBase::nStrips() const { return m_nStrips; }

    double TgcCalibHitBase::error() const { return m_error; }

    TgcCalibHitBase::TgcCalibHitBase(int nStrips, double stripWidth, double error, const Amg::Vector3D &globalPos,
                                     const Amg::Vector3D &localPos) :
        m_nStrips{nStrips},
        m_stripWidth{stripWidth},
        m_stripLength{0.},
        m_error{error},
        m_globalPosition{globalPos},
        m_localPosition{localPos} {}

    std::ostream &TgcCalibHitBase::dump(std::ostream &stream) const {
        stream << "TgcCalibHitBase: " << std::endl;
        stream << "   Number of Strips " << nStrips() << "   Strip width " << stripWidth() << "   Strip length " << stripLength()
               << "   Error " << error() << "   Global position " << globalPosition() << "   local position " << localPosition()
               << std::endl;
        return stream;
    }

    // All the other ostream operators are outside namespace MuonCalib, does it matter?
    std::ostream &operator<<(std::ostream &stream, const TgcCalibHitBase &hit) { return hit.dump(stream); }

}  // namespace MuonCalib
