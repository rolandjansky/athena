/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawTgcCoin.h"

#include <iostream>

namespace MuonCalib {

    MuonCalibRawTgcCoin::MuonCalibRawTgcCoin(const MuonCalibRawTgcCoin &coin) {
        m_id = coin.identify();
        m_gPosIn = coin.gPosIn();
        m_gPosOut = coin.gPosOut();
        m_type = coin.type();
        m_eta = coin.eta();
        m_phi = coin.phi();
        m_sector = coin.sector();
        m_isForward = coin.isForward();
        m_isStrip = coin.isStrip();
        m_trackletId = coin.trackletId();
        m_trackletIdStrip = coin.trackletIdStrip();
        m_widthIn = coin.widthIn();
        m_widthOut = coin.widthOut();
        m_widthR = coin.widthR();
        m_widthPhi = coin.widthPhi();
        m_delta = coin.delta();
        m_roi = coin.roi();
        m_pt = coin.pt();
        m_sub = coin.sub();
        m_isPositiveDeltaR = coin.isPositiveDeltaR();
        m_bcTag = coin.m_bcTag;
    }

    MuonCalibRawTgcCoin &MuonCalibRawTgcCoin::operator=(const MuonCalibRawTgcCoin &rhs) {
        if (this != &rhs) {
            m_id = rhs.identify();
            m_gPosIn = rhs.gPosIn();
            m_gPosOut = rhs.gPosOut();
            m_type = rhs.type();
            m_eta = rhs.eta();
            m_phi = rhs.phi();
            m_sector = rhs.sector();
            m_isForward = rhs.isForward();
            m_isStrip = rhs.isStrip();
            m_trackletId = rhs.trackletId();
            m_trackletIdStrip = rhs.trackletIdStrip();
            m_widthIn = rhs.widthIn();
            m_widthOut = rhs.widthOut();
            m_widthR = rhs.widthR();
            m_widthPhi = rhs.widthPhi();
            m_delta = rhs.delta();
            m_roi = rhs.roi();
            m_pt = rhs.pt();
            m_sub = rhs.sub();
            m_isPositiveDeltaR = rhs.isPositiveDeltaR();
            m_bcTag = rhs.m_bcTag;
        }
        return (*this);
    }

    std::ostream &MuonCalibRawTgcCoin::dump(std::ostream &stream) const {
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

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibRawTgcCoin &coin) { return coin.dump(stream); }
