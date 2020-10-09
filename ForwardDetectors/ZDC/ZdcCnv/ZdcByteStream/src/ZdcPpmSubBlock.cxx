/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** Sub-Block class for PPM data.
 *
 *  This may be link to the equivalent in LVL1 code,
 *  TRY_LINK_LVL1
 *  Marco Leite
 *
 */

#include "ZdcByteStream/ZdcPpmCompression.h"

#include "ZdcByteStream/ZdcPpmSubBlock.h"


// Constant definitions

const uint32_t ZdcPpmSubBlock::s_wordIdVal;
const int      ZdcPpmSubBlock::s_errorMarker;

const int      ZdcPpmSubBlock::s_wordLen;
const int      ZdcPpmSubBlock::s_lutBit;
const int      ZdcPpmSubBlock::s_bcidLutBit;
const int      ZdcPpmSubBlock::s_fadcBit;
const int      ZdcPpmSubBlock::s_bcidFadcBit;
const uint32_t ZdcPpmSubBlock::s_lutMask;
const uint32_t ZdcPpmSubBlock::s_bcidLutMask;
const uint32_t ZdcPpmSubBlock::s_fadcMask;
const uint32_t ZdcPpmSubBlock::s_bcidFadcMask;

const int      ZdcPpmSubBlock::s_channels;
const int      ZdcPpmSubBlock::s_glinkPins;
const int      ZdcPpmSubBlock::s_asicChannels;
const int      ZdcPpmSubBlock::s_dataBits;
const int      ZdcPpmSubBlock::s_errorBits;
const int      ZdcPpmSubBlock::s_bunchCrossingBits;

const uint32_t ZdcPpmSubBlock::s_errorMask;
const int      ZdcPpmSubBlock::s_glinkPinParityBit;
const int      ZdcPpmSubBlock::s_fpgaCorruptBit;
const int      ZdcPpmSubBlock::s_bunchMismatchBit;
const int      ZdcPpmSubBlock::s_eventMismatchBit;
const int      ZdcPpmSubBlock::s_asicFullBit;
const int      ZdcPpmSubBlock::s_timeoutBit;
const int      ZdcPpmSubBlock::s_mcmAbsentBit;
const int      ZdcPpmSubBlock::s_channelDisabledBit;

// This is faster than going through a operation for every channel for every event
//As tested, the right order is A D B C
const int      ZdcPpmSubBlock::s_ppmChannel[64] = {0,  4,  8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,  60,
                                                    3,  7, 11,  15,  19,  23,  27,  31,  35,  39,  43,  47,  51,  55,  59,  63,
													1,  5,  9,  13,  17,  21,  25,  29,  33,  37,  41,  45,  49,  53,  57,  61,
                                                    2,  6, 10,  14,  18,  22,  26,  30,  34,  38,  42,  46,  50,  54,  58,  62 };


ZdcPpmSubBlock::ZdcPpmSubBlock() :
		m_globalError(0),
        m_lutOffset(-1),
        m_fadcOffset(-1),
        m_pedestal(10),
        m_fadcBaseline(0),
        m_fadcThreshold(0),
        m_runNumber(0)
{
}

ZdcPpmSubBlock::~ZdcPpmSubBlock()
{
}

// Clear all data

void ZdcPpmSubBlock::clear()
{
  ZdcSubBlock::clear();
  m_globalError   = 0;
  m_lutOffset     = -1;
  m_fadcOffset    = -1;
  m_datamap.clear();
  m_errormap.clear();
}

// Store PPM header

void ZdcPpmSubBlock::setPpmHeader(const int version, const int format,
                               const int seqno, const int crate,
                               const int module, const int slicesFadc,
			       const int slicesLut)
{
  setHeader(s_wordIdVal, version, format, seqno, crate, module,
                                                 slicesFadc, slicesLut);
}

// Store PPM error block header

void ZdcPpmSubBlock::setPpmErrorHeader(const int version, const int format,
                                    const int crate, const int module,
				    const int slicesFadc, const int slicesLut)
{
  setHeader(s_wordIdVal, version, format, s_errorMarker, crate, module,
                                                 slicesFadc, slicesLut);
}

// Return the number of FADC slices

int ZdcPpmSubBlock::slicesFadc() const
{
  int slices = slices2();
  if (slices == 0 && format() == NEUTRAL) {
    slices = dataWords()/(s_asicChannels*s_dataBits) - slicesLut();
  }
  if (slices <= 0) slices = 1;
  return slices;
}

// Return the number of LUT slices

int ZdcPpmSubBlock::slicesLut() const
{
  int slices = slices1();
  if (slices == 0) slices = 1;
  return slices;
}

// Store PPM data for later packing

void ZdcPpmSubBlock::fillPpmData(const int chan, const std::vector<int>& lut,
                                              const std::vector<int>& fadc,
				              const std::vector<int>& bcidLut,
				              const std::vector<int>& bcidFadc)
{
  const int sliceL = slicesLut();
  const int sliceF = slicesFadc();
  const int slices = sliceL + sliceF;
  const int chanPerSubBlock = channelsPerSubBlock();
  if (chanPerSubBlock == 0) {
    setUnpackErrorCode(ZdcSubBlock::UNPACK_FORMAT);
    m_datamap.clear();
    return;
  }
  int dataSize = m_datamap.size();
  if (dataSize == 0) {
    dataSize = slices * chanPerSubBlock;
    m_datamap.resize(dataSize);
  }
  int offset = (chan % chanPerSubBlock) * slices;
  if (offset + slices <= dataSize) {
    for (int pos = 0; pos < sliceL; ++pos) {
      uint32_t datum = (lut[pos] & s_lutMask) << s_lutBit;
      datum |= (bcidLut[pos] & s_bcidLutMask) << s_bcidLutBit;
      m_datamap[offset + pos] = datum;
    }
    offset += sliceL;
    for (int pos = 0; pos < sliceF; ++pos) {
      const int adc = (fadc[pos] > 0) ? fadc[pos] : 0;
      uint32_t datum = (adc & s_fadcMask) << s_fadcBit;
      datum |= (bcidFadc[pos] & s_bcidFadcMask) << s_bcidFadcBit;
      m_datamap[offset + pos] = datum;
    }
  }
}

// Return unpacked data for given channel

void ZdcPpmSubBlock::ppmData(const int chan, std::vector<int>& lut,
                                          std::vector<int>& fadc,
				          std::vector<int>& bcidLut,
				          std::vector<int>& bcidFadc) const
{
  lut.clear();
  fadc.clear();
  bcidLut.clear();
  bcidFadc.clear();
  const int sliceL = slicesLut();
  const int sliceF = slicesFadc();
  const int chans = channelsPerSubBlock();
  if (chans == 0) {
    return;
  }
  int beg = (chan % chans) * (sliceL + sliceF);
  int end = beg + sliceL;
  if (size_t(end + sliceF) <= m_datamap.size()) {
    for (int pos = beg; pos < end; ++pos) {
      const uint32_t word = m_datamap[pos];
      lut.push_back((word >> s_lutBit) & s_lutMask);
      bcidLut.push_back((word >> s_bcidLutBit) & s_bcidLutMask);
    }
    beg += sliceL;
    end += sliceF;
    for (int pos = beg; pos < end; ++pos) {
      const uint32_t word = m_datamap[pos];
      fadc.push_back((word >> s_fadcBit) & s_fadcMask);
      bcidFadc.push_back((word >> s_bcidFadcBit) & s_bcidFadcMask);
    }
  } else {
    lut.resize(sliceL);
    fadc.resize(sliceF);
    bcidLut.resize(sliceL);
    bcidFadc.resize(sliceF);
  }
}

// Store an error word corresponding to a data channel

void ZdcPpmSubBlock::fillPpmError(const int chan, const int errorWord)
{
  if (m_errormap.empty()) m_errormap.resize(s_glinkPins);
  // Expand one ASIC channel disabled bit to four
  const uint32_t chanDisabled = (errorWord & 0x1) << asic(chan);
  uint32_t word = (((errorWord >> 1) << s_asicChannels)
                   | chanDisabled) & s_errorMask;
  m_errormap[pin(chan)] |= word;
  m_globalError |= word;
}

// Store an error word corresponding to a G-Link pin

void ZdcPpmSubBlock::fillPpmPinError(const int pin, const int errorWord)
{
  if (m_errormap.empty()) m_errormap.resize(s_glinkPins);
  m_errormap[pin] = errorWord & s_errorMask;
  m_globalError = 0;
  for (uint32_t word : m_errormap) {
    m_globalError |= word;
  }
}

// Return the error word for a data channel

int ZdcPpmSubBlock::ppmError(const int chan) const
{
  int err = 0;
  if ( !m_errormap.empty()) {
    // Replace the four ASIC channel disabled bits with just the one
    // corresponding to the data channel
    err = (((m_errormap[pin(chan)] & s_errorMask) >> s_asicChannels) << 1)
                                                  | channelDisabled(chan);
  }
  return err;
}

// Return the error word for a G-Link pin

int ZdcPpmSubBlock::ppmPinError(const int pin) const
{
  int err = 0;
  if ( !m_errormap.empty()) err = m_errormap[pin] & s_errorMask;
  return err;
}

// Return global error bit

bool ZdcPpmSubBlock::errorBit(const int bit) const
{
  return m_globalError & (0x1 << bit);
}

// Packing/Unpacking routines

bool ZdcPpmSubBlock::pack()
{
  bool rc = false;
  switch (version()) {
    case 1:
      switch (format()) {
        case NEUTRAL:
	  rc = packNeutral();
	  break;
        case UNCOMPRESSED:
	  switch (seqno()) {
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
	  rc = ZdcPpmCompression::pack(*this);
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

bool ZdcPpmSubBlock::unpack()
{
  bool rc = false;
  switch (version()) {
    case 1:
      switch (format()) {
        case NEUTRAL:
	  rc = unpackNeutral();
	  break;
        case UNCOMPRESSED:
	  switch (seqno()) {
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
	  rc = ZdcPpmCompression::unpack(*this);
	  break;
        default:
	  setUnpackErrorCode(ZdcSubBlock::UNPACK_FORMAT);
	  break;
      }
      break;
    default:
      setUnpackErrorCode(ZdcSubBlock::UNPACK_VERSION);
      break;
  }
  return rc;
}

// Pack neutral data

bool ZdcPpmSubBlock::packNeutral()
{
  const int slices   = slicesLut() + slicesFadc();
  const int channels = channelsPerSubBlock();
  if (m_datamap.empty()) m_datamap.resize(slices * channels);
  // Bunch crossing number
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    uint32_t bc = 0;
    if (pin < s_bunchCrossingBits) bc = (bunchCrossing() >> pin) & 0x1;
    packerNeutral(pin, bc, 1);
  }
  // Data
  std::vector<uint32_t>::const_iterator pos = m_datamap.begin();
  for (int asic = 0; asic < s_asicChannels; ++asic) {
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      for (int sl = 0; sl < slices; ++sl) {
        packerNeutral(pin, *pos, s_dataBits);
	++pos;
      }
    }
  }
  // Errors, including GP
  if (m_errormap.empty()) m_errormap.resize(s_glinkPins);
  pos = m_errormap.begin();
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    packerNeutral(pin, *pos, s_errorBits);
    packerNeutralParity(pin);
    ++pos;
  }
  return true;
}

// Pack uncompressed data

bool ZdcPpmSubBlock::packUncompressedData()
{
  const int slices   = slicesLut() + slicesFadc();
  const int channels = channelsPerSubBlock();
  if (m_datamap.empty()) m_datamap.resize(slices * channels);
  for (int sl = 0; sl < slices; ++sl) {
    for (int chan = 0; chan < channels; ++chan) {
      packer(m_datamap[sl + chan*slices], s_wordLen);
    }
  }
  packerFlush();
  return true;
}

// Pack uncompressed error data

bool ZdcPpmSubBlock::packUncompressedErrors()
{
  if (m_errormap.empty()) m_errormap.resize(s_glinkPins);
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    packer(m_errormap[pin], s_wordLen);
  }
  packerFlush();
  return true;
}

// Unpack neutral data

bool ZdcPpmSubBlock::unpackNeutral()
{
  const int slices = slicesLut() + slicesFadc();
  m_datamap.clear();
  // Bunch Crossing number
  int bunchCrossing = 0;
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    const int bc = unpackerNeutral(pin, 1);
    if (pin < s_bunchCrossingBits) bunchCrossing |= bc << pin;
  }
  setBunchCrossing(bunchCrossing);
  // Data
  for (int asic = 0; asic < s_asicChannels; ++asic) {
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      for (int sl = 0; sl < slices; ++sl) {
        m_datamap.push_back(unpackerNeutral(pin, s_dataBits));
      }
    }
  }
  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(ZdcSubBlock::UNPACK_DATA_TRUNCATED);
  // Errors
  m_errormap.clear();
  m_globalError = 0;
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    uint32_t error = unpackerNeutral(pin, s_errorBits);
    error |= unpackerNeutralParityError(pin) << s_errorBits;
    m_errormap.push_back(error);
    m_globalError |= error;
  }
  return rc;
}

// Unpack uncompressed data

bool ZdcPpmSubBlock::unpackUncompressedData()
{
  const int slices = slicesLut() + slicesFadc();
  const int channels = channelsPerSubBlock();
  m_datamap.resize(slices * channels);
  unpackerInit();
  for (int sl = 0; sl < slices; ++sl) {
    for (int chan = 0; chan < channels; ++chan) {
      m_datamap[sl + chan*slices] = unpacker(s_wordLen);
    }
  }
  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(ZdcSubBlock::UNPACK_DATA_TRUNCATED);
  return rc;
}

// Unpack uncompressed error data

bool ZdcPpmSubBlock::unpackUncompressedErrors()
{
  unpackerInit();
  m_errormap.clear();
  m_globalError = 0;
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    uint32_t word = unpacker(s_wordLen);
    m_errormap.push_back(word);
    m_globalError |= word;
  }
  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(ZdcSubBlock::UNPACK_DATA_TRUNCATED);
  return rc;
}

// Return the number of channels per sub-block

int ZdcPpmSubBlock::channelsPerSubBlock(const int version, const int format)
{
  int chan = 0;
  switch (version) {
    case 1:
      switch (format) {
        case UNCOMPRESSED:
	  chan = s_channels/s_asicChannels;
	  break;
        case NEUTRAL:
        case COMPRESSED:
        case SUPERCOMPRESSED:
	  chan = s_channels;
	  break;
        default:
	  break;
      }
      break;
    default:
      break;
  }
  return chan;
}

int ZdcPpmSubBlock::channelsPerSubBlock() const
{
  return channelsPerSubBlock(version(), format());
}

// Check if a header word is for an error block

bool ZdcPpmSubBlock::errorBlock(const uint32_t word)
{
  bool rc = false;
  if (format(word) == UNCOMPRESSED &&
       seqno(word) == s_errorMarker) rc = true;
  return rc;
}


