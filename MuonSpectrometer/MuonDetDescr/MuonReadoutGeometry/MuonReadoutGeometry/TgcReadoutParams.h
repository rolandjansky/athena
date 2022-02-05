/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREADOUTGEOMETRY_TGCREADOUTPARAMS_H
#define MUONREADOUTGEOMETRY_TGCREADOUTPARAMS_H

// ******************************************************************************
// class TgcReadoutParams
// ******************************************************************************
//
// Description
// -----------
// Provides access to the TGC readout parameters.  Created from one of the
// "sources": TGC_ZebraDetDescrSource, TGC_NovaDetDescrSource, etc.  One
// object is instantiated per chamber type and is pointed to by the
// TGC_DetDescriptor object corresponding to that type.
//
// Inheritance
// -----------
// None.
//
// Dependencies
// ------------
// None.
//
// ******************************************************************************

#include <array>
#include <string>

namespace MuonGM {

    class TgcReadoutParams {
    public:
        TgcReadoutParams(const std::string& name, int iCh, int Version, float WireSp, const float NCHRNG, const float* NWGS,
                         const float* IWGS1, const float* IWGS2, const float* IWGS3, const float* ROFFST, const float* NSPS,
                         const float* POFFST);

        // Another constructor for the layout Q
        TgcReadoutParams(const std::string& name, int iCh, int Version, float WireSp, const int NCHRNG, const float* NWGS,
                         const float* IWGS1, const float* IWGS2, const float* IWGS3, float PDIST, const float* SLARGE, const float* SSHORT,
                         const float* ROFFST, const float* NSPS, const float* POFFST);

        ~TgcReadoutParams();

        inline const std::string GetName() const;
        int chamberType() const;
        int readoutVersion() const;
        int nPhiChambers() const;
        int nGaps() const;

        // Access to wire gang parameters

        float wirePitch() const;
        inline float gangThickness() const;
        int nGangs(int gasGap) const;
        int totalWires(int gasGap) const;
        int nWires(int gasGap, int gang) const;
        int gangOffset(int gasGap) const;

        // Access to strip parameters

        inline float stripThickness() const;
        int nStrips(int gasGap) const;
        float stripOffset(int gasGap) const;
        float physicalDistanceFromBase() const;
        float stripPositionOnLargeBase(int strip) const;
        float stripPositionOnShortBase(int strip) const;

    private:
        // Readout array sizes
        enum TgcReadoutArraySizes { MaxNGaps = 3, MaxNGangs = 180, MaxNStrips = 33 };

        // Data members
        std::string m_chamberName{};
        int m_chamberType{0};
        int m_readoutVersion{0};
        float m_wirePitch{0.f};
        int m_nPhiChambers{0};

        std::array<int, MaxNGaps> m_nGangs{0};
        int m_nWires[MaxNGaps][MaxNGangs];
        std::array<int, MaxNGaps> m_gangOffset{0};
        std::array<int, MaxNGaps> m_nStrips{0};
        std::array<float, MaxNGaps> m_stripOffset{0.f};
        std::array<int, MaxNGaps> m_totalWires{0};

        // strip postion on the bases for the first layer in +Z
        float m_physicalDistanceFromBase{-9999.};
        std::array<float, MaxNStrips> m_stripPositionOnLargeBase{0.f};
        std::array<float, MaxNStrips> m_stripPositionOnShortBase{0.f};

        // Hard-coded data
        static constexpr float m_gangThickness = 0.05;
        static constexpr float m_stripThickness = 0.03;
    };
    float TgcReadoutParams::stripThickness() const { return m_stripThickness; }
    float TgcReadoutParams::gangThickness() const { return m_gangThickness; }

    const std::string TgcReadoutParams::GetName() const { return m_chamberName; }
}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_TGCREADOUTPARAMS_H
