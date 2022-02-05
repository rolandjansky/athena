/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawTgcCoin.h"

#include <iostream>

namespace MuonCalib {

    const MuonFixedId& MuonCalibRawTgcCoin::identify() const { return m_id; }
    const Amg::Vector3D& MuonCalibRawTgcCoin::gPosIn() const { return m_gPosIn; }
    const Amg::Vector3D& MuonCalibRawTgcCoin::gPosOut() const { return m_gPosOut; }
    int MuonCalibRawTgcCoin::type() const { return m_type; }
    int MuonCalibRawTgcCoin::eta() const { return m_eta; }
    int MuonCalibRawTgcCoin::phi() const { return m_phi; }
    int MuonCalibRawTgcCoin::sector() const { return m_sector; }
    int MuonCalibRawTgcCoin::isForward() const { return m_isForward; }
    int MuonCalibRawTgcCoin::isStrip() const { return m_isStrip; }
    int MuonCalibRawTgcCoin::trackletId() const { return m_trackletId; }
    int MuonCalibRawTgcCoin::trackletIdStrip() const { return m_trackletIdStrip; }
    int MuonCalibRawTgcCoin::bcTag() const { return m_bcTag; }
    float MuonCalibRawTgcCoin::widthIn() const { return m_widthIn; }
    float MuonCalibRawTgcCoin::widthOut() const { return m_widthOut; }
    float MuonCalibRawTgcCoin::widthR() const { return m_widthR; }
    float MuonCalibRawTgcCoin::widthPhi() const { return m_widthPhi; }
    int MuonCalibRawTgcCoin::delta() const { return m_delta; }
    int MuonCalibRawTgcCoin::roi() const { return m_roi; }
    int MuonCalibRawTgcCoin::pt() const { return m_pt; }
    int MuonCalibRawTgcCoin::sub() const { return m_sub; }
    int MuonCalibRawTgcCoin::isPositiveDeltaR() const { return m_isPositiveDeltaR; }

    void MuonCalibRawTgcCoin::setId(MuonFixedId Id) { m_id = Id; }
    void MuonCalibRawTgcCoin::setGlobalPositionIn(const Amg::Vector3D& gPosIn) { m_gPosIn = gPosIn; }
    void MuonCalibRawTgcCoin::setGlobalPositionOut(const Amg::Vector3D& gPosOut) { m_gPosOut = gPosOut; }
    void MuonCalibRawTgcCoin::setType(int type) { m_type = type; }
    void MuonCalibRawTgcCoin::setEta(int eta) { m_eta = eta; }
    void MuonCalibRawTgcCoin::setPhi(int phi) { m_phi = phi; }
    void MuonCalibRawTgcCoin::setSector(int sector) { m_sector = sector; }
    void MuonCalibRawTgcCoin::setIsForward(int isForward) { m_isForward = isForward; }
    void MuonCalibRawTgcCoin::setIsStrip(int isStrip) { m_isStrip = isStrip; }
    void MuonCalibRawTgcCoin::setTrackletId(int trackletId) { m_trackletId = trackletId; }
    void MuonCalibRawTgcCoin::setTrackletIdStrip(int trackletIdStrip) { m_trackletIdStrip = trackletIdStrip; }
    void MuonCalibRawTgcCoin::setBcTag(int bcTag) { m_bcTag = bcTag; }
    void MuonCalibRawTgcCoin::setWidthIn(float widthIn) { m_widthIn = widthIn; }
    void MuonCalibRawTgcCoin::setWidthOut(float widthOut) { m_widthOut = widthOut; }
    void MuonCalibRawTgcCoin::setWidthR(float widthR) { m_widthR = widthR; }
    void MuonCalibRawTgcCoin::setWidthPhi(float widthPhi) { m_widthPhi = widthPhi; }
    void MuonCalibRawTgcCoin::setDelta(int delta) { m_delta = delta; }
    void MuonCalibRawTgcCoin::setRoi(int roi) { m_roi = roi; }
    void MuonCalibRawTgcCoin::setPt(int pt) { m_pt = pt; }
    void MuonCalibRawTgcCoin::setSub(int sub) { m_sub = sub; }
    void MuonCalibRawTgcCoin::setIsPositiveDeltaR(int isPositiveDeltaR) { m_isPositiveDeltaR = isPositiveDeltaR; }

    MuonCalibRawTgcCoin::MuonCalibRawTgcCoin(const MuonFixedId& id, const Amg::Vector3D& gPosIn, const Amg::Vector3D& gPosOut,
                                             const int type, const int eta, const int phi, const int sector, const int isForward,
                                             const int isStrip, const int trackletId, const int trackletIdStrip, const int bcTag,
                                             const float widthIn, const float widthOut, const float widthR, const float widthPhi,
                                             const int delta, const int roi, const int pt, const int sub = 0,
                                             const int isPositiveDeltaR = 0) :
        m_id{id},
        m_gPosIn{gPosIn},
        m_gPosOut{gPosOut},
        m_type{type},
        m_eta{eta},
        m_phi{phi},
        m_sector{sector},
        m_isForward{isForward},
        m_isStrip{isStrip},
        m_trackletId{trackletId},
        m_trackletIdStrip{trackletIdStrip},
        m_bcTag{bcTag},
        m_widthIn{widthIn},
        m_widthOut{widthOut},
        m_widthR{widthR},
        m_widthPhi{widthPhi},
        m_delta{delta},
        m_roi{roi},
        m_pt{pt},
        m_sub{sub},
        m_isPositiveDeltaR{isPositiveDeltaR} {}

    std::ostream& MuonCalibRawTgcCoin::dump(std::ostream& stream) const {
        stream << "MuonCalibRawTgcCoin with" << std::endl;
        stream << "   identifier " << identify() << std::endl;
        stream << " gPosIn = " << gPosIn() << std::endl;
        stream << " gPosOut = " << gPosOut() << std::endl;
        stream << " type = " << type() << std::endl;
        stream << " eta = " << eta() << std::endl;
        stream << " phi = " << phi() << std::endl;
        stream << " sector = " << sector() << std::endl;
        stream << " isForward = " << isForward() << std::endl;
        stream << " isStrip = " << isStrip() << std::endl;
        stream << " trackletId = " << trackletId() << std::endl;
        stream << " trackletIdStrip = " << trackletIdStrip() << std::endl;
        stream << " widthIn = " << widthIn() << std::endl;
        stream << " widthOut = " << widthOut() << std::endl;
        stream << " widthR = " << widthR() << std::endl;
        stream << " widthPhi = " << widthPhi() << std::endl;
        stream << " delta = " << delta() << std::endl;
        stream << " roi = " << roi() << std::endl;
        stream << " pt = " << pt() << std::endl;
        stream << " sub = " << sub() << std::endl;
        stream << " isPositiveDeltaR = " << isPositiveDeltaR() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawTgcCoin& coin) { return coin.dump(stream); }
