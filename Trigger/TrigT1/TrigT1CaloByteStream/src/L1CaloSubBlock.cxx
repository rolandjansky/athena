/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "L1CaloSubBlock.h"

namespace LVL1BS
{

// Static constant definitions

const int      L1CaloSubBlock::s_headerBit;
const int      L1CaloSubBlock::s_statusBit;
const uint32_t L1CaloSubBlock::s_headerMask;
const uint32_t L1CaloSubBlock::s_statusMask;
const uint32_t L1CaloSubBlock::s_headerVal;
const uint32_t L1CaloSubBlock::s_statusVal;

const int      L1CaloSubBlock::s_ppmCrates;

const int      L1CaloSubBlock::s_wordIdBit;
const int      L1CaloSubBlock::s_versionBit;
const int      L1CaloSubBlock::s_formatBit;
const int      L1CaloSubBlock::s_seqnoBit;
const int      L1CaloSubBlock::s_crateBit;
const int      L1CaloSubBlock::s_moduleBit;
const int      L1CaloSubBlock::s_slices2Bit;
const int      L1CaloSubBlock::s_slices1Bit;
const uint32_t L1CaloSubBlock::s_wordIdMask;
const uint32_t L1CaloSubBlock::s_versionMask;
const uint32_t L1CaloSubBlock::s_formatMask;
const uint32_t L1CaloSubBlock::s_seqnoMask;
const uint32_t L1CaloSubBlock::s_crateMask;
const uint32_t L1CaloSubBlock::s_moduleMask;
const uint32_t L1CaloSubBlock::s_slices2Mask;
const uint32_t L1CaloSubBlock::s_slices1Mask;

const int      L1CaloSubBlock::s_failingBcnBit;
const int      L1CaloSubBlock::s_glinkTimeoutBit;
const int      L1CaloSubBlock::s_glinkDownBit;
const int      L1CaloSubBlock::s_upstreamErrorBit;
const int      L1CaloSubBlock::s_daqOverflowBit;
const int      L1CaloSubBlock::s_bcnMismatchBit;
const int      L1CaloSubBlock::s_glinkProtocolBit;
const int      L1CaloSubBlock::s_glinkParityBit;
const uint32_t L1CaloSubBlock::s_failingBcnMask;

const int      L1CaloSubBlock::s_maxWordBits;
const int      L1CaloSubBlock::s_maxStreamedBits;
const uint32_t L1CaloSubBlock::s_maxWordMask;
const uint32_t L1CaloSubBlock::s_maxStreamedMask;

const int      L1CaloSubBlock::s_maxPins;
const uint32_t L1CaloSubBlock::s_glinkDavSet;

L1CaloSubBlock::L1CaloSubBlock() : m_header(0), m_trailer(0),
    m_bunchCrossing(0),
    m_unpackError(UNPACK_NONE),
    m_bitword(0), m_currentBit(0),
    m_maxBits(s_maxWordBits),
    m_maxMask(s_maxWordMask),
    m_unpackerFlag(false),
    m_currentPinBit(s_maxPins),
    m_oddParity(s_maxPins, 1),
    m_dataWords(0)
{
    // Initialize unpacking masks
    m_unpackingMasks.assign(s_maxWordBits + 1, 0);
    for (int i = 1; i <= s_maxWordBits; ++i)
    {
        m_unpackingMasks[i] = (m_unpackingMasks[i - 1] << 1) | 0x1;
    }
}

L1CaloSubBlock::~L1CaloSubBlock()
{
}

// Clear all data

void L1CaloSubBlock::clear()
{
    m_header = 0;
    m_trailer = 0;
    m_bunchCrossing = 0;
    m_unpackError = UNPACK_NONE;
    m_bitword = 0;
    m_currentBit = 0;
    m_unpackerFlag = false;
    m_currentPinBit.assign(s_maxPins, 0);
    m_oddParity.assign(s_maxPins, 1);
    m_dataWords = 0;
    m_data.clear();
}

// Store header data

void L1CaloSubBlock::setHeader(const int wordId, const int version,
                               const int format, const int seqno,
                               const int crate, const int module,
                               const int slices2, const int slices1)
{
    uint32_t word = 0;
    word |= (wordId  & s_wordIdMask)  << s_wordIdBit;
    word |= (version & s_versionMask) << s_versionBit;
    word |= (format  & s_formatMask)  << s_formatBit;
    word |= (seqno   & s_seqnoMask)   << s_seqnoBit;
    word |= (crate   & s_crateMask)   << s_crateBit;
    word |= (module  & s_moduleMask)  << s_moduleBit;
    word |= (slices2 & s_slices2Mask) << s_slices2Bit;
    word |= (slices1 & s_slices1Mask) << s_slices1Bit;
    m_header = word;
}

// Input complete packed sub-block from ROD vector

OFFLINE_FRAGMENTS_NAMESPACE::PointerType L1CaloSubBlock::read(
    const OFFLINE_FRAGMENTS_NAMESPACE::PointerType beg,
    const OFFLINE_FRAGMENTS_NAMESPACE::PointerType end)
{
    m_dataWords = 0;
    m_unpackerFlag = true;
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType pos(beg);
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType pose(end);
    for (; pos != pose; ++pos)
    {
        const uint32_t word = *pos;
        const SubBlockWordType type = wordType(word);
        if (type == HEADER)
        {
            if (m_header) return pos;
            m_header = word;
        }
        else if (type == STATUS)
        {
            // Word ID should be consistent with header
            if (m_trailer || (wordId(word) != wordId() + 1)) return pos;
            m_trailer = word;
        }
        else
        {
            // Check data word IDs
            const int id = wordId(word);
            bool badId = false;
            // All neutral format '0000'
            if (format() == NEUTRAL)        badId = (id != 0);
            // Other PPM '0xxx'
            else if (crate() < s_ppmCrates) badId = ((id & 0x8) != 0);
            // Other CPM/JEM '01xx' or '10xx'
            else if (wordId() == 0xc)       badId = (((id & 0xc) != 0x4) &&
                                                        ((id & 0xc) != 0x8));
            // Other CMM/CMX '00xx'
            else                            badId = ((id & 0xc) != 0);
            if (m_trailer || badId) return pos;
            m_data.push_back(word);
            ++m_dataWords;
        }
    }
    return pose;
}

// Output complete packed sub-block to ROD vector

void L1CaloSubBlock::write(
    FullEventAssembler<L1CaloSrcIdMap>::RODDATA *const theROD) const
{
    theROD->push_back(m_header);
    std::vector<uint32_t>::const_iterator pos;
    for (pos = m_data.begin(); pos != m_data.end(); ++pos)
    {
        theROD->push_back(*pos);
    }
    if (m_trailer) theROD->push_back(m_trailer);
}

// Store error status trailer

void L1CaloSubBlock::setStatus(const uint32_t failingBCN,
                               const bool glinkTimeout, const bool glinkDown, const bool upstreamError,
                               const bool daqOverflow, const bool bcnMismatch,
                               const bool glinkProtocol, const bool glinkParity)
{
    uint32_t word = 0;
    word |= (failingBCN & s_failingBcnMask) << s_failingBcnBit;
    word |= glinkTimeout  << s_glinkTimeoutBit;
    word |= glinkDown     << s_glinkDownBit;
    word |= upstreamError << s_upstreamErrorBit;
    word |= daqOverflow   << s_daqOverflowBit;
    word |= bcnMismatch   << s_bcnMismatchBit;
    word |= glinkProtocol << s_glinkProtocolBit;
    word |= glinkParity   << s_glinkParityBit;
    if (word)
    {
        word |= (wordId()    & s_wordIdMask) << s_wordIdBit;
        word |= (s_statusVal & s_statusMask) << s_statusBit;
        word |= (seqno()     & s_seqnoMask)  << s_seqnoBit;
        word |= (crate()     & s_crateMask)  << s_crateBit;
        word |= (module()    & s_moduleMask) << s_moduleBit;
    }
    m_trailer = word;
}

// Set DAQ FIFO Overflow bit in Sub-status word

void L1CaloSubBlock::setDaqOverflow(const int bit)
{
    if (bit)
    {
        if (m_trailer) m_trailer |= (1 << s_daqOverflowBit);
        else setStatus(0, false, false, false, true, false, false, false);
    }
}

// Set G-Link Parity bit in Sub-status word

void L1CaloSubBlock::setGlinkParity(const int bit)
{
    if (bit)
    {
        if (m_trailer) m_trailer |= (1 << s_glinkParityBit);
        else setStatus(0, false, false, false, false, false, false, true);
    }
}

// Return the unpacking error message for printing

std::string L1CaloSubBlock::unpackErrorMsg() const
{
    std::string msg;
    switch (m_unpackError)
    {
    case UNPACK_NONE:
        msg = "No error";
        break;
    case UNPACK_VERSION:
        msg = "Unsupported Data Version";
        break;
    case UNPACK_FORMAT:
        msg = "Unsupported Data Format";
        break;
    case UNPACK_COMPRESSION_VERSION:
        msg = "Unsupported Compression Version";
        break;
    case UNPACK_COMPRESSION_SLICES:
        msg = "Unsupported Number of Slices for Compression Version";
        break;
    case UNPACK_DATA_TRUNCATED:
        msg = "Premature End of Sub-block Data";
        break;
    case UNPACK_EXCESS_DATA:
        msg = "Excess Data in Sub-block";
        break;
    case UNPACK_SOURCE_ID:
        msg = "Invalid Source ID in Sub-block Data";
        break;
    case UNPACK_EXCESS_TOBS:
        msg = "Excess TOBs in Sub-block Data";
        break;
    case UNPACK_DATA_ID:
        msg = "Invalid word ID in Sub-block Data";
        break;
    default:
        msg = "Unknown Error Code";
        break;
    }
    return msg;
}

// Packing utilities

// Return the minimum number of bits needed for given data

int L1CaloSubBlock::minBits(const uint32_t datum) const
{
    const int maxBits = 32;
    int nbits = maxBits;
    for (int i = 0; i < maxBits; ++i)
    {
        if ( !(datum >> i))
        {
            nbits = i;
            break;
        }
    }
    return nbits;
}

// Return the parity bit for given data

int L1CaloSubBlock::parityBit(const int init, const uint32_t datum,
                              const int nbits) const
{
    // set init to 0/1 for even/odd parity
    int parity = init;
    for (int bit = 0; bit < nbits; ++bit) parity ^= (datum >> bit) & 0x1;
    return parity;
}

// Pack given data into given number of bits

void L1CaloSubBlock::packer(const uint32_t datum, const int nbits)
{
    if (nbits > 0)
    {
        uint32_t mask = m_unpackingMasks[nbits];
        m_bitword |= (datum & mask) << m_currentBit;
        m_currentBit += nbits;
        if (m_currentBit >= m_maxBits)
        {
            m_bitword &= m_maxMask;
            m_data.push_back(m_bitword);
            ++m_dataWords;
            const int bitsLeft = m_currentBit - m_maxBits;
            if (bitsLeft > 0)
            {
                m_bitword = (datum & mask) >> (nbits - bitsLeft);
                m_currentBit = bitsLeft;
            }
            else
            {
                m_bitword = 0;
                m_currentBit = 0;
            }
        }
    }
}

// Flush the current data word padded with zeros

void L1CaloSubBlock::packerFlush()
{
    if (m_currentBit > 0)
    {
        m_bitword &= m_maxMask;
        m_data.push_back(m_bitword);
        ++m_dataWords;
        m_bitword = 0;
        m_currentBit = 0;
    }
}

// Unpack given number of bits of data

uint32_t L1CaloSubBlock::unpacker(const int nbits)
{
    uint32_t word = 0;
    if (nbits > 0)
    {
        if (m_dataPos == m_dataPosEnd)
        {
            m_unpackerFlag = false;
            return 0;
        }
        int nbitsDone = nbits;
        if (nbitsDone > m_maxBits - m_currentBit)
        {
            nbitsDone = m_maxBits - m_currentBit;
        }
        word = (m_bitword >> m_currentBit) & m_unpackingMasks[nbitsDone];
        m_currentBit += nbits;
        
        if (m_currentBit >= m_maxBits)
        {
            m_bitword = 0;
            if (m_dataPos != m_dataPosEnd)
            {
                ++m_dataPos;
                if (m_dataPos != m_dataPosEnd)
                {
                    m_bitword = *m_dataPos;
                }
            }
            m_currentBit = 0;
            const int bitsLeft = nbits - nbitsDone;
            if (bitsLeft > 0)
            {
                if (m_dataPos == m_dataPosEnd)
                {
                    m_unpackerFlag = false;
                    return word;
                }
                word |= (m_bitword & m_unpackingMasks[bitsLeft]) << nbitsDone;
                m_currentBit = bitsLeft;
            }
        }
    }  
    return word;
}

// Initialise unpacker

void L1CaloSubBlock::unpackerInit()
{
    m_bitword = 0;
    m_currentBit = 0;
    m_unpackerFlag = true;
    m_dataPos = m_data.begin();
    m_dataPosEnd = m_data.end();
    // std::cout << "SASHA4 m_dataPosEnd - m_dataPos=" << (m_dataPosEnd - m_dataPos) << std::endl;
    // uint16_t a = (*m_dataPos) & 0xFF;
    // uint16_t b = (*m_dataPos >> 11) & 0xFF;
    
    // std::cout << "SASHA5 first " << a << std::endl;
    // std::cout << "SASHA5 second " << b << std::endl;
    // std::cout << "SASHA5 datawords " << m_data.size() << std::endl;

    if (m_dataPos != m_dataPosEnd) m_bitword = *m_dataPos;
}

// Pack given neutral data from given pin

void L1CaloSubBlock::packerNeutral(const int pin, const uint32_t datum,
                                   const int nbits)
{
    if (pin >= 0 && pin < s_maxPins && nbits > 0)
    {
        if (m_currentPinBit[pin] + nbits > m_dataWords)
        {
            m_dataWords = m_currentPinBit[pin] + nbits;
            m_data.resize(m_dataWords, s_glinkDavSet);
        }
        for (int bit = 0; bit < nbits; ++bit)
        {
            m_data[m_currentPinBit[pin] + bit] |= ((datum >> bit) & 0x1) << pin;
        }
        m_currentPinBit[pin] += nbits;
        m_oddParity[pin] = parityBit(m_oddParity[pin], datum, nbits);
    }
}

// Pack current G-Link parity bit for given pin

void L1CaloSubBlock::packerNeutralParity(const int pin)
{
    if (pin >= 0 && pin < s_maxPins)
    {
        packerNeutral(pin, m_oddParity[pin], 1);
        m_oddParity[pin] = 1;
    }
}

// Unpack given number of bits of neutral data for given pin

uint32_t L1CaloSubBlock::unpackerNeutral(const int pin, const int nbits)
{
    uint32_t word = 0;
    if (pin >= 0 && pin < s_maxPins && nbits > 0
            && m_currentPinBit[pin] + nbits <= m_dataWords)
    {
        for (int bit = 0; bit < nbits; ++bit)
        {
            word |= ((m_data[m_currentPinBit[pin] + bit] >> pin) & 0x1) << bit;
        }
        m_currentPinBit[pin] += nbits;
        m_oddParity[pin] = parityBit(m_oddParity[pin], word, nbits);
    }
    else m_unpackerFlag = false;
    return word;
}

// Unpack and test G-Link parity bit for given pin

bool L1CaloSubBlock::unpackerNeutralParityError(const int pin)
{
    bool error = true;
    if (pin >= 0 && pin < s_maxPins)
    {
        int parity = m_oddParity[pin];
        int bit    = unpackerNeutral(pin, 1);
        m_oddParity[pin] = 1;
        error = !(bit == parity);
    }
    return error;
}

// Static function to determine word type

L1CaloSubBlock::SubBlockWordType L1CaloSubBlock::wordType(const uint32_t word)
{
    SubBlockWordType type = DATA;
    if (((word >> s_headerBit) & s_headerMask) == s_headerVal)
    {
        if (((word >> s_statusBit) & s_statusMask) == s_statusVal) type = STATUS;
        else type = HEADER;
    }
    return type;
}

// Return wordID field from given header word

int L1CaloSubBlock::wordId(const uint32_t word)
{
    return (word >> s_wordIdBit) & s_wordIdMask;
}

// Return version number from given header word

int L1CaloSubBlock::version(const uint32_t word)
{
    return (word >> s_versionBit) & s_versionMask;
}

// Return data format from given header word

int L1CaloSubBlock::format(const uint32_t word)
{
    return (word >> s_formatBit) & s_formatMask;
}

// Return seqno field from given header word

int L1CaloSubBlock::seqno(const uint32_t word)
{
    return (word >> s_seqnoBit) & s_seqnoMask;
}

// Return module field from given header word

int L1CaloSubBlock::module(const uint32_t word)
{
    return (word >> s_moduleBit) & s_moduleMask;
}

} // end namespace
