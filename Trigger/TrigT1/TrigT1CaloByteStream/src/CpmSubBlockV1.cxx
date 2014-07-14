/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CpmSubBlockV1.h"

namespace LVL1BS {

// Constant definitions

const int      CpmSubBlockV1::s_wordIdVal;

const int      CpmSubBlockV1::s_wordLength;

const int      CpmSubBlockV1::s_ttDataABit;
const int      CpmSubBlockV1::s_ttDataBBit;
const int      CpmSubBlockV1::s_parityABit;
const int      CpmSubBlockV1::s_parityBBit;
const int      CpmSubBlockV1::s_linkDownABit;
const int      CpmSubBlockV1::s_linkDownBBit;
const int      CpmSubBlockV1::s_pairBit;
const int      CpmSubBlockV1::s_fpgaBit;
const int      CpmSubBlockV1::s_dataIdBit;
const int      CpmSubBlockV1::s_ttWordId;
const uint32_t CpmSubBlockV1::s_ttDataMask;
const uint32_t CpmSubBlockV1::s_pairPinMask;
const uint32_t CpmSubBlockV1::s_dataIdMask;

const int      CpmSubBlockV1::s_indicatorBit;
const int      CpmSubBlockV1::s_threshWordId;
const uint32_t CpmSubBlockV1::s_threshMask;

const int      CpmSubBlockV1::s_pairsPerPin;
const int      CpmSubBlockV1::s_wordsPerPin;
const int      CpmSubBlockV1::s_ttBits;
const int      CpmSubBlockV1::s_errBits;
const int      CpmSubBlockV1::s_hitBits;
const int      CpmSubBlockV1::s_hitWords;
const int      CpmSubBlockV1::s_glinkPins;
const int      CpmSubBlockV1::s_glinkBitsPerSlice;


CpmSubBlockV1::CpmSubBlockV1() : m_channels(80)
{
  m_chanPresent.assign(m_channels, 0);
}

CpmSubBlockV1::~CpmSubBlockV1()
{
}

// Clear all data

void CpmSubBlockV1::clear()
{
  L1CaloSubBlock::clear();
  m_ttData.clear();
  m_hitData.clear();
  m_chanPresent.assign(m_channels, 0);
}

// Store CPM header

void CpmSubBlockV1::setCpmHeader(const int version, const int format,
                                 const int slice, const int crate,
                                 const int module, const int timeslices)
{
  setHeader(s_wordIdVal, version, format, slice, crate, module, 0, timeslices);
}

// Store trigger tower data

void CpmSubBlockV1::fillTowerData(const int slice, const int channel,
                                  const int em, const int had,
                                  const int emErr, const int hadErr)
{
  if (channel < m_channels && (em || emErr || had || hadErr)) {
    resize(m_ttData, m_channels);
    int dat = em;
    int err = emErr;
    for (int pinOffset = 0; pinOffset < 2; ++pinOffset) {
      if (dat || err) {
        const int pin  = 2 * (channel/s_wordsPerPin) + pinOffset;
        const int pair = (channel % s_wordsPerPin) / 2;
        const int pos  = pin * s_pairsPerPin + pair;
        const int ix   = index(slice) * m_channels + pos;
        uint32_t word = m_ttData[ix];
        if (channel % 2 == 0) {
          word |= (dat & s_ttDataMask) << s_ttDataABit;
	  word |= (err & 0x1)          << s_parityABit;
	  word |= ((err >> 1) & 0x1)   << s_linkDownABit;
        } else {
          word |= (dat & s_ttDataMask) << s_ttDataBBit;
	  word |= (err & 0x1)          << s_parityBBit;
	  word |= ((err >> 1) & 0x1)   << s_linkDownBBit;
        }
        word |= pair       << s_pairBit;
        word |= pin        << s_fpgaBit;
        word |= s_ttWordId << s_dataIdBit;
        m_ttData[ix] = word;
      }
      dat = had;
      err = hadErr;
    }
    m_chanPresent[channel] = 1;
  }
}

// Store hit counts

void CpmSubBlockV1::setHits(const int slice, const unsigned int hit0,
                                             const unsigned int hit1)
{
  if (hit0 || hit1) {
    resize(m_hitData, 2);
    const int ix = index(slice)*2;
    unsigned int hits = hit0;
    for (int indicator = 0; indicator < 2; ++indicator) {
      if (hits) {
        uint32_t word = (hits & s_threshMask);
        word |= indicator      << s_indicatorBit;
        word |= s_threshWordId << s_dataIdBit;
        m_hitData[ix + indicator] = word;
      }
      hits = hit1;
    }
  }
}

// Return Em data for given channel

int CpmSubBlockV1::emData(const int slice, const int channel) const
{
  return data(slice, channel, 0);
}

// Return Had data for given channel

int CpmSubBlockV1::hadData(const int slice, const int channel) const
{
  return data(slice, channel, 1);
}

// Return Em error for given channel

int CpmSubBlockV1::emError(const int slice, const int channel) const
{
  return error(slice, channel, 0);
}

// Return Had error for given channel

int CpmSubBlockV1::hadError(const int slice, const int channel) const
{
  return error(slice, channel, 1);
}

// Return first hit counts word

unsigned int CpmSubBlockV1::hits0(const int slice) const
{
  return hits(slice, 0);
}

// Return second hit counts word

unsigned int CpmSubBlockV1::hits1(const int slice) const
{
  return hits(slice, 1);
}

// Return number of timeslices

int CpmSubBlockV1::timeslices() const
{
  int slices = slices1();
  if (slices == 0 && format() == NEUTRAL) {
    slices = dataWords() / s_glinkBitsPerSlice;
  }
  if (slices == 0) slices = 1;
  return slices;
}

// Packing/Unpacking routines

bool CpmSubBlockV1::pack()
{
  bool rc = false;
  switch (version()) {
    case 1:
      switch (format()) {
        case NEUTRAL:
	  rc = packNeutral();
	  break;
        case UNCOMPRESSED:
	  rc = packUncompressed();
	  break;
        default:
	  break;
      }
      break;
    default:
      break;
  }
  return rc;
}

bool CpmSubBlockV1::unpack()
{
  bool rc = false;
  switch (version()) {
    case 1:
      switch (format()) {
        case NEUTRAL:
	  rc = unpackNeutral();
	  break;
        case UNCOMPRESSED:
	  rc = unpackUncompressed();
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

// Return data for given channel and pin offset

int CpmSubBlockV1::data(const int slice, const int channel,
                                         const int offset) const
{
  int dat = 0;
  if (slice >= 0 && slice < timeslices() &&
      channel >= 0 && channel < m_channels && !m_ttData.empty()) {
    const int pin  = 2 * (channel/s_wordsPerPin) + offset;
    const int pair = (channel % s_wordsPerPin) / 2;
    const int pos  = pin * s_pairsPerPin + pair;
    const int ix   = index(slice) * m_channels + pos;
    const uint32_t word = m_ttData[ix];
    if (channel % 2 == 0) {
           dat = (word >> s_ttDataABit) & s_ttDataMask;
    } else dat = (word >> s_ttDataBBit) & s_ttDataMask;
  }
  return dat;
}

// Return error for given channel and pin offset

int CpmSubBlockV1::error(const int slice, const int channel,
                                          const int offset) const
{
  int err = 0;
  if (slice >= 0 && slice < timeslices() &&
      channel >= 0 && channel < m_channels && !m_ttData.empty()) {
    const int pin  = 2 * (channel/s_wordsPerPin) + offset;
    const int pair = (channel % s_wordsPerPin) / 2;
    const int pos  = pin * s_pairsPerPin + pair;
    const int ix   = index(slice) * m_channels + pos;
    const uint32_t word = m_ttData[ix];
    if (channel % 2 == 0) {
      err  =  (word >> s_parityABit)   & 0x1;
      err |= ((word >> s_linkDownABit) & 0x1) << 1;
    } else {
      err  =  (word >> s_parityBBit)   & 0x1;
      err |= ((word >> s_linkDownBBit) & 0x1) << 1;
    }
  }
  return err;
}

// Return hit counts with given offset

unsigned int CpmSubBlockV1::hits(const int slice, const int offset) const
{
  unsigned int hit = 0;
  if (slice >= 0 && slice < timeslices() && !m_hitData.empty()) {
    hit = m_hitData[index(slice)*2 + offset] & s_threshMask;
  }
  return hit;
}

// Return data index appropriate to format

int CpmSubBlockV1::index(const int slice) const
{
  return (format() == NEUTRAL) ? slice : 0;
}

// Resize a data vector according to format

void CpmSubBlockV1::resize(std::vector<uint32_t>& vec, int channels)
{
  if (vec.empty()) {
    int size = channels;
    if (format() == NEUTRAL) size *= timeslices();
    vec.resize(size);
  }
}

// Pack neutral data

bool CpmSubBlockV1::packNeutral()
{
  resize(m_ttData, m_channels);
  resize(m_hitData, 2);
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    const unsigned int hit0 = hits0(slice);
    const unsigned int hit1 = hits1(slice);
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      // Trigger tower data
      for (int pair = 0; pair < s_pairsPerPin; ++pair) {
        for (int i = 0; i < 2; ++i) {
          const int channel = s_wordsPerPin*(pin/2) + 2*pair + i;
	  if ((pin & 0x1)) { // Odd pins Had, even Em
	    packerNeutral(pin, hadData(slice,  channel), s_ttBits);
	    packerNeutral(pin, hadError(slice, channel), s_errBits);
          } else {
	    packerNeutral(pin, emData(slice,  channel), s_ttBits);
	    packerNeutral(pin, emError(slice, channel), s_errBits);
	  }
        }
      }
      // Hits and Bunch Crossing number
      if (pin < s_hitWords) {
        packerNeutral(pin, hit0 >> pin*s_hitBits, s_hitBits);
      } else if (pin < 2*s_hitWords) {
        packerNeutral(pin, hit1 >> (pin - s_hitWords)*s_hitBits, s_hitBits);
      } else {
        packerNeutral(pin, bunchCrossing() >> (pin - 2*s_hitWords)*s_hitBits,
	                                                         s_hitBits);
      }
      // G-Link parity
      packerNeutralParity(pin);
    }
  }
  return true;
}

// Pack uncompressed data

bool CpmSubBlockV1::packUncompressed()
{
  // Trigger tower data
  std::vector<uint32_t>::const_iterator pos;
  for (pos = m_ttData.begin(); pos != m_ttData.end(); ++pos) {
    if (*pos) packer(*pos, s_wordLength);
  }
        
  // Hits data
  for (pos = m_hitData.begin(); pos != m_hitData.end(); ++pos) {
    if (*pos) packer(*pos, s_wordLength);
  }
  packerFlush();
  return true;
}

// Unpack neutral data

bool CpmSubBlockV1::unpackNeutral()
{
  resize(m_ttData, m_channels);
  resize(m_hitData, 2);
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    unsigned int hit0 = 0;
    unsigned int hit1 = 0;
    int bunchCrossing = 0;
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      // Trigger tower data
      for (int pair = 0; pair < s_pairsPerPin; ++pair) {
	for (int i = 0; i < 2; ++i) {
          const int channel = s_wordsPerPin*(pin/2) + 2*pair + i;
	  int em     = 0;
	  int had    = 0;
	  int emErr  = 0;
	  int hadErr = 0;
	  if ((pin & 0x1)) { // Odd pins Had, even Em
	    em     = emData(slice, channel);
	    had    = unpackerNeutral(pin, s_ttBits);
	    emErr  = emError(slice, channel);
	    hadErr = unpackerNeutral(pin, s_errBits);
	  } else {
	    em     = unpackerNeutral(pin, s_ttBits);
	    had    = hadData(slice, channel);
	    emErr  = unpackerNeutral(pin, s_errBits);
	    hadErr = hadError(slice, channel);
          }
	  fillTowerData(slice, channel, em, had, emErr, hadErr);
        }
      }
      // Hits and Bunch Crossing number
      if (pin < s_hitWords) {
        hit0 |= unpackerNeutral(pin, s_hitBits) << pin*s_hitBits;
      } else if (pin < 2*s_hitWords) {
        hit1 |= unpackerNeutral(pin, s_hitBits) << (pin - s_hitWords)*s_hitBits;
      } else {
	bunchCrossing |= unpackerNeutral(pin, s_hitBits)
	                                << (pin - 2*s_hitWords)*s_hitBits;
      }
      // G-Link parity error
      unpackerNeutralParityError(pin);
    }
    setHits(slice, hit0, hit1);
    setBunchCrossing(bunchCrossing);
  }
  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
  return rc;
}

// Unpack uncompressed data

bool CpmSubBlockV1::unpackUncompressed()
{
  resize(m_ttData, m_channels);
  resize(m_hitData, 2);
  unpackerInit();
  uint32_t word = unpacker(s_wordLength);
  while (unpackerSuccess()) {
    const int id = dataId(word);
    bool err = false;
    // Trigger tower data
    if (id == s_ttWordId) {
      const int ix = (word >> s_pairBit) & s_pairPinMask;
      if (ix < m_channels && m_ttData[ix] == 0) {
        m_ttData[ix] = word;
	const int pin  = ix/4;
	const int pair = ix%4;
	const int channel = s_wordsPerPin*(pin/2) + 2*pair;
	m_chanPresent[channel]   = 1;
	m_chanPresent[channel+1] = 1;
      } else err = true;
    // Hits
    } else {
      const int indicator = (word >> s_indicatorBit) & 0x1;
      if (m_hitData[indicator] == 0) m_hitData[indicator] = word;
      else err = true;
    }
    if (err) {
      setUnpackErrorCode(UNPACK_SOURCE_ID);
      return false;
    }
    word = unpacker(s_wordLength);
  }
  return true;
}

} // end namespace
