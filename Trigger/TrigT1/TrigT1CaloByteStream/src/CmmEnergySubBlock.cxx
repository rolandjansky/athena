/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "CmmEnergySubBlock.h"

namespace LVL1BS {

// Constant definitions

const int      CmmEnergySubBlock::s_wordLength;

const int      CmmEnergySubBlock::s_exBit;
const int      CmmEnergySubBlock::s_eyBit;
const int      CmmEnergySubBlock::s_etBit;
const int      CmmEnergySubBlock::s_jemErrorBit;
const int      CmmEnergySubBlock::s_errorBit;
const int      CmmEnergySubBlock::s_etMissBit;
const int      CmmEnergySubBlock::s_etHitsBit;
const int      CmmEnergySubBlock::s_etMissSigBit;
const int      CmmEnergySubBlock::s_sourceIdBit;
const int      CmmEnergySubBlock::s_dataWordIdBit;
const int      CmmEnergySubBlock::s_dataWordId;
const int      CmmEnergySubBlock::s_maxSums;
const int      CmmEnergySubBlock::s_maxJems;
const uint32_t CmmEnergySubBlock::s_exMask;
const uint32_t CmmEnergySubBlock::s_eyMask;
const uint32_t CmmEnergySubBlock::s_etMask;
const uint32_t CmmEnergySubBlock::s_errorMask;
const uint32_t CmmEnergySubBlock::s_remoteErrorMask;
const uint32_t CmmEnergySubBlock::s_etMissMask;
const uint32_t CmmEnergySubBlock::s_etHitsMask;
const uint32_t CmmEnergySubBlock::s_etHitsMaskV1;
const uint32_t CmmEnergySubBlock::s_etMissSigMask;
const uint32_t CmmEnergySubBlock::s_sumsMask;
const uint32_t CmmEnergySubBlock::s_sourceIdMask;

const int      CmmEnergySubBlock::s_jemSumBits;
const int      CmmEnergySubBlock::s_sumBits;
const int      CmmEnergySubBlock::s_bunchCrossingBits;
const int      CmmEnergySubBlock::s_paddingBits;
const int      CmmEnergySubBlock::s_fifoOverflowPin;


CmmEnergySubBlock::CmmEnergySubBlock()
{
}

CmmEnergySubBlock::~CmmEnergySubBlock()
{
}

// Clear all data

void CmmEnergySubBlock::clear()
{
  L1CaloSubBlock::clear();
  m_sumsData.clear();
}

// Return Ex subsum for given JEM or source ID

unsigned int CmmEnergySubBlock::ex(const int slice, const int source) const
{
  unsigned int ex = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    if (source >= 0 && source < s_maxJems) {
      ex = (m_sumsData[index(slice, source)] >> s_exBit) & s_exMask;
    } else if (source == REMOTE || source == LOCAL || source == TOTAL) {
      ex = m_sumsData[index(slice, source)] & s_sumsMask;
    }
  }
  return ex;
}

// Return Ey subsum for given JEM or source ID

unsigned int CmmEnergySubBlock::ey(const int slice, const int source) const
{
  unsigned int ey = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    if (source >= 0 && source < s_maxJems) {
      ey = (m_sumsData[index(slice, source)] >> s_eyBit) & s_eyMask;
    } else if (source == REMOTE || source == LOCAL || source == TOTAL) {
      ey = m_sumsData[index(slice, source+1)] & s_sumsMask;
    }
  }
  return ey;
}

// Return Et subsum for given JEM or source ID

unsigned int CmmEnergySubBlock::et(const int slice, const int source) const
{
  unsigned int et = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    if (source >= 0 && source < s_maxJems) {
      et = (m_sumsData[index(slice, source)] >> s_etBit) & s_etMask;
    } else if (source == REMOTE || source == LOCAL || source == TOTAL) {
      et = m_sumsData[index(slice, source+2)] & s_sumsMask;
    }
  }
  return et;
}

// Return Ex subsum error for given JEM or source ID

int CmmEnergySubBlock::exError(const int slice, const int source) const
{
  int error = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    if (source >= 0 && source < s_maxJems) {
      error = (m_sumsData[index(slice, source)] >> s_jemErrorBit) & s_errorMask;
    } else if (source == REMOTE) {
      error = (m_sumsData[index(slice, source)] >> s_errorBit)
                                                         & s_remoteErrorMask;
    } else if (source == LOCAL || source == TOTAL) {
      error = (m_sumsData[index(slice, source)] >> s_errorBit) & s_errorMask;
    }
  }
  return error;
}

// Return Ey subsum error for given JEM or source ID

int CmmEnergySubBlock::eyError(const int slice, const int source) const
{
  int error = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    if (source >= 0 && source < s_maxJems) {
      error = (m_sumsData[index(slice, source)] >> s_jemErrorBit) & s_errorMask;
    } else if (source == REMOTE) {
      error = (m_sumsData[index(slice, source+1)] >> s_errorBit)
                                                           & s_remoteErrorMask;
    } else if (source == LOCAL || source == TOTAL) {
      error = (m_sumsData[index(slice, source+1)] >> s_errorBit) & s_errorMask;
    }
  }
  return error;
}

// Return Et subsum error for given JEM or source ID

int CmmEnergySubBlock::etError(const int slice, const int source) const
{
  int error = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    if (source >= 0 && source < s_maxJems) {
      error = (m_sumsData[index(slice, source)] >> s_jemErrorBit) & s_errorMask;
    } else if (source == REMOTE) {
      error = (m_sumsData[index(slice, source+2)] >> s_errorBit)
                                                           & s_remoteErrorMask;
    } else if (source == LOCAL || source == TOTAL) {
      error = (m_sumsData[index(slice, source+2)] >> s_errorBit) & s_errorMask;
    }
  }
  return error;
}

// Return Missing-ET Hits map

unsigned int CmmEnergySubBlock::missingEtHits(const int slice) const
{
  unsigned int map = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    map = (m_sumsData[index(slice, TOTAL+1)] >> s_etMissBit) & s_etMissMask;
  }
  return map;
}

// Return Sum-ET Hits map

unsigned int CmmEnergySubBlock::sumEtHits(const int slice) const
{
  unsigned int map = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    unsigned int mask = (version() > 1) ? s_etHitsMask : s_etHitsMaskV1;
    map = (m_sumsData[index(slice, TOTAL+2)] >> s_etHitsBit) & mask;
  }
  return map;
}

// Return Missing-ET-Sig Hits map

unsigned int CmmEnergySubBlock::missingEtSigHits(const int slice) const
{
  unsigned int map = 0;
  if (version() > 1 && slice >= 0 && slice < timeslices()
                                  && !m_sumsData.empty()) {
    map = (m_sumsData[index(slice, TOTAL)] >> s_etMissSigBit) & s_etMissSigMask;
  }
  return map;
}

// Store energy subsums and errors for given JEM or source ID

void CmmEnergySubBlock::setSubsums(const int slice, const int source,
                                   const unsigned int ex, const unsigned int ey,
				   const unsigned int et, const int exError,
				   const int eyError, const int etError)
{
  if (slice >= 0 && slice < timeslices() &&
      source >= 0 && source < s_maxSums  &&
      (ex || ey || et || exError || eyError || etError)) {
    resize();
    if (source < s_maxJems) {
      uint32_t word = 0;
      word |= (ex      & s_exMask)       << s_exBit;
      word |= (ey      & s_eyMask)       << s_eyBit;
      word |= (et      & s_etMask)       << s_etBit;
      word |= (exError & s_errorMask)    << s_jemErrorBit;
      word |= (eyError & s_errorMask)    << s_jemErrorBit;
      word |= (etError & s_errorMask)    << s_jemErrorBit;
      word |= (source  & s_sourceIdMask) << s_sourceIdBit;
      word |=  s_dataWordId              << s_dataWordIdBit;
      m_sumsData[index(slice, source)] = word;
    } else {
      unsigned int sum  = ex;
      int          err  = exError;
      uint32_t     mask = s_errorMask;
      if (source == REMOTE) mask = s_remoteErrorMask;
      for (int i = 0; i < 3; ++i) {
        if (sum || err) {
          uint32_t word = m_sumsData[index(slice, source+i)];
          word |=  sum        & s_sumsMask;
	  word |= (err        & mask)           << s_errorBit;
          word |= ((source+i) & s_sourceIdMask) << s_sourceIdBit;
          word |=  s_dataWordId                 << s_dataWordIdBit;
          m_sumsData[index(slice, source+i)] = word;
        }
	if (i == 0) {
	  sum = ey;
	  err = eyError;
        } else {
	  sum = et;
	  err = etError;
        }
      }
    }
  }
}

// Store Missing-ET Hits map

void CmmEnergySubBlock::setMissingEtHits(const int slice,
                                         const unsigned int map)
{
  if (slice >= 0 && slice < timeslices() && map) {
    resize();
    const int source = TOTAL + 1;
    uint32_t word = m_sumsData[index(slice, source)];
    word |= (map & s_etMissMask)      << s_etMissBit;
    word |= (source & s_sourceIdMask) << s_sourceIdBit;
    word |=  s_dataWordId             << s_dataWordIdBit;
    m_sumsData[index(slice, source)] = word;
  }
}

// Store Sum-Et Hits map

void CmmEnergySubBlock::setSumEtHits(const int slice, const unsigned int map)
{
  if (slice >= 0 && slice < timeslices() && map) {
    resize();
    const int source = TOTAL + 2;
    const unsigned int mask = (version() > 1) ? s_etHitsMask : s_etHitsMaskV1;
    uint32_t word = m_sumsData[index(slice, source)];
    word |= (map & mask)              << s_etHitsBit;
    word |= (source & s_sourceIdMask) << s_sourceIdBit;
    word |=  s_dataWordId             << s_dataWordIdBit;
    m_sumsData[index(slice, source)] = word;
  }
}

// Store Missing-ET-Sig Hits map

void CmmEnergySubBlock::setMissingEtSigHits(const int slice,
                                            const unsigned int map)
{
  if (version() > 1 && slice >= 0 && slice < timeslices() && map) {
    resize();
    const int source = TOTAL;
    uint32_t word = m_sumsData[index(slice, source)];
    word |= (map & s_etMissSigMask)   << s_etMissSigBit;
    word |= (source & s_sourceIdMask) << s_sourceIdBit;
    word |=  s_dataWordId             << s_dataWordIdBit;
    m_sumsData[index(slice, source)] = word;
  }
}

// Packing/Unpacking routines

bool CmmEnergySubBlock::pack()
{
  bool rc = false;
  switch (version()) {
    case 2:
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

bool CmmEnergySubBlock::unpack()
{
  bool rc = false;
  switch (version()) {
    case 1:
    case 2:
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

int CmmEnergySubBlock::index(const int slice, const int source) const
{
  int ix = source;
  if (format() == NEUTRAL) ix += slice * s_maxSums;
  return ix;
}

// Resize the sums vector according to format

void CmmEnergySubBlock::resize()
{
  if (m_sumsData.empty()) {
    int size = s_maxSums;
    if (format() == NEUTRAL) size *= timeslices();
    m_sumsData.resize(size);
  }
}

// Pack neutral data

bool CmmEnergySubBlock::packNeutral()
{
  resize();
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    for (int pin = 0; pin < s_maxJems; ++pin) {
      // JEM energy sums (jem == pin); parity error
      packerNeutral(pin, ex(slice, pin), s_jemSumBits);
      packerNeutral(pin, ey(slice, pin), s_jemSumBits);
      packerNeutral(pin, et(slice, pin), s_jemSumBits);
      packerNeutral(pin, etError(slice, pin), 1);
      // Bunch crossing number; Fifo overflow
      if (pin < s_bunchCrossingBits) {
        packerNeutral(pin, bunchCrossing() >> pin, 1);
        // Padding
        packerNeutral(pin, 0, s_paddingBits);
      } else if (pin == s_fifoOverflowPin) {
        packerNeutral(pin, daqOverflow(), 1);
      } else packerNeutral(pin, 0, 1);
    }
    int pin = s_bunchCrossingBits - 1;
    // Missing-ET-Sig Hits Map
    packerNeutral(pin, missingEtSigHits(slice), s_paddingBits);
    // Total Et
    packerNeutral(++pin, et(slice, TOTAL), s_paddingBits);
    packerNeutral(++pin, et(slice, TOTAL) >> s_paddingBits, s_paddingBits-1);
    packerNeutral(pin, etError(slice, TOTAL), 1);
    // Sum-Et Hits Map
    packerNeutral(++pin, sumEtHits(slice), s_paddingBits);
    // Missing-ET Hits Map
    packerNeutral(++pin, missingEtHits(slice), s_paddingBits);
    // Remote Ex, Ey
    packerNeutral(++pin, ex(slice, REMOTE), s_sumBits);
    packerNeutral(pin, exError(slice, REMOTE), 2);
    packerNeutral(pin, ey(slice, REMOTE), s_sumBits);
    packerNeutral(pin, eyError(slice, REMOTE), 2);
    // Local Ex, Ey
    packerNeutral(++pin, ex(slice, LOCAL), s_sumBits);
    packerNeutral(pin, exError(slice, LOCAL), 2);
    packerNeutral(pin, ey(slice, LOCAL), s_sumBits);
    packerNeutral(pin, eyError(slice, LOCAL), 2);
    // Total Ex, Ey
    packerNeutral(++pin, ex(slice, TOTAL), s_sumBits);
    packerNeutral(pin, exError(slice, TOTAL), 2);
    packerNeutral(pin, ey(slice, TOTAL), s_sumBits);
    packerNeutral(pin, eyError(slice, TOTAL), 2);
    // Remote and Local Et
    packerNeutral(++pin, et(slice, REMOTE), s_sumBits-1);
    packerNeutral(pin, etError(slice, REMOTE), 3);
    packerNeutral(pin, et(slice, LOCAL), s_sumBits-1);
    packerNeutral(pin, etError(slice, LOCAL), 3);
    // G-Link parity errors
    for (int p = 0; p <= pin; ++p) packerNeutralParity(p);
  }
  return true;
}

// Pack uncompressed data

bool CmmEnergySubBlock::packUncompressed()
{
  std::vector<uint32_t>::const_iterator pos;
  for (pos = m_sumsData.begin(); pos != m_sumsData.end(); ++pos) {
    if (*pos) packer(*pos, s_wordLength);
  }
  packerFlush();
  return true;
}

// Unpack neutral data

bool CmmEnergySubBlock::unpackNeutral()
{
  resize();
  int bunchCrossing = 0;
  int overflow = 0;
  int parity = 0;
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    for (int pin = 0; pin < s_maxJems; ++pin) {
      // JEM energy sums (jem == pin); parity error
      const unsigned int ex = unpackerNeutral(pin, s_jemSumBits);
      const unsigned int ey = unpackerNeutral(pin, s_jemSumBits);
      const unsigned int et = unpackerNeutral(pin, s_jemSumBits);
      const int er = unpackerNeutral(pin, 1);
      setSubsums(slice, pin, ex, ey, et, er, er, er);
      // Bunch crossing number; Fifo overflow
      if (pin < s_bunchCrossingBits) {
        bunchCrossing |= unpackerNeutral(pin, 1) << pin;
        // Padding
        unpackerNeutral(pin, s_paddingBits);
      } else if (pin == s_fifoOverflowPin) {
        overflow |= unpackerNeutral(pin, 1);
      } else unpackerNeutral(pin, 1);
    }
    int pin = s_bunchCrossingBits - 1;
    // Missing-ET-Sig Hits Map
    setMissingEtSigHits(slice, unpackerNeutral(pin, s_paddingBits));
    // Total Et
    unsigned int etTot = unpackerNeutral(++pin, s_paddingBits);
    etTot |= unpackerNeutral(++pin, s_paddingBits-1) << s_paddingBits;
    const int etErrTot = unpackerNeutral(pin, 1);
    // Sum-Et Hits Map + padding
    setSumEtHits(slice, unpackerNeutral(++pin, s_paddingBits) & s_etHitsMask);
    // Missing-ET Hits Map
    setMissingEtHits(slice, unpackerNeutral(++pin, s_paddingBits));
    // Remote Ex, Ey
    const unsigned int exRem = unpackerNeutral(++pin, s_sumBits);
    const int exErrRem = unpackerNeutral(pin, 2);
    const unsigned int eyRem = unpackerNeutral(pin, s_sumBits);
    const int eyErrRem = unpackerNeutral(pin, 2);
    // Local Ex, Ey
    const unsigned int exLoc = unpackerNeutral(++pin, s_sumBits);
    const int exErrLoc = unpackerNeutral(pin, 1);
    unpackerNeutral(pin, 1);
    const unsigned int eyLoc = unpackerNeutral(pin, s_sumBits);
    const int eyErrLoc = unpackerNeutral(pin, 1);
    unpackerNeutral(pin, 1);
    // Total Ex, Ey
    const unsigned int exTot = unpackerNeutral(++pin, s_sumBits);
    const int exErrTot = unpackerNeutral(pin, 1);
    unpackerNeutral(pin, 1);
    const unsigned int eyTot = unpackerNeutral(pin, s_sumBits);
    const int eyErrTot = unpackerNeutral(pin, 1);
    unpackerNeutral(pin, 1);
    // Remote and Local Et
    const unsigned int etRem = unpackerNeutral(++pin, s_sumBits-1);
    const int etErrRem = unpackerNeutral(pin, 2);
    unpackerNeutral(pin, 1);
    const unsigned int etLoc = unpackerNeutral(pin, s_sumBits-1);
    const int etErrLoc = unpackerNeutral(pin, 1);
    unpackerNeutral(pin, 2);
    setSubsums(slice, REMOTE, exRem, eyRem, etRem,
                              exErrRem, eyErrRem, etErrRem);
    setSubsums(slice, LOCAL,  exLoc, eyLoc, etLoc,
                              exErrLoc, eyErrLoc, etErrLoc);
    setSubsums(slice, TOTAL,  exTot, eyTot, etTot,
                              exErrTot, eyErrTot, etErrTot);
    // G-Link parity errors
    for (int p = 0; p <= pin; ++p) parity |= unpackerNeutralParityError(p);
  }
  setBunchCrossing(bunchCrossing);
  setDaqOverflow(overflow);
  setGlinkParity(parity);

  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
  return rc;
}

// Unpack uncompressed data

bool CmmEnergySubBlock::unpackUncompressed()
{
  resize();
  unpackerInit();
  uint32_t word = unpacker(s_wordLength);
  while (unpackerSuccess()) {
    const int source = sourceId(word);
    if (source < s_maxSums && m_sumsData[source] == 0) m_sumsData[source] = word;
    else {
      setUnpackErrorCode(UNPACK_SOURCE_ID);
      return false;
    }
    word = unpacker(s_wordLength);
  }
  return true;
}

} // end namespace
