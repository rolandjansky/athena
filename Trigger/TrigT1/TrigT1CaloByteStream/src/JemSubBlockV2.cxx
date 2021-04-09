/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "JemJetElement.h"
#include "JemSubBlockV2.h"

namespace LVL1BS {

// Constant definitions

const int      JemSubBlockV2::s_wordIdVal;

const int      JemSubBlockV2::s_wordLength;

const int      JemSubBlockV2::s_dataIdBit;
const int      JemSubBlockV2::s_jeWordId;
const uint32_t JemSubBlockV2::s_dataIdMask;

const uint32_t JemSubBlockV2::s_energyWordId;
const int      JemSubBlockV2::s_sourceIdBit;
const uint32_t JemSubBlockV2::s_sourceIdMask;
const int      JemSubBlockV2::s_etId;
const int      JemSubBlockV2::s_exEyId;
const int      JemSubBlockV2::s_exBit;
const int      JemSubBlockV2::s_eyBit;
const int      JemSubBlockV2::s_etBit;
const uint32_t JemSubBlockV2::s_exMask;
const uint32_t JemSubBlockV2::s_eyMask;
const uint32_t JemSubBlockV2::s_etMask;

const int      JemSubBlockV2::s_pairsPerPin;
const int      JemSubBlockV2::s_jetElementBits;
const int      JemSubBlockV2::s_jePaddingBits;
const int      JemSubBlockV2::s_energyBits;
const int      JemSubBlockV2::s_bunchCrossingBits;
const int      JemSubBlockV2::s_energyPaddingBits;
const int      JemSubBlockV2::s_glinkBitsPerSlice;


JemSubBlockV2::JemSubBlockV2() : m_channels(44), m_energyWords(2)
{
}

JemSubBlockV2::~JemSubBlockV2()
{
}

// Clear all data

void JemSubBlockV2::clear()
{
  L1CaloSubBlock::clear();
  m_jeData.clear();
  m_energySubsums.clear();
}

// Store JEM header

void JemSubBlockV2::setJemHeader(const int version, const int format,
                                 const int slice, const int crate,
                                 const int module, const int timeslices)
{
  setHeader(s_wordIdVal, version, format, slice, crate, module, 0, timeslices);
}

// Store jet element data

void JemSubBlockV2::fillJetElement(const int slice, const JemJetElement& jetEle)
{
  if (jetEle.data()) {
    const int channel = jetEle.channel();
    if (channel < m_channels) {
      resize(m_jeData, m_channels);
      m_jeData[index(slice, m_channels) + channel] = jetEle.data();
    }
  }
}

// Store energy subsum data

void JemSubBlockV2::setEnergySubsums(const int slice, const unsigned int ex,
                                     const unsigned int ey, const unsigned int et)
{
  uint32_t word1 = 0;
  uint32_t word2 = 0;
  word1 |= (et & s_etMask) << s_etBit;
  word2 |= (ex & s_exMask) << s_exBit;
  word2 |= (ey & s_eyMask) << s_eyBit;
  if (word1 || word2) {
    resize(m_energySubsums, m_energyWords);
    const int ix = index(slice, m_energyWords);
    if (word1) {
      word1 |= s_etId         << s_sourceIdBit;
      word1 |= s_energyWordId << s_dataIdBit;
      m_energySubsums[ix] = word1;
    }
    if (word2) {
      word2 |= s_exEyId       << s_sourceIdBit;
      word2 |= s_energyWordId << s_dataIdBit;
      m_energySubsums[ix+1] = word2;
    }
  }
}

// Return jet element for given channel

JemJetElement JemSubBlockV2::jetElement(const int slice, const int channel) const
{
  uint32_t je = 0;
  if (slice >= 0 && slice < timeslices() &&
      channel >= 0 && channel < m_channels && !m_jeData.empty()) {
    je = m_jeData[index(slice, m_channels) + channel];
  }
  return JemJetElement(je);
}

// Return energy subsum Ex

unsigned int JemSubBlockV2::ex(const int slice) const
{
  unsigned int ex = 0;
  if (slice >= 0 && slice < timeslices() && !m_energySubsums.empty()) {
    ex = (m_energySubsums[index(slice, m_energyWords)+1] >> s_exBit) & s_exMask;
  }
  return ex;
}

// Return energy subsum Ey

unsigned int JemSubBlockV2::ey(const int slice) const
{
  unsigned int ey = 0;
  if (slice >= 0 && slice < timeslices() && !m_energySubsums.empty()) {
    ey = (m_energySubsums[index(slice, m_energyWords)+1] >> s_eyBit) & s_eyMask;
  }
  return ey;
}

// Return energy subsum Et

unsigned int JemSubBlockV2::et(const int slice) const
{
  unsigned int et = 0;
  if (slice >= 0 && slice < timeslices() && !m_energySubsums.empty()) {
    et = (m_energySubsums[index(slice, m_energyWords)] >> s_etBit) & s_etMask;
  }
  return et;
}

// Return number of timeslices

int JemSubBlockV2::timeslices() const
{
  int slices = slices1();
  if (slices == 0 && format() == NEUTRAL) {
    slices = dataWords() / s_glinkBitsPerSlice;
  }
  if (slices == 0) slices = 1;
  return slices;
}

// Packing/Unpacking routines

bool JemSubBlockV2::pack()
{
  bool rc = false;
  switch (version()) {
    case 2:                                                    //<< CHECK
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

bool JemSubBlockV2::unpack()
{
  bool rc = false;
  switch (version()) {
    case 1:                                                    //<< CHECK
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

int JemSubBlockV2::index(const int slice, const int channels) const
{
  return (format() == NEUTRAL) ? slice*channels : 0;
}

// Resize a data vector according to format

void JemSubBlockV2::resize(std::vector<uint32_t>& vec, const int channels)
{
  if (vec.empty()) {
    int size = channels;
    if (format() == NEUTRAL) size *= timeslices();
    vec.resize(size);
  }
}

// Pack neutral data

bool JemSubBlockV2::packNeutral()
{
  resize(m_jeData, m_channels);
  resize(m_energySubsums, m_energyWords);
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
    // Energy Sums
    ++lastpin;
    packerNeutral(lastpin, ex(slice), s_energyBits);
    packerNeutral(lastpin, ey(slice), s_energyBits);
    packerNeutral(lastpin, et(slice), s_energyBits);
    // Bunch Crossing number and padding
    packerNeutral(lastpin, bunchCrossing(), s_bunchCrossingBits);
    packerNeutral(lastpin, 0, s_energyPaddingBits);
    // G-Link parity
    for (int pin = 0; pin <= lastpin; ++pin) packerNeutralParity(pin);
  }
  return true;
}

// Pack uncompressed data

bool JemSubBlockV2::packUncompressed()
{
  // Jet element data
  std::vector<uint32_t>::const_iterator pos;
  for (pos = m_jeData.begin(); pos != m_jeData.end(); ++pos) {
    if (*pos) packer(*pos, s_wordLength);
  }
        
  // Subsum data
  if ( !m_energySubsums.empty() ) {
    if (m_energySubsums[0]) packer(m_energySubsums[0], s_wordLength);
    if (m_energySubsums[1]) packer(m_energySubsums[1], s_wordLength);
  }
  packerFlush();
  return true;
}

// Unpack neutral data

bool JemSubBlockV2::unpackNeutral()
{
  resize(m_jeData, m_channels);
  resize(m_energySubsums, m_energyWords);
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
    // Energy Sums
    ++lastpin;
    const unsigned int ex = unpackerNeutral(lastpin, s_energyBits);
    const unsigned int ey = unpackerNeutral(lastpin, s_energyBits);
    const unsigned int et = unpackerNeutral(lastpin, s_energyBits);
    setEnergySubsums(slice, ex, ey, et);
    // Bunch Crossing number and padding
    setBunchCrossing(unpackerNeutral(lastpin, s_bunchCrossingBits));
    unpackerNeutral(lastpin, s_energyPaddingBits);
    // G-Link parity errors
    for (int pin = 0; pin <= lastpin; ++pin) unpackerNeutralParityError(pin);
  }
  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
  return rc;
}

// Unpack uncompressed data

bool JemSubBlockV2::unpackUncompressed()
{
  resize(m_jeData, m_channels);
  resize(m_energySubsums, m_energyWords);
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
    // Energy subsums
    } else if (id == s_energyWordId) {
      switch (sourceId(word)) {
        case s_etId: {
	  if (m_energySubsums[0] == 0) m_energySubsums[0] = word;
	  else err = true;
	  break;
	}
        case s_exEyId: {
	  if (m_energySubsums[1] == 0) m_energySubsums[1] = word;
	  else err = true;
	  break;
	}
        default:
	  err = true;
	  break;
      }
    } else err = true;
    if (err) {
      setUnpackErrorCode(UNPACK_SOURCE_ID);
      return false;
    }
    word = unpacker(s_wordLength);
  }
  return true;
}

} // end namespace
