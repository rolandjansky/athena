/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JemJetElement.h"
#include "JemSubBlock.h"

namespace LVL1BS {

// Constant definitions

const int      JemSubBlock::s_wordIdVal;

const int      JemSubBlock::s_wordLength;

const int      JemSubBlock::s_dataIdBit;
const int      JemSubBlock::s_jeWordId;
const uint32_t JemSubBlock::s_dataIdMask;

const int      JemSubBlock::s_threshBit;
const int      JemSubBlock::s_sourceIdBit;
const int      JemSubBlock::s_jetIndicatorBit;
const int      JemSubBlock::s_jetIndicator;
const int      JemSubBlock::s_mainThreshId;
const int      JemSubBlock::s_mainFwdThreshId;
const int      JemSubBlock::s_threshWordId;
const uint32_t JemSubBlock::s_threshMask;
const uint32_t JemSubBlock::s_sourceIdMask;

const int      JemSubBlock::s_exBit;
const int      JemSubBlock::s_eyBit;
const int      JemSubBlock::s_etBit;
const int      JemSubBlock::s_sumIndicatorBit;
const int      JemSubBlock::s_sumIndicator;
const int      JemSubBlock::s_subsumId;
const uint32_t JemSubBlock::s_exMask;
const uint32_t JemSubBlock::s_eyMask;
const uint32_t JemSubBlock::s_etMask;

const int      JemSubBlock::s_pairsPerPin;
const int      JemSubBlock::s_jetElementBits;
const int      JemSubBlock::s_jePaddingBits;
const int      JemSubBlock::s_jetHitsBits;
const int      JemSubBlock::s_energyBits;
const int      JemSubBlock::s_bunchCrossingBits;
const int      JemSubBlock::s_hitPaddingBits;
const int      JemSubBlock::s_glinkBitsPerSlice;


JemSubBlock::JemSubBlock() : m_channels(44)
{
}

JemSubBlock::~JemSubBlock()
{
}

// Clear all data

void JemSubBlock::clear()
{
  L1CaloSubBlock::clear();
  m_jeData.clear();
  m_jetHits.clear();
  m_energySubsums.clear();
}

// Store JEM header

void JemSubBlock::setJemHeader(const int version, const int format,
                               const int slice, const int crate,
                               const int module, const int timeslices)
{
  setHeader(s_wordIdVal, version, format, slice, crate, module, 0, timeslices);
}

// Store jet element data

void JemSubBlock::fillJetElement(const int slice, const JemJetElement& jetEle)
{
  if (jetEle.data()) {
    const int channel = jetEle.channel();
    if (channel < m_channels) {
      resize(m_jeData, m_channels);
      m_jeData[index(slice)*m_channels + channel] = jetEle.data();
    }
  }
}

// Store jet hit counts

void JemSubBlock::setJetHits(const int slice, const unsigned int hits)
{
  if (hits) {
    const int jem = module();
    const int sourceId = (jem == 0 || jem == 7 || jem == 8 || jem == 15)
                         ? s_mainFwdThreshId : s_mainThreshId;
    uint32_t word = 0;
    word |= (hits           & s_threshMask)   << s_threshBit;
    word |=  s_jetIndicator                   << s_jetIndicatorBit;
    word |= (sourceId       & s_sourceIdMask) << s_sourceIdBit;
    word |=  s_threshWordId                   << s_dataIdBit;
    resize(m_jetHits);
    m_jetHits[index(slice)] = word;
  }
}

// Store energy subsum data

void JemSubBlock::setEnergySubsums(const int slice, const unsigned int ex,
                                   const unsigned int ey, const unsigned int et)
{
  uint32_t word = 0;
  word |= (ex & s_exMask) << s_exBit;
  word |= (ey & s_eyMask) << s_eyBit;
  word |= (et & s_etMask) << s_etBit;
  if (word) {
    word |= s_sumIndicator << s_sumIndicatorBit;
    word |= s_subsumId     << s_sourceIdBit;
    word |= s_threshWordId << s_dataIdBit;
    resize(m_energySubsums);
    m_energySubsums[index(slice)] = word;
  }
}

// Return jet element for given channel

JemJetElement JemSubBlock::jetElement(const int slice, const int channel) const
{
  uint32_t je = 0;
  if (slice >= 0 && slice < timeslices() &&
      channel >= 0 && channel < m_channels && !m_jeData.empty()) {
    je = m_jeData[index(slice)*m_channels + channel];
  }
  return JemJetElement(je);
}

// Return jet hit counts

unsigned int JemSubBlock::jetHits(const int slice) const
{
  unsigned int hits = 0;
  if (slice >= 0 && slice < timeslices() && !m_jetHits.empty()) {
    hits = (m_jetHits[index(slice)] >> s_threshBit) & s_threshMask;
  }
  return hits;
}

// Return energy subsum Ex

unsigned int JemSubBlock::ex(const int slice) const
{
  unsigned int ex = 0;
  if (slice >= 0 && slice < timeslices() && !m_energySubsums.empty()) {
    ex = (m_energySubsums[index(slice)] >> s_exBit) & s_exMask;
  }
  return ex;
}

// Return energy subsum Ey

unsigned int JemSubBlock::ey(const int slice) const
{
  unsigned int ey = 0;
  if (slice >= 0 && slice < timeslices() && !m_energySubsums.empty()) {
    ey = (m_energySubsums[index(slice)] >> s_eyBit) & s_eyMask;
  }
  return ey;
}

// Return energy subsum Et

unsigned int JemSubBlock::et(const int slice) const
{
  unsigned int et = 0;
  if (slice >= 0 && slice < timeslices() && !m_energySubsums.empty()) {
    et = (m_energySubsums[index(slice)] >> s_etBit) & s_etMask;
  }
  return et;
}

// Return number of timeslices

int JemSubBlock::timeslices() const
{
  int slices = slices1();
  if (slices == 0 && format() == NEUTRAL) {
    slices = dataWords() / s_glinkBitsPerSlice;
  }
  if (slices == 0) slices = 1;
  return slices;
}

// Packing/Unpacking routines

bool JemSubBlock::pack()
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

bool JemSubBlock::unpack()
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

// Return data index appropriate to format

int JemSubBlock::index(const int slice) const
{
  return (format() == NEUTRAL) ? slice : 0;
}

// Resize a data vector according to format

void JemSubBlock::resize(std::vector<uint32_t>& vec, const int channels)
{
  if (vec.empty()) {
    int size = channels;
    if (format() == NEUTRAL) size *= timeslices();
    vec.resize(size);
  }
}

// Pack neutral data

bool JemSubBlock::packNeutral()
{
  resize(m_jeData, m_channels);
  resize(m_jetHits);
  resize(m_energySubsums);
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    // Jet element data
    for (int channel = 0; channel < m_channels; ++channel) {
      const int pin = channel / s_pairsPerPin;
      const JemJetElement je = jetElement(slice, channel);
      packerNeutral(pin, je.emData(), s_jetElementBits);
      packerNeutral(pin, je.emParity(), 1);
      packerNeutral(pin, je.linkError(), 1);
      packerNeutral(pin, je.hadData(), s_jetElementBits);
      packerNeutral(pin, je.hadParity(), 1);
      packerNeutral(pin, (je.linkError() >> 1), 1);
    }
    // Pad out last jet element pin
    int lastpin = (m_channels - 1) / s_pairsPerPin;
    packerNeutral(lastpin, 0, s_jePaddingBits);
    // Jet Hits and Energy Sums with parity bits
    ++lastpin;
    packerNeutral(lastpin, jetHits(slice), s_jetHitsBits);
    packerNeutral(lastpin, parityBit(1, jetHits(slice), s_jetHitsBits), 1);
    packerNeutral(lastpin, ex(slice), s_energyBits);
    packerNeutral(lastpin, ey(slice), s_energyBits);
    packerNeutral(lastpin, et(slice), s_energyBits);
    int parity =  parityBit(1, ex(slice), s_energyBits);
    parity = parityBit(parity, ey(slice), s_energyBits);
    parity = parityBit(parity, et(slice), s_energyBits);
    packerNeutral(lastpin, parity, 1);
    // Bunch Crossing number and padding
    packerNeutral(lastpin, bunchCrossing(), s_bunchCrossingBits);
    packerNeutral(lastpin, 0, s_hitPaddingBits);
    // G-Link parity
    for (int pin = 0; pin <= lastpin; ++pin) packerNeutralParity(pin);
  }
  return true;
}

// Pack uncompressed data

bool JemSubBlock::packUncompressed()
{
  // Jet element data
  std::vector<uint32_t>::const_iterator pos;
  for (pos = m_jeData.begin(); pos != m_jeData.end(); ++pos) {
    if (*pos) packer(*pos, s_wordLength);
  }
        
  // Hits and Subsum data
  if ( !m_jetHits.empty() && m_jetHits[0]) packer(m_jetHits[0], s_wordLength);
  if ( !m_energySubsums.empty() && m_energySubsums[0]) {
    packer(m_energySubsums[0], s_wordLength);
  }
  packerFlush();
  return true;
}

// Unpack neutral data

bool JemSubBlock::unpackNeutral()
{
  resize(m_jeData, m_channels);
  resize(m_jetHits);
  resize(m_energySubsums);
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    // Jet element data
    for (int channel = 0; channel < m_channels; ++channel) {
      const int pin = channel / s_pairsPerPin;
      const int emData     = unpackerNeutral(pin, s_jetElementBits);
      const int emParity   = unpackerNeutral(pin, 1);
            int linkError  = unpackerNeutral(pin, 1);
      const int hadData    = unpackerNeutral(pin, s_jetElementBits);
      const int hadParity  = unpackerNeutral(pin, 1);
                linkError |= unpackerNeutral(pin, 1) << 1;
      const JemJetElement je(channel, emData, hadData, emParity,
                                              hadParity, linkError);
      fillJetElement(slice, je);
    }
    // Padding from last jet element pin
    int lastpin = (m_channels - 1) / s_pairsPerPin;
    unpackerNeutral(lastpin, s_jePaddingBits);
    // Jet Hits and Energy Sums
    ++lastpin;
    setJetHits(slice, unpackerNeutral(lastpin, s_jetHitsBits));
    unpackerNeutral(lastpin, 1); // parity bit
    const unsigned int ex = unpackerNeutral(lastpin, s_energyBits);
    const unsigned int ey = unpackerNeutral(lastpin, s_energyBits);
    const unsigned int et = unpackerNeutral(lastpin, s_energyBits);
    setEnergySubsums(slice, ex, ey, et);
    unpackerNeutral(lastpin, 1); // parity bit
    // Bunch Crossing number and padding
    setBunchCrossing(unpackerNeutral(lastpin, s_bunchCrossingBits));
    unpackerNeutral(lastpin, s_hitPaddingBits);
    // G-Link parity errors
    for (int pin = 0; pin <= lastpin; ++pin) unpackerNeutralParityError(pin);
  }
  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
  return rc;
}

// Unpack uncompressed data

bool JemSubBlock::unpackUncompressed()
{
  resize(m_jeData, m_channels);
  resize(m_jetHits);
  resize(m_energySubsums);
  unpackerInit();
  uint32_t word = unpacker(s_wordLength);
  while (unpackerSuccess()) {
    const int id = dataId(word);
    bool err = false;
    // Jet element data
    if (id == s_jeWordId) {
      const JemJetElement jetEle(word);
      const int channel = jetEle.channel();
      if (channel < m_channels && m_jeData[channel] == 0) {
        m_jeData[channel] = word;
      } else err = true;
    // Other data
    } else {
      switch (sourceId(word)) {
	// Jet hit counts/thresholds
        case s_mainThreshId:
        case s_mainFwdThreshId: {
	  if (m_jetHits[0] == 0) m_jetHits[0] = word;
	  else err = true;
          break;
	}
	// Energy subsums
        case s_subsumId: {
	  if (m_energySubsums[0] == 0) m_energySubsums[0] = word;
	  else err = true;
	  break;
	}
        default:
	  err = true;
	  break;
      }
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
