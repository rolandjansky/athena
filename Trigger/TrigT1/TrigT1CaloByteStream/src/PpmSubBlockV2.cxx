/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PpmCompressionV2.h"
#include "PpmSubBlockV2.h"

namespace LVL1BS
{

// Constant definitions

const uint32_t PpmSubBlockV2::s_wordIdVal;
const int      PpmSubBlockV2::s_errorMarker;

const int      PpmSubBlockV2::s_wordLen;
const int      PpmSubBlockV2::s_lutBit;
const int      PpmSubBlockV2::s_bcidLutBit;
const int      PpmSubBlockV2::s_fadcBit;
const int      PpmSubBlockV2::s_fadcBitV2;
const int      PpmSubBlockV2::s_bcidFadcBit;
const int      PpmSubBlockV2::s_bcidFadcBitV2;
const uint32_t PpmSubBlockV2::s_lutMask;
const uint32_t PpmSubBlockV2::s_bcidLutMask;
const uint32_t PpmSubBlockV2::s_fadcMask;
const uint32_t PpmSubBlockV2::s_bcidFadcMask;

const int      PpmSubBlockV2::s_channels;
const int      PpmSubBlockV2::s_glinkPins;
const int      PpmSubBlockV2::s_asicChannels;
const int      PpmSubBlockV2::s_dataBits;
const int      PpmSubBlockV2::s_errorBits;
const int      PpmSubBlockV2::s_bunchCrossingBits;

const uint32_t PpmSubBlockV2::s_errorMask;
const int      PpmSubBlockV2::s_glinkPinParityBit;
const int      PpmSubBlockV2::s_fpgaCorruptBit;
const int      PpmSubBlockV2::s_bunchMismatchBit;
const int      PpmSubBlockV2::s_eventMismatchBit;
const int      PpmSubBlockV2::s_asicFullBit;
const int      PpmSubBlockV2::s_timeoutBit;
const int      PpmSubBlockV2::s_mcmAbsentBit;
const int      PpmSubBlockV2::s_channelDisabledBit;

const uint16_t PpmSubBlockV2::s_run2minorVersion;

PpmSubBlockV2::PpmSubBlockV2() : m_globalError(0), m_globalDone(false),
    m_lutOffset(-1), m_fadcOffset(-1),
    m_pedestal(10), m_fadcBaseline(0),
    m_fadcThreshold(0), m_runNumber(0)
{
}

PpmSubBlockV2::~PpmSubBlockV2()
{
}

// Clear all data

void PpmSubBlockV2::clear()
{
    L1CaloSubBlock::clear();
    m_globalError   = 0;
    m_globalDone    = false;
    m_lutOffset     = -1;
    m_fadcOffset    = -1;
    m_datamap.clear();
    m_errormap.clear();
}

// Store PPM header

void PpmSubBlockV2::setPpmHeader(const int version, const int format,
                                 const int seqno, const int crate,
                                 const int module, const int slicesFadc,
                                 const int slicesLut)
{
    setHeader(s_wordIdVal, version, format, seqno, crate, module,
              slicesFadc, slicesLut);
}

// Store PPM error block header

void PpmSubBlockV2::setPpmErrorHeader(const int version, const int format,
                                      const int crate, const int module,
                                      const int slicesFadc, const int slicesLut)
{
    setHeader(s_wordIdVal, version, format, s_errorMarker, crate, module,
              slicesFadc, slicesLut);
}

// Return the number of FADC slices

int PpmSubBlockV2::slicesFadc() const
{
    int slices = slices2();
    if (slices == 0 && format() == NEUTRAL)
    {
        slices = dataWords() / (s_asicChannels * s_dataBits) - slicesLut();
    }
    if (slices <= 0) slices = 1;
    return slices;
}

// Return the number of LUT slices

int PpmSubBlockV2::slicesLut() const
{
    int slices = slices1();
    if (slices == 0) slices = 1;
    return slices;
}

// Store PPM data for later packing

void PpmSubBlockV2::fillPpmData(const int chan,
                                const std::vector<uint_least8_t> &lut,
                                const std::vector<uint_least16_t> &fadc,
                                const std::vector<int> &bcidLut,
                                const std::vector<int> &bcidFadc)
{
    const int sliceL = slicesLut();
    const int sliceF = slicesFadc();
    const int slices = sliceL + sliceF;
    const int chanPerSubBlock = channelsPerSubBlock();
    int dataSize = m_datamap.size();
    if (dataSize == 0)
    {
        dataSize = slices * chanPerSubBlock;
        m_datamap.resize(dataSize);
    }
    int offset = (chan % chanPerSubBlock) * slices;
    if (offset + slices <= dataSize)
    {
        for (int pos = 0; pos < sliceL; ++pos)
        {
            uint32_t datum = (lut[pos] & s_lutMask) << s_lutBit;
            datum |= (bcidLut[pos] & s_bcidLutMask) << s_bcidLutBit;
            m_datamap[offset + pos] = datum;
        }
        offset += sliceL;
        for (int pos = 0; pos < sliceF; ++pos)
        {
            const int adc = (fadc[pos] > 0) ? fadc[pos] : 0;
            uint32_t datum = (adc & s_fadcMask) << s_fadcBit;
            datum |= (bcidFadc[pos] & s_bcidFadcMask) << s_bcidFadcBit;
            m_datamap[offset + pos] = datum;
        }
    }
}

// Return unpacked data for given channel

void PpmSubBlockV2::ppmData(
    const int chan,
    std::vector<uint_least8_t> &lutCp,
    std::vector<uint_least8_t> &lutJep,
    std::vector<uint_least16_t> &fadc,
    std::vector<uint_least8_t> &bcidLutCp,
    std::vector<uint_least8_t> &satLutJep,
    std::vector<uint_least8_t> &bcidFadc,
    std::vector<int_least16_t> &correction,
    std::vector<uint_least8_t> &correctionEnabled

)
{
    lutCp.clear();
    lutJep.clear();
    fadc.clear();

    bcidLutCp.clear();
    satLutJep.clear();
    bcidFadc.clear();

    correction.clear();
    correctionEnabled.clear();

    if (isRun2())
    {
        ppmDataRun2(chan, lutCp, lutJep, fadc, bcidLutCp, satLutJep,
                    bcidFadc, correction, correctionEnabled);
    }
    else     // MCM
    {
        ppmDataRun1(chan, lutCp, fadc, bcidLutCp, bcidFadc);
    }
}


void PpmSubBlockV2::ppmDataRun2(
    const int chan,
    std::vector<uint_least8_t> &lutCp,
    std::vector<uint_least8_t> &lutJep,
    std::vector<uint_least16_t> &fadc,
    std::vector<uint_least8_t> &bcidLutCp,
    std::vector<uint_least8_t> &satLutJep,
    std::vector<uint_least8_t> &bcidFadc,
    std::vector<int_least16_t> &correction,
    std::vector<uint_least8_t> &correctionEnabled

)
{
    const int sliceL = slicesLut();
    const int sliceF = slicesFadc();

    int pos = (chan % channelsPerSubBlock()) * (3 * sliceL + sliceF);

    uint_least32_t word;
    for (int i = 0; i < sliceL; ++i)
    {
        word = m_datamap[pos++];
        lutCp.push_back((word >> s_lutBit) & s_lutMask);
        bcidLutCp.push_back((word >> s_bcidLutBit) & s_bcidLutMask);
    }

    for (int i = 0; i < sliceL; ++i)
    {
        word = m_datamap[pos++];
        lutJep.push_back((word >> s_lutBit) & s_lutMask);
        satLutJep.push_back((word >> s_bcidLutBit) & s_bcidLutMask);
    }

    for (int i = 0; i < sliceF; i++)
    {
        word = m_datamap[pos++];
        fadc.push_back((word >> s_fadcBitV2) & s_fadcMask);
        bcidFadc.push_back((word >> s_bcidFadcBitV2) & s_bcidFadcMask);
    }

    for (int i = 0; i < sliceL; i++)
    {
        word = m_datamap[pos++];
        correction.push_back((word >> s_fadcBitV2) & s_fadcMask);
        correctionEnabled.push_back((word >> s_bcidFadcBitV2) & s_bcidFadcMask);
    }
}

void PpmSubBlockV2::ppmDataRun1(int chan,
                                std::vector<uint_least8_t> &lut,
                                std::vector<uint_least16_t> &fadc,
                                std::vector<uint_least8_t> &bcidLut,
                                std::vector<uint_least8_t> &bcidFadc
                               )
{
    const int sliceL = slicesLut();
    const int sliceF = slicesFadc();

    int pos = (chan % channelsPerSubBlock()) * (sliceL + sliceF);

    uint32_t word;
    for (int i = 0; i < sliceL; ++i)
    {
        word = m_datamap[pos++];
        lut.push_back((word >> s_lutBit) & s_lutMask);
        bcidLut.push_back((word >> s_bcidLutBit) & s_bcidLutMask);
    }

    for (int i = 0; i < sliceF; i++)
    {
        word = m_datamap[pos++];
        fadc.push_back((word >> s_fadcBit) & s_fadcMask);
        bcidFadc.push_back((word >> s_bcidFadcBit) & s_bcidFadcMask);
    }
}

// Store an error word corresponding to a data channel

void PpmSubBlockV2::fillPpmError(const int chan, const int errorWord)
{
    if (m_errormap.empty()) m_errormap.resize(s_glinkPins);
    // Expand one ASIC channel disabled bit to four
    const uint32_t chanDisabled = (errorWord & 0x1) << asic(chan);
    m_errormap[pin(chan)] |= (((errorWord >> 1) << s_asicChannels)
                              | chanDisabled) & s_errorMask;
}

// Store an error word corresponding to a G-Link pin

void PpmSubBlockV2::fillPpmPinError(const int pin, const int errorWord)
{
    if (m_errormap.empty()) m_errormap.resize(s_glinkPins);
    m_errormap[pin] = errorWord & s_errorMask;
}

// Return the error word for a data channel

int PpmSubBlockV2::ppmError(const int chan) const
{
    int err = 0;
    if ( !m_errormap.empty())
    {
        // Replace the four ASIC channel disabled bits with just the one
        // corresponding to the data channel
        err = (((m_errormap[pin(chan)] & s_errorMask) >> s_asicChannels) << 1)
              | channelDisabled(chan);
    }
    return err;
}

// Return the error word for a G-Link pin

int PpmSubBlockV2::ppmPinError(const int pin) const
{
    int err = 0;
    if ( !m_errormap.empty()) err = m_errormap[pin] & s_errorMask;
    return err;
}

// Return global error bit

bool PpmSubBlockV2::errorBit(const int bit) const
{
    if ( ! m_globalDone)
    {
        std::vector<uint32_t>::const_iterator pos;
        for (pos = m_errormap.begin(); pos != m_errormap.end(); ++pos)
        {
            m_globalError |= *pos;
        }
        m_globalDone  = true;
    }
    return m_globalError & (0x1 << bit);
}

// Packing/Unpacking routines

bool PpmSubBlockV2::pack()
{
    bool rc = false;
    switch (format())
    {
    case NEUTRAL:
        rc = packNeutral();
        break;
    case UNCOMPRESSED:
        switch (seqno())
        {
        case s_errorMarker:
            rc = packUncompressedErrors();
            break;
        default:
            rc = packUncompressedData();
            break;
        }
        break;
    case COMPRESSED:
    case SUPERCOMPRESSED:
        rc = PpmCompressionV2::pack(*this);
        break;
    default:
        break;
    }
    return rc;
}

bool PpmSubBlockV2::unpack()
{
    bool rc = false;
    // TODO: (sasha) We can check subblock header here (ask experts)
    // Now we verify only ROD version
    switch (version())
    {
    case 1: case 2:
        switch (format())
        {
        case NEUTRAL:
            rc = unpackNeutral();
            break;
        case UNCOMPRESSED:
            switch (seqno())
            {
            case s_errorMarker:
                rc = unpackUncompressedErrors();
                break;
            default:
                rc = unpackUncompressedData();
                break;
            }
            break;
        case COMPRESSED:
        case SUPERCOMPRESSED:
            rc = PpmCompressionV2::unpack(*this);
            break;
        default:
            setUnpackErrorCode(UNPACK_FORMAT);
            break;
        }
        break;
    default:
        setUnpackErrorCode(UNPACK_VERSION);
        break;
    }
    return rc;
}

// Pack neutral data

bool PpmSubBlockV2::packNeutral()
{
    const int slices   = slicesLut() + slicesFadc();
    const int channels = channelsPerSubBlock();
    if (m_datamap.empty()) m_datamap.resize(slices * channels);
    // Bunch crossing number
    for (int pin = 0; pin < s_glinkPins; ++pin)
    {
        uint32_t bc = 0;
        if (pin < s_bunchCrossingBits) bc = (bunchCrossing() >> pin) & 0x1;
        packerNeutral(pin, bc, 1);
    }
    // Data
    std::vector<uint32_t>::const_iterator pos = m_datamap.begin();
    for (int asic = 0; asic < s_asicChannels; ++asic)
    {
        for (int pin = 0; pin < s_glinkPins; ++pin)
        {
            for (int sl = 0; sl < slices; ++sl)
            {
                packerNeutral(pin, *pos, s_dataBits);
                ++pos;
            }
        }
    }
    // Errors, including GP
    if (m_errormap.empty()) m_errormap.resize(s_glinkPins);
    pos = m_errormap.begin();
    for (int pin = 0; pin < s_glinkPins; ++pin)
    {
        packerNeutral(pin, *pos, s_errorBits);
        packerNeutralParity(pin);
        ++pos;
    }
    return true;
}

// Pack uncompressed data

bool PpmSubBlockV2::packUncompressedData()
{
    const int slices   = slicesLut() + slicesFadc();
    const int channels = channelsPerSubBlock();
    if (m_datamap.empty()) m_datamap.resize(slices * channels);
    for (int sl = 0; sl < slices; ++sl)
    {
        for (int chan = 0; chan < channels; ++chan)
        {
            packer(m_datamap[sl + chan * slices], s_wordLen);
        }
    }
    packerFlush();
    return true;
}

// Pack uncompressed error data

bool PpmSubBlockV2::packUncompressedErrors()
{
    if (m_errormap.empty()) m_errormap.resize(s_glinkPins);
    for (int pin = 0; pin < s_glinkPins; ++pin)
    {
        packer(m_errormap[pin], s_wordLen);
    }
    packerFlush();
    return true;
}

// Unpack neutral data

bool PpmSubBlockV2::unpackNeutral()
{
    const int slices = isRun2()
                       ? 3 * slicesLut() + slicesFadc()
                       : slicesLut() + slicesFadc();
    m_datamap.clear();
    // Bunch Crossing number
    int bunchCrossing = 0;
    for (int pin = 0; pin < s_glinkPins; ++pin)
    {
        const int bc = unpackerNeutral(pin, 1);
        if (pin < s_bunchCrossingBits) bunchCrossing |= bc << pin;
    }
    setBunchCrossing(bunchCrossing);
    // Data
    for (int asic = 0; asic < s_asicChannels; ++asic)
    {
        for (int pin = 0; pin < s_glinkPins; ++pin)
        {
            for (int sl = 0; sl < slices; ++sl)
            {
                m_datamap.push_back(unpackerNeutral(pin, s_dataBits));
            }
        }
    }
    const bool rc = unpackerSuccess();
    if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
    // Errors
    m_errormap.clear();
    for (int pin = 0; pin < s_glinkPins; ++pin)
    {
        uint32_t error = unpackerNeutral(pin, s_errorBits);
        error |= unpackerNeutralParityError(pin) << s_errorBits;
        m_errormap.push_back(error);
    }
    return rc;
}

// Unpack uncompressed data

bool PpmSubBlockV2::unpackUncompressedData()
{
    const int slices = isRun2()
                       ? 3 * slicesLut() + slicesFadc()
                       : slicesLut() + slicesFadc();   
    const int channels = channelsPerSubBlock();
    if (isRun2()) {
        setStreamed();
    }

    m_datamap.resize(slices * channels);
    unpackerInit();
    // for (int sl = 0; sl < slices; ++sl)
    // {
    //     for (int chan = 0; chan < channels; ++chan)
    //     {
    //         m_datamap[sl + chan * slices] = unpacker(wordLen());
    //         std::cout << "SASHA3 wordLen=" << wordLen() << " slice=" << sl << " channel=" << chan << " OK?=" << unpackerSuccess() << " value=" << std::hex << (m_datamap[sl + chan * slices]) << std::endl;
    //     }
    // }
    for (int chan = 0; chan < channels; ++chan)
    {
        for (int sl = 0; sl < slices; ++sl)
        {
            m_datamap[sl + chan * slices] = unpacker(wordLen());
            // std::cout << "SASHA3 wordLen=" << wordLen() << " slice=" << sl << " channel=" << chan << " OK?=" << unpackerSuccess() << " value=" << std::hex << (m_datamap[sl + chan * slices]) << std::endl;
        }
    }
    bool rc = unpackerSuccess();
    if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
    else
    {
        // Check no more non-zero data
        while (unpackerSuccess())
        {
            if (unpacker(wordLen()))
            {
                setUnpackErrorCode(UNPACK_EXCESS_DATA);
                rc = false;
                break;
            }
        }
    }
    return rc;
}

// Unpack uncompressed error data

bool PpmSubBlockV2::unpackUncompressedErrors()
{
    unpackerInit();
    m_errormap.clear();
    for (int pin = 0; pin < s_glinkPins; ++pin)
    {
        m_errormap.push_back(unpacker(wordLen()));
    }
    bool rc = unpackerSuccess();
    if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
    else
    {
        while (unpackerSuccess())
        {
            if (unpacker(wordLen()))
            {
                setUnpackErrorCode(UNPACK_EXCESS_DATA);
                rc = false;
                break;
            }
        }
    }
    return rc;
}

// Return the number of channels per sub-block

int PpmSubBlockV2::channelsPerSubBlock(const int version, const int format)
{
    int chan = 0;
    switch (version)
    {
    case 1:
        switch (format)
        {
        case UNCOMPRESSED:
            chan = s_channels / s_asicChannels;
            break;
        case NEUTRAL:
        case COMPRESSED:
        case SUPERCOMPRESSED:
            chan = s_channels;
            break;
        default:
            setUnpackErrorCode(UNPACK_FORMAT);
            break;
        }
        break;
    case 2:
        switch (format)
        {
        case UNCOMPRESSED:
        case NEUTRAL:
        case COMPRESSED:
        case SUPERCOMPRESSED:
            chan = s_channels;
            break;
        default:
            setUnpackErrorCode(UNPACK_FORMAT);
            break;
        }
        break;
    default:
        setUnpackErrorCode(UNPACK_VERSION);
        break;
    }
    return chan;
}

int PpmSubBlockV2::channelsPerSubBlock()
{
    return channelsPerSubBlock(version(), format());
}

int PpmSubBlockV2::wordLen(const int version, const int format)
{
    int chan = 0;
    switch (version)
    {
    case 1:
        return s_wordLen;
        break;
    case 2:
        switch (format)
        {
        case UNCOMPRESSED:
            return 11;
        case NEUTRAL:
        case COMPRESSED:
        case SUPERCOMPRESSED:
            return s_wordLen;
            break;
        default:
            setUnpackErrorCode(UNPACK_FORMAT);
            break;
        }
        break;
    default:
        setUnpackErrorCode(UNPACK_VERSION);
        break;
    }
    return chan;
}

int PpmSubBlockV2::wordLen()
{
    return wordLen(version(), format());
}

// Check if a header word is for an error block

bool PpmSubBlockV2::errorBlock(const uint32_t word)
{
    if (format(word) == UNCOMPRESSED) {
        return seqno(word) == s_errorMarker;
    }
    return false;
}

} // end namespace
