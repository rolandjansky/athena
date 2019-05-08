/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKEBKGTOOLS_FAKEBKGINTERNALS_H
#define FAKEBKGTOOLS_FAKEBKGINTERNALS_H

#include <string>
#include <vector>
#include <bitset>
#include <cmath>
#include <functional>
#include <memory>
#include <boost/container/flat_map.hpp>

namespace CP
{
    class BaseFakeBkgTool;
}

namespace FakeBkgTools
{
    
struct Uncertainty
{
    float up = 0, down = 0;
    Uncertainty() {} /// for older compilers
    Uncertainty(float init_up, float init_down) : up(init_up), down(init_down) {} /// for older compilers
    inline Uncertainty& operator+=(const Uncertainty& rhs);
    inline Uncertainty& operator*=(float rhs);
    friend Uncertainty operator*(float lhs, const Uncertainty& rhs) { return {lhs*rhs.up, lhs*rhs.down}; }
};

/// \brief a structure to hold an efficiency together with a variable number of uncertainties
struct Efficiency
{
    float value(const CP::BaseFakeBkgTool* tool) const;
    float nominal = 0;
    boost::container::flat_map<uint16_t, FakeBkgTools::Uncertainty> uncertainties; /// key = source of uncertainty (ID), value = up/down
};

using FakeFactor = Efficiency;

/// \brief a structure to hold a weight together with a variable number of systematic uncertainties
struct Weight : public Efficiency
{
    /// \brief Helper function to extract the total systematic uncertainty from the 'uncertainties' field;
    /// it is computed as the quadratic sum of all symmetrized uncertainties
    inline float syst() const;
};

/// \brief a structure to hold an event yield together with a statistical uncertainty and a variable number of systematic uncertainties
struct Yield : public Efficiency
{
    Uncertainty stat2;
    /// add() treats systematic uncertainties as correlated and updates (or creates) the total statistical uncertainty (RMS)
    inline Yield& add(const Yield& rhs, float weight = 1.f);
    inline Yield& add(const Weight& rhs, float weight = 1.f);
    /// Helper function to extract the total statistical uncertainty from the 'uncertainties' field;
    /// it returns the symmetrized uncertainties[Uncertainty::totalStat()] if present, else fabs(value) (not 0!)
    inline float stat() const;
    /// Helper function to extract the total systematic uncertainty from the 'uncertainties' field;
    /// it is computed as the quadratic sum of all symmetrized systematic uncertainties
    inline float syst() const;
};


struct ParticleData
{
    Efficiency real_efficiency, fake_efficiency;
    FakeFactor fake_factor;
    bool tight;
    int8_t charge;
};

inline constexpr uint8_t maxParticles() { return 6; }
inline constexpr uint64_t maxCombinations() { return 1 << maxParticles(); }
using FSBitset = std::bitset<maxCombinations()>;

class FinalState
{
public:
    mutable FSBitset selection = 0;
    FinalState(size_t h) : m_hash(h) {} // for search by hash
    FinalState(size_t h, const unsigned nparticles, const std::string& strSelection, const std::string& strProc, std::string& error); // main constructor
    /// Important: the accept() function doesn't check the selection (for speed reason), this has to be done separately
    bool accept_selection(const FSBitset& tights, const FSBitset& charges) const
    {
        if(!selection[tights.to_ulong()]) return false;
        auto nT = tights.count();
        if(hasSS() && nT==2 && (tights&charges).count()==1) return false; /// SS tight
        if(hasOS() && nT>=2 && ((tights&charges).count()%nT)==0) return false; /// OS tight
        return true;
    }
    bool accept_process(uint8_t nparticles, const FSBitset& reals, const FSBitset& tights) const
    {
        auto nT = tights.count();
        auto nR = reals.count(), nRT = (reals&tights).count(), nRL = nR - nRT;
        auto nF = nparticles - nR, nFT = nT - nRT, nFL = nF - nFT;
        uint32_t w = nRT | (nRL<<5) | (nR<<10) | (nFT<<15) | (nFL<<20) | (nF<<25); /// 4 bits per number + extra bit for carry
        if((m_wmax-w)&0x21084210 || (w-m_wmin)&0x21084210) return false;
        return true;
    }
    FSBitset retrieveCharges(const std::vector<FakeBkgTools::ParticleData>& particles) const
    {
        FSBitset charges{0};
        for(unsigned i=0;i<particles.size();++i) charges.set(i, particles[i].charge>0);
        return charges;
    }
    bool hasSS() const { return m_wmin & 0x40000000; }
    bool hasOS() const { return m_wmax & 0x40000000; }
    void setSS(bool set = true) { m_wmin = set? (m_wmin|0x40000000) : m_wmin&~0x40000000; }
    void setOS(bool set = true) { m_wmax = set? (m_wmax|0x40000000) : m_wmax&~0x40000000; }
    bool hasChargeRequirement() const { return hasOS() || hasSS(); }
    bool operator<(const FinalState& rhs) const { return m_hash < rhs.m_hash; }
private:
    size_t m_hash;
    mutable uint32_t m_wmin = 0x0, m_wmax=0x3FFFFFFF;
    bool parseProcess(std::string process, std::string& error);
    bool parseSelection(const unsigned short nparticles, std::string strSelection, std::string& error);
};

enum class Client
{
    NONE, MATRIX_METHOD, FAKE_FACTOR, ALL_METHODS
};

inline Uncertainty& Uncertainty::operator+=(const Uncertainty& rhs)
{
    up += rhs.up;
    down += rhs.down;
    return *this;
}

inline Uncertainty& Uncertainty::operator*=(float rhs)
{
    up *= rhs;
    down *= rhs;
    return *this;
}

inline float Weight::syst() const
{
    double syst2 = 0;
    for(auto& kv : uncertainties) syst2 += std::pow(0.5*(kv.second.up+kv.second.down), 2);
    return sqrt(syst2);
}

inline Yield& Yield::add(const Weight& rhs, float weight)
{
    /// Systematic uncertainties
    for(auto& kv : rhs.uncertainties)
    {
        auto r = uncertainties.emplace(kv);
        if(r.second) r.first->second *= weight;
        else r.first->second += weight * kv.second;
    }
    /// Statistical uncertainty ("RMS of weights")
    float ds2 = pow(weight * rhs.nominal, 2);
    stat2.up += ds2;
    stat2.down += ds2;
    /// Central value
    nominal += weight * rhs.nominal;
    return *this;
}

inline Yield& Yield::add(const Yield& rhs, float weight)
{
    /// Systematic uncertainties
    for(auto& kv : rhs.uncertainties)
    {
        auto r = uncertainties.emplace(kv);
        if(r.second) r.first->second *= weight;
        else r.first->second += weight * kv.second;
    }
    /// Statistical uncertainty ("RMS of weights")
    stat2.up += weight * weight * rhs.stat2.up;
    stat2.down += weight * weight * rhs.stat2.down;
    /// Central value
    nominal += weight * rhs.nominal;
    return *this;
}

inline float Yield::stat() const
{
    return 0.5f*sqrt(stat2.up) + 0.5f*sqrt(stat2.down);
}

inline float Yield::syst() const
{
    double syst2 = 0;
    for(auto& kv : uncertainties) syst2 += std::pow(0.5*(kv.second.up+kv.second.down), 2);
    return sqrt(syst2);
}

}

#endif
