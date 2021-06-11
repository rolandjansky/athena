/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOL_CSCRODREADOUTV0_H
#define MUONCSC_CNVTOOL_CSCRODREADOUTV0_H

#include <stdint.h>

#include <cmath>
#include <vector>

#include "MuonIdHelpers/CscIdHelper.h"

// CSC ROD encoder/decoder for CscRDO
// Author Ketevi A. Assamagan
// BNL December 27 2003

class CscRODReadOutV0 {
public:
    CscRODReadOutV0();
    ~CscRODReadOutV0();

    // get static head/footer information
    uint32_t getHeaderMarker() { return ROD_HEADER; }
    uint32_t getHeaderSize() { return ROD_HEADER_SIZE; }
    uint32_t getFooterSize() { return ROD_FOOTER_SIZE; }
    uint32_t getFormatVersion() { return ROD_VERSION; }
    double getSamplingTime() { return m_SAMPLING_TIME; }
    double getStartTime() { return m_TIME_OFFSET; }
    double getConversion() { return m_CHARGE_TO_ADC_COUNT; }
    double getMaxTimeBin() { return m_Z0; }

    // encoding
    uint32_t getSourceID(uint16_t side, uint16_t rodId);
    void encodeFragments(const uint16_t* amplitude, const uint32_t& address, std::vector<uint32_t>& v) const;

    // initialize helper
    void set(const CscIdHelper* cscHelper) { m_cscHelper = cscHelper; }

    // testing
    bool isBody(const uint32_t fragment) const;
    bool isAmplitude(const uint16_t fragment) const;
    bool isAddress(const uint32_t fragment) const;

    // Decoding
    void decodeSourceID(uint32_t sourceId);
    void decodeAmplitude(const uint32_t fragment, int index);
    void decodeAddress(const uint32_t fragment);
    Identifier decodeAddress();
    Identifier decodeAddress(const Identifier& moduleId);
    double findCharge();
    double signal_amplitude(double samplingTime) const;
    uint32_t address(const Identifier& channelId, int& eta, int& phi) const;

    // Retrieve decoded results
    uint16_t sourceID() const { return m_sourceID; }
    uint16_t moduleType() const { return m_moduleType; }
    uint16_t subDetectorId() const { return m_subDetectorId; }
    uint16_t rodId() const { return m_rodId; }
    uint16_t getAmp1() const { return m_amp1; }
    uint16_t getAmp2() const { return m_amp2; }
    uint16_t getAmp3() const { return m_amp3; }
    uint16_t getAmp4() const { return m_amp4; }
    uint32_t address() const { return m_address; }

private:
    const CscIdHelper* m_cscHelper;
    uint16_t m_sourceID;
    uint16_t m_moduleType;
    uint16_t m_rodId;
    uint16_t m_subDetectorId;
    uint16_t m_amp1;
    uint16_t m_amp2;
    uint16_t m_amp3;
    uint16_t m_amp4;
    uint32_t m_address;
    double m_norm;

    double m_TIME_OFFSET;
    double m_SIGNAL_WIDTH;
    double m_SAMPLING_TIME;
    int m_NUMBER_OF_INTEGRATION;
    double m_CHARGE_TO_ADC_COUNT;
    double m_Z0;

    static const uint32_t ROD_HEADER = 0xEE1234EE;
    static const uint32_t ROD_HEADER_SIZE = 0x8;
    static const uint32_t ROD_VERSION = 0x00000000;
    static const uint32_t ROD_FOOTER_SIZE = 0x3;

    static const uint16_t SOURCE_ID = 0x00;
    static const uint16_t MODULE_TYPE = 0x00;

    static const uint16_t BODY_AMPLITUDE = 0x000A;
    static const uint32_t BODY_ADDRESS = 0x0000000C;

    void set32bits(const uint16_t* v16, uint32_t& v32) const;
    double signal(double z) const;
};

inline uint32_t CscRODReadOutV0::getSourceID(uint16_t side, uint16_t rodId) {
    uint32_t sourceIdentifier = 0;
    sourceIdentifier = SOURCE_ID << 24 | MODULE_TYPE << 16 | side << 8 | rodId;
    return sourceIdentifier;
}

inline void CscRODReadOutV0::decodeSourceID(uint32_t sourceID) {
    m_sourceID = (sourceID & 0xff000000) >> 24;
    m_moduleType = (sourceID & 0x00ff0000) >> 16;
    m_subDetectorId = (sourceID & 0x0000ff00) >> 8;
    m_rodId = (sourceID & 0x000000ff);
}

inline void CscRODReadOutV0::set32bits(const uint16_t* v16, uint32_t& v32) const {
    uint32_t p = 0, v = 0;
    uint16_t n = 2;
    uint16_t pos[] = {0, 16};
    for (uint16_t i = 0; i < n; i++) {
        v = (uint32_t)(*(v16 + i));
        p = (uint32_t)(*(pos + i));
        v32 = v32 | (v << p);
    }
}

inline bool CscRODReadOutV0::isBody(const uint32_t fragment) const { return (0x80000000 & fragment); }

inline bool CscRODReadOutV0::isAmplitude(const uint16_t fragment) const { return (0xA000 & fragment); }

inline bool CscRODReadOutV0::isAddress(const uint32_t fragment) const { return (0xC0000000 & fragment); }

inline void CscRODReadOutV0::decodeAmplitude(const uint32_t fragment, int index) {
    uint32_t amp1 = 0;
    uint32_t amp2 = 0;
    amp1 = 0x0000FFFF & fragment;
    amp2 = (0xFFFF0000 & fragment) >> 16;
    if (index == 1) {
        m_amp1 = 0x0FFF & amp1;
        m_amp2 = 0x0FFF & amp2;
    } else {
        m_amp3 = 0x0FFF & amp1;
        m_amp4 = 0x0FFF & amp2;
    }
}

inline void CscRODReadOutV0::decodeAddress(const uint32_t fragment) { m_address = 0x0003FFFF & fragment; }

inline Identifier CscRODReadOutV0::decodeAddress() {
    int stationName = ((m_address & 0x00020000) >> 17) + 50;
    int stationEta = (((m_address & 0x00010000) >> 16) == 0x0) ? -1 : 1;
    int stationPhi = ((m_address & 0x0000E000) >> 13) + 1;

    return m_cscHelper->elementID(stationName, stationEta, stationPhi);
}

inline Identifier CscRODReadOutV0::decodeAddress(const Identifier& moduleId) {
    int chamberLayer = ((m_address & 0x00001000) >> 12) + 1;
    int wireLayer = ((m_address & 0x00000E00) >> 9) + 1;
    int measuresPhi = ((m_address & 0x00000100) >> 8);
    int strip = (m_address & 0x000000FF);
    return m_cscHelper->channelID(moduleId, chamberLayer, wireLayer, measuresPhi, strip);
}

// get the signal amplitude for a given sampling time (ns)
inline double CscRODReadOutV0::signal_amplitude(double samplingTime) const {
    if (samplingTime <= m_TIME_OFFSET) return 0.0;
    double z = (samplingTime - m_TIME_OFFSET) / m_SIGNAL_WIDTH;
    return signal(z) / m_norm;
}

// signal amplitude as a function of the time bin z
inline double CscRODReadOutV0::signal(double z) const {
    double amplitude = (1.0 - z / (1 + m_NUMBER_OF_INTEGRATION)) * std::pow(z, m_NUMBER_OF_INTEGRATION) * exp(-z);
    return amplitude;
}

// find the address of this strip
inline uint32_t CscRODReadOutV0::address(const Identifier& channelId, int& eta, int& phi) const {
    // unpack the strip identifier
    int name = m_cscHelper->stationName(channelId);
    eta = m_cscHelper->stationEta(channelId);
    phi = m_cscHelper->stationPhi(channelId);
    int chamberLayer = m_cscHelper->chamberLayer(channelId);
    int wireLayer = m_cscHelper->wireLayer(channelId);
    int orientation = m_cscHelper->measuresPhi(channelId);
    int strip = m_cscHelper->strip(channelId);

    // redefine the ranges
    uint32_t nameIndex = uint32_t(name - 50);
    uint32_t etaIndex = (eta == -1) ? 0 : 1;
    uint32_t phiIndex = uint32_t(phi - 1);
    uint32_t chamberIndex = uint32_t(chamberLayer - 1);
    uint32_t layerIndex = uint32_t(wireLayer - 1);
    uint32_t stripType = uint32_t(orientation);
    uint32_t stripNumber = uint32_t(strip);

    // build the address
    uint32_t address =
        nameIndex << 17 | etaIndex << 16 | phiIndex << 13 | chamberIndex << 12 | layerIndex << 9 | stripType << 8 | stripNumber;

    return address;
}

#endif  // MUONCSC_CNVTOOL_CSCRODREADOUTV0_H
