/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOL_CSCRODREADOUT_H
#define MUONCSC_CNVTOOL_CSCRODREADOUT_H

#include <stdint.h>

#include <atomic>
#include <cmath>
#include <string>
#include <vector>

#include "MuonIdHelpers/CscIdHelper.h"

// TGC ROD encoder/decoder for CscRDO
// Author Ketevi A. Assamagan
// BNL December 27 2003

class CscRODReadOut {
public:
    CscRODReadOut();
    CscRODReadOut(double startTime, double signalWidth, uint16_t numIntegration);
    ~CscRODReadOut() = default;

    // more static header/footer information
    uint32_t getHeaderSize() const { return ROD_HEADER_SIZE; }
    uint32_t getFooterSize() const { return ROD_FOOTER_SIZE; }
    uint32_t numSamples() const { return NUM_SAMPLES; }
    uint32_t latency() const { return LATENCY; }
    uint32_t samplingRate() const { return RATE; }
    uint32_t numDPU() const { return NUM_DPU; }

    // DPU header info
    uint32_t dpuHeader() const { return DPU_HEADER_MARKER; }
    uint32_t dpuHeaderSize() const { return DPU_HEADER_SIZE; }

    // get additional info
    double getStartTime() const { return m_TIME_OFFSET; }
    double getConversion() const { return m_CHARGE_TO_ADC_COUNT; }
    double getMaxTimeBin() const { return m_Z0; }

    // encoding
    uint32_t getSourceID(uint16_t side, uint16_t rodId) const;
    void encodeFragments(const std::vector<uint16_t>& amplitude, std::vector<uint32_t>& v) const;

    // initialize helper
    void set(const CscIdHelper* cscIdHelper) { m_cscIdHelper = cscIdHelper; }
    void setChamberBitVaue(uint32_t value) { m_chamberBitValue = value; }

    void setParams(double timeOffset, double signalWidth);

    // testing
    bool isDPU(const uint32_t fragment) const;
    bool discard(const uint32_t fragment) const;
    bool isAmplitude(const uint16_t fragment) const;
    bool isAddress(const uint32_t fragment) const;

    // Decoding
    void decodeSourceID(uint32_t sourceIdIDIn, uint16_t& sourceID, uint16_t& moduleType, uint16_t& subDetectorId, uint16_t& rodId) const;
    void decodeAmplitude(const uint32_t fragment, uint16_t& amp1, uint16_t& amp2) const;
    uint32_t fragmentToAddress(const uint32_t fragment) const;
    Identifier decodeAddress(const uint32_t address) const;
    uint32_t hashIdentifier(const uint32_t address, const Identifier& moduleId) const;

    uint32_t numberOfStrips(const uint32_t fragment) const;
    Identifier decodeAddress(const uint32_t address, const Identifier& moduleId) const;
    Identifier decodeAddress(const uint32_t address, const Identifier& moduleId, int j) const;
    int findCharge(double samplingTime, const std::vector<uint16_t>& amplitude, double& time) const;
    double signal_amplitude(double samplingTime) const;
    uint32_t address(const Identifier& channelId, int& eta, int& phi) const;

private:
    const CscIdHelper* m_cscIdHelper;
    uint32_t m_chamberBitValue;
    double m_norm;

    double m_TIME_OFFSET;
    double m_SIGNAL_WIDTH;
    int m_NUMBER_OF_INTEGRATION;
    double m_CHARGE_TO_ADC_COUNT;
    double m_Z0;

    static const uint32_t ROD_HEADER_SIZE = 12;

    static const uint32_t ROD_FOOTER_SIZE = 0x0;

    static const uint16_t SOURCE_ID = 0x00;
    static const uint16_t MODULE_TYPE = 0x00;

    static const uint16_t BODY_AMPLITUDE = 0x0000;
    static const uint32_t BODY_ADDRESS = 0x00000000;

    static const uint32_t NUM_SAMPLES = 25;
    static const uint32_t LATENCY = 0;
    static const uint32_t RATE = 40;  /// MHz
    static const uint32_t NUM_DPU = 12;

    static const uint32_t DPU_HEADER_MARKER = 0xC5F38856;
    static const uint32_t DPU_HEADER_SIZE = 13;

    static const uint32_t DPU_DISCARD = 0xFFFFFFFF;

    void set32bits(const uint16_t* v16, uint32_t& v32) const;
    double signal(double z) const;
};

inline void CscRODReadOut::setParams(double timeOffset, double signalWidth) {
    m_TIME_OFFSET = timeOffset;
    m_SIGNAL_WIDTH = signalWidth;
}

inline uint32_t CscRODReadOut::getSourceID(uint16_t side, uint16_t rodId) const {
    uint32_t sourceIdentifier = 0;
    sourceIdentifier = SOURCE_ID << 24 | MODULE_TYPE << 16 | side << 8 | rodId;
    return sourceIdentifier;
}

inline void CscRODReadOut::decodeSourceID(uint32_t sourceIDIn, uint16_t& sourceID, uint16_t& moduleType, uint16_t& subDetectorId,
                                          uint16_t& rodId) const {
    sourceID = (sourceIDIn & 0xff000000) >> 24;
    moduleType = (sourceIDIn & 0x00ff0000) >> 16;
    subDetectorId = (sourceIDIn & 0x0000ff00) >> 8;
    rodId = (sourceIDIn & 0x000000ff);
}

inline void CscRODReadOut::set32bits(const uint16_t* v16, uint32_t& v32) const {
    uint32_t p = 0, v = 0;
    uint16_t n = 2;
    uint16_t pos[] = {16, 0};
    for (uint16_t i = 0; i < n; i++) {
        v = (uint32_t)(*(v16 + i));
        p = (uint32_t)(*(pos + i));
        v32 = v32 | (v << p);
    }
}

inline bool CscRODReadOut::isDPU(const uint32_t fragment) const { return (fragment == DPU_HEADER_MARKER); }

inline bool CscRODReadOut::discard(const uint32_t fragment) const { return (fragment == DPU_DISCARD); }

inline bool CscRODReadOut::isAmplitude(const uint16_t fragment) const {
    uint16_t amplitudeTest = (fragment >> 12);
    return (amplitudeTest == BODY_AMPLITUDE);
}

inline bool CscRODReadOut::isAddress(const uint32_t fragment) const {
    uint32_t addressTest = (fragment >> 17);
    return (addressTest == BODY_ADDRESS);
}

inline void CscRODReadOut::decodeAmplitude(const uint32_t fragment, uint16_t& amp1, uint16_t& amp2) const {
    amp2 = 0x0000FFFF & fragment;
    amp1 = (0xFFFF0000 & fragment) >> 16;
}

inline uint32_t CscRODReadOut::fragmentToAddress(const uint32_t fragment) const { return 0x0001FFFF & fragment; }

inline Identifier CscRODReadOut::decodeAddress(const uint32_t address) const {
    int stationName = ((address & 0x00010000) >> 16) + 50;
    int stationEta = (((address & 0x00001000) >> 12) == 0x0) ? -1 : 1;
    int stationPhi = ((address & 0x0000E000) >> 13) + 1;

    return m_cscIdHelper->elementID(stationName, stationEta, stationPhi);
}

// module Id is given and strip is from address.
// This should be flipped.
inline Identifier CscRODReadOut::decodeAddress(const uint32_t address, const Identifier& moduleId) const {
    int chamberLayer = ((address & 0x00000800) >> 11) + m_chamberBitValue;

    int wireLayer = ((address & 0x00000600) >> 9) + 1;
    int measuresPhi = ((address & 0x00000100) >> 8);

    int strip = (address & 0x000000FF) + 1;

    int stationEta = (((address & 0x00001000) >> 12) == 0x0) ? -1 : 1;

    // Added to Online -> Offline id  in A side number is opposite bug#56002
    if (measuresPhi) {
        if (stationEta > 0) strip = 49 - strip;
    }

    return m_cscIdHelper->channelID(moduleId, chamberLayer, wireLayer, measuresPhi, strip);
}

// module Id is given and strip is from address.
// This should be flipped.
inline Identifier CscRODReadOut::decodeAddress(const uint32_t address, const Identifier& moduleId, int j) const {
    int chamberLayer = ((address & 0x00000800) >> 11) + m_chamberBitValue;

    int wireLayer = ((address & 0x00000600) >> 9) + 1;
    int measuresPhi = ((address & 0x00000100) >> 8);
    int strip = (address & 0x000000FF) + 1 + j;

    // Added to Online -> Offline id  in A side number is opposite bug#56002
    if (measuresPhi) {
        int stationEta = (((address & 0x00001000) >> 12) == 0x0) ? -1 : 1;
        if (stationEta > 0) strip = 49 - strip;
    }

    return m_cscIdHelper->channelID(moduleId, chamberLayer, wireLayer, measuresPhi, strip);
}

// module Id is given and strip is from address.
// This should be flipped.
inline uint32_t CscRODReadOut::hashIdentifier(const uint32_t address, const Identifier& moduleId) const {
    int chamberLayer = ((address & 0x00000800) >> 11) + m_chamberBitValue;

    int wireLayer = ((address & 0x00000600) >> 9) + 1;
    int measuresPhi = ((address & 0x00000100) >> 8);
    int strip = (address & 0x000000FF) + 1;

    // Added to Online -> Offline id  in A side number is opposite bug#56002
    if (measuresPhi) {
        int stationEta = (((address & 0x00001000) >> 12) == 0x0) ? -1 : 1;
        if (stationEta > 0) strip = 49 - strip;
    }

    Identifier id = m_cscIdHelper->channelID(moduleId, chamberLayer, wireLayer, measuresPhi, strip);
    IdContext context = m_cscIdHelper->channel_context();
    IdentifierHash hash;
    m_cscIdHelper->get_hash(id, hash, &context);
    return (uint32_t)hash;
}

inline uint32_t CscRODReadOut::numberOfStrips(const uint32_t fragment) const {
    uint32_t address = fragmentToAddress(fragment);
    Identifier moduleId = decodeAddress(address);
    Identifier channelId = decodeAddress(address, moduleId);
    return uint32_t(m_cscIdHelper->stripMax(channelId));
}

// get the signal amplitude for a given sampling time (ns)
inline double CscRODReadOut::signal_amplitude(double samplingTime) const {
    if (samplingTime <= m_TIME_OFFSET) return 0.0;
    double z = (samplingTime - m_TIME_OFFSET) / m_SIGNAL_WIDTH;
    return signal(z) / m_norm;
}

// signal amplitude as a function of the time bin z
inline double CscRODReadOut::signal(double z) const {
    double amplitude = (1.0 - z / (1 + m_NUMBER_OF_INTEGRATION)) * std::pow(z, m_NUMBER_OF_INTEGRATION) * exp(-z);
    return amplitude;
}

// find the address of this strip
// This function is called by CscDigitToCscRdoTool
// so return address should be consistent to online
inline uint32_t CscRODReadOut::address(const Identifier& channelId, int& eta, int& phi) const {
    // unpack the strip identifier
    int name = m_cscIdHelper->stationName(channelId);
    eta = m_cscIdHelper->stationEta(channelId);
    phi = m_cscIdHelper->stationPhi(channelId);
    int chamberLayer = m_cscIdHelper->chamberLayer(channelId);
    int wireLayer = m_cscIdHelper->wireLayer(channelId);
    int orientation = m_cscIdHelper->measuresPhi(channelId);
    int strip = m_cscIdHelper->strip(channelId);

    // NOPE. Don't flip it!! This strip is for CscDigitToCscRdoTool online address

    // redefine the ranges
    uint32_t nameIndex = uint32_t(name - 50);
    uint32_t etaIndex = (eta == -1) ? 0 : 1;
    uint32_t phiIndex = uint32_t(phi - 1);
    uint32_t chamberIndex = uint32_t(chamberLayer - 0);
    uint32_t layerIndex = uint32_t(wireLayer - 1);
    uint32_t stripType = uint32_t(orientation);
    uint32_t stripNumber = uint32_t(strip - 1);

    if (m_chamberBitValue == 1) chamberIndex = uint32_t(chamberLayer - 1);

    // build the address
    uint32_t address =
        nameIndex << 16 | phiIndex << 13 | etaIndex << 12 | chamberIndex << 11 | layerIndex << 9 | stripType << 8 | stripNumber;

    return address;
}

#endif  // MUONCSC_CNVTOOL_CSCRODREADOUT_H
