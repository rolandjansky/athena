/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// Atlas Muon Detector Description
// -----------------------------------------
// ******************************************************************************

#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include <GaudiKernel/IMessageSvc.h>

#include <utility>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace MuonGM {

    TgcReadoutParams::TgcReadoutParams(const std::string& name, int iCh, int Version, float WireSp, const float NCHRNG, const float* NWGS,
                                       const float* IWGS1, const float* IWGS2, const float* IWGS3, const float* ROFFST, const float* NSPS,
                                       const float* POFFST) :
        m_chamberName(name), m_chamberType(iCh), m_readoutVersion(Version), m_wirePitch(WireSp), m_nPhiChambers((int)NCHRNG) {
        for (int iGap = 0; iGap < MaxNGaps; ++iGap) {
            m_nGangs[iGap] = (int)NWGS[iGap];
            m_gangOffset[iGap] = (int)ROFFST[iGap];
            m_nStrips[iGap] = (int)NSPS[iGap];
            m_stripOffset[iGap] = POFFST[iGap];
            m_totalWires[iGap] = 0;
        }
        for (int iGang = 0; iGang < MaxNGangs; ++iGang) {
            if (iGang < m_nGangs[0]) {
                m_nWires[0][iGang] = (int)IWGS1[iGang];
                m_totalWires[0] += (int)IWGS1[iGang];
            } else
                m_nWires[0][iGang] = 0;

            if (iGang < m_nGangs[1]) {
                m_nWires[1][iGang] = (int)IWGS2[iGang];
                m_totalWires[1] += (int)IWGS2[iGang];
            } else
                m_nWires[1][iGang] = 0;

            if (iGang < m_nGangs[2]) {
                m_nWires[2][iGang] = (int)IWGS3[iGang];
                m_totalWires[2] += (int)IWGS3[iGang];
            } else
                m_nWires[2][iGang] = 0;
        }
    }

    TgcReadoutParams::TgcReadoutParams(const std::string& name, int iCh, int Version, float WireSp, const int NCHRNG, const float* NWGS,
                                       const float* IWGS1, const float* IWGS2, const float* IWGS3, float PDIST, const float* SLARGE,
                                       const float* SSHORT, const float* ROFFST, const float* NSPS, const float* POFFST) :
        m_chamberName(name), m_chamberType(iCh), m_readoutVersion(Version), m_wirePitch(WireSp), m_nPhiChambers(NCHRNG) {
        for (int iGap = 0; iGap < MaxNGaps; ++iGap) {
            m_nGangs[iGap] = (int)NWGS[iGap];
            m_gangOffset[iGap] = (int)ROFFST[iGap];
            m_nStrips[iGap] = (int)NSPS[iGap];
            m_stripOffset[iGap] = POFFST[iGap];
            m_totalWires[iGap] = 0;
        }
        for (int iGang = 0; iGang < MaxNGangs; ++iGang) {
            if (iGang < m_nGangs[0]) {
                m_nWires[0][iGang] = (int)IWGS1[iGang];
                m_totalWires[0] += (int)IWGS1[iGang];
            } else
                m_nWires[0][iGang] = 0;
            if (iGang < m_nGangs[1]) {
                m_nWires[1][iGang] = (int)IWGS2[iGang];
                m_totalWires[1] += (int)IWGS2[iGang];
            } else
                m_nWires[1][iGang] = 0;
            if (iGang < m_nGangs[2]) {
                m_nWires[2][iGang] = (int)IWGS3[iGang];
                m_totalWires[2] += (int)IWGS3[iGang];
            } else
                m_nWires[2][iGang] = 0;
        }

        m_physicalDistanceFromBase = PDIST;

        for (int iStrip = 0; iStrip < MaxNStrips; ++iStrip) {
            m_stripPositionOnLargeBase[iStrip] = SLARGE[iStrip];
            m_stripPositionOnShortBase[iStrip] = SSHORT[iStrip];
        }
    }

    TgcReadoutParams::~TgcReadoutParams() = default;

    // Access to general parameters

    int TgcReadoutParams::chamberType() const { return m_chamberType; }

    int TgcReadoutParams::readoutVersion() const { return m_readoutVersion; }

    int TgcReadoutParams::nPhiChambers() const { return m_nPhiChambers; }

    int TgcReadoutParams::nGaps() const {
        int nGaps = 2;
        if (nStrips(3) > 1) nGaps = 3;
        return nGaps;
    }

    // Access to wire gang parameters

    float TgcReadoutParams::wirePitch() const { return m_wirePitch; }

    int TgcReadoutParams::nGangs(int gasGap) const {
        if (gasGap < 1 || gasGap > MaxNGaps) {
            MsgStream log(Athena::getMessageSvc(), "TgcReadoutParams");
            log << MSG::WARNING << "TgcReadoutParams::nGangs(" << gasGap << ") gasGap out of allowed range: 1-" << MaxNGaps << endmsg;
#ifndef NDEBUG
            throw std::out_of_range("input gas gap index is incorrect");
#endif
            return 0;
        }
        return m_nGangs[gasGap - 1];
    }

    int TgcReadoutParams::totalWires(int gasGap) const {
        if (gasGap < 1 || gasGap > MaxNGaps) {
            MsgStream log(Athena::getMessageSvc(), "TgcReadoutParams");
            log << MSG::WARNING << "TgcReadoutParams::totalWires(" << gasGap << ") gasGap out of allowed range: 1-" << MaxNGaps << endmsg;
#ifndef NDEBUG
            throw std::out_of_range("input gas gap index is incorrect");
#endif
            return 0;
        }
        return m_totalWires[gasGap - 1];
    }

    int TgcReadoutParams::nWires(int gasGap, int gang) const {
        if (gasGap < 1 || gasGap > MaxNGaps || gang < 1 || gang > MaxNGangs) {
            MsgStream log(Athena::getMessageSvc(), "TgcReadoutParams");
            log << MSG::WARNING << "TgcReadoutParams::nWires gasGap " << gasGap << " or gang " << gang << " out of allowed range" << endmsg;
#ifndef NDEBUG
            throw std::out_of_range("input gas gap or wire gang index are incorrect");
#endif
            return 0;
        }
        return m_nWires[gasGap - 1][gang - 1];
    }

    int TgcReadoutParams::gangOffset(int gasGap) const {
        if (gasGap < 1 || gasGap > MaxNGaps) {
            MsgStream log(Athena::getMessageSvc(), "TgcReadoutParams");
            log << MSG::WARNING << "TgcReadoutParams::gangOffset(" << gasGap << ") gasGap out of allowed range: 1-" << MaxNGaps << endmsg;
#ifndef NDEBUG
            throw std::out_of_range("input gas gap index is incorrect");
#endif
            return 0;
        }
        return m_gangOffset[gasGap - 1];
    }

    // Access to strip parameters
    int TgcReadoutParams::nStrips(int gasGap) const {
        if (gasGap < 1 || gasGap > MaxNGaps) {
            MsgStream log(Athena::getMessageSvc(), "TgcReadoutParams");
            log << MSG::WARNING << "TgcReadoutParams::nStrips(" << gasGap << ") gasGap out of allowed range: 1-" << MaxNGaps << endmsg;
#ifndef NDEBUG
            throw std::out_of_range("input gas gap index is incorrect");
#endif
            return 0;
        }
        return m_nStrips[gasGap - 1];
    }

    float TgcReadoutParams::stripOffset(int gasGap) const {
        if (gasGap < 1 || gasGap > MaxNGaps) {
            MsgStream log(Athena::getMessageSvc(), "TgcReadoutParams");
            log << MSG::WARNING << "TgcReadoutParams::stripOffset(" << gasGap << ") gasGap out of allowed range: 1-" << MaxNGaps << endmsg;
#ifndef NDEBUG
            throw std::out_of_range("input gas gap index is incorrect");
#endif
            return 0.;
        }
        return m_stripOffset[gasGap - 1];
    }

    float TgcReadoutParams::physicalDistanceFromBase() const { return m_physicalDistanceFromBase; }

    float TgcReadoutParams::stripPositionOnLargeBase(int istrip) const {
        // all gas gaps have the same n. of strips (=> check the first one)
        if (istrip <= m_nStrips[0] + 1)
            return m_stripPositionOnLargeBase[istrip - 1];
        else {
            MsgStream log(Athena::getMessageSvc(), "TgcReadoutParams");
            log << MSG::WARNING << "Input strip n. " << istrip
                << " out of range in TgcReadoutParams::stripPositionOnLargeBase for TgcReadoutParams of name/type " << m_chamberName << "/"
                << m_chamberType << "  - Nstrips = " << m_nStrips[0] << " MaxNStrips = " << MaxNStrips << endmsg;
#ifndef NDEBUG
            throw std::out_of_range(
                "input strip index >= m_nStrips[0]+1 (remember that positions on short/large bases are given for 33 elements [begin of "
                "each strip + end of last one])");
#endif
            return m_stripPositionOnLargeBase[0];  // if invalid input, return the first strip}
        }
    }

    float TgcReadoutParams::stripPositionOnShortBase(int istrip) const {
        // all gas gaps have the same n. of strips (=> check the first one)
        if (istrip <= m_nStrips[0] + 1)
            return m_stripPositionOnShortBase[istrip - 1];
        else {
            MsgStream log(Athena::getMessageSvc(), "TgcReadoutParams");
            log << MSG::WARNING << "Input strip n. " << istrip
                << " out of range in TgcReadoutParams::stripPositionOnShortBase for TgcReadoutParams of name/type " << m_chamberName << "/"
                << m_chamberType << "  - Nstrips = " << m_nStrips[0] << " MaxNStrips = " << MaxNStrips << endmsg;
#ifndef NDEBUG
            throw std::out_of_range(
                "input strip index >= m_nStrips[0]+1 (remember that positions on short/large bases are given for 33 elements [begin of "
                "each strip + end of last one])");
#endif
            return m_stripPositionOnShortBase[0];  //  if invalid input, return the first strip
        }
    }
}  // namespace MuonGM
