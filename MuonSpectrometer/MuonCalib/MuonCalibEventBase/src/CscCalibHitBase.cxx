/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// this package
#include "MuonCalibEventBase/CscCalibHitBase.h"

namespace MuonCalib {

    CscCalibHitBase::CscCalibHitBase(int nStrips, double stripWidth, int charge, double error, const Amg::Vector3D &globalPos,
                                     const Amg::Vector3D &localPos) :
        m_nStrips(nStrips),
        m_stripWidth(stripWidth),
        m_charge(charge),
        m_error(error),
        m_globalPosition(globalPos),
        m_localPosition(localPos) {}

    std::ostream &CscCalibHitBase::dump(std::ostream &stream) const {
        stream << "CscCalibHitBase: " << std::endl;
        stream << "   Number of Strips " << nStrips() << "   Strip width " << stripWidth() << "   Charge " << charge() << "   Error "
               << error() << "   Global position " << globalPosition() << "   Local position " << localPosition() << std::endl;
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const CscCalibHitBase &hit) { return hit.dump(stream); }
    void CscCalibHitBase::setIdentifier(const MuonFixedId &id) { m_id = id; }
    void CscCalibHitBase::setNStrips(int nStrips) { m_nStrips = nStrips; }
    void CscCalibHitBase::setStripWidth(double stripWidth) { m_stripWidth = stripWidth; }
    void CscCalibHitBase::setCharge(int charge) { m_charge = charge; }
    void CscCalibHitBase::setError(double error) { m_error = error; }
    void CscCalibHitBase::setGlobalPos(const Amg::Vector3D &globalPos) { m_globalPosition = globalPos; }
    void CscCalibHitBase::setLocalPos(const Amg::Vector3D &localPos) { m_localPosition = localPos; }
    const MuonFixedId &CscCalibHitBase::identify() const { return m_id; }
    int CscCalibHitBase::nStrips() const { return m_nStrips; }
    double CscCalibHitBase::stripWidth() const { return m_stripWidth; }
    int CscCalibHitBase::charge() const { return m_charge; }
    double CscCalibHitBase::error() const { return m_error; }
    const Amg::Vector3D &CscCalibHitBase::globalPosition() const { return m_globalPosition; }
    const Amg::Vector3D &CscCalibHitBase::localPosition() const { return m_localPosition; }
}  // namespace MuonCalib
