/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "CmxEnergySubBlock.h"

namespace LVL1BS {

// Constant definitions

const int      CmxEnergySubBlock::s_wordLength;

const int      CmxEnergySubBlock::s_overflowBit;
const int      CmxEnergySubBlock::s_errorBit;
const int      CmxEnergySubBlock::s_etHitsBit;
const int      CmxEnergySubBlock::s_energyTypeJemBit;
const int      CmxEnergySubBlock::s_jemBit;
const int      CmxEnergySubBlock::s_energyTypeBit;
const int      CmxEnergySubBlock::s_sumTypeBit;
const int      CmxEnergySubBlock::s_sourceBit;
const int      CmxEnergySubBlock::s_wordIdBit;
const int      CmxEnergySubBlock::s_maxJems;
const int      CmxEnergySubBlock::s_maxSums;
const uint32_t CmxEnergySubBlock::s_energyJemMask;
const uint32_t CmxEnergySubBlock::s_energySumMask;
const uint32_t CmxEnergySubBlock::s_errorMask;
const uint32_t CmxEnergySubBlock::s_overflowMask;
const uint32_t CmxEnergySubBlock::s_etHitsMask;
const uint32_t CmxEnergySubBlock::s_jemMask;
const uint32_t CmxEnergySubBlock::s_energyTypeMask;
const uint32_t CmxEnergySubBlock::s_sumTypeMask;
const uint32_t CmxEnergySubBlock::s_sourceMask;
const uint32_t CmxEnergySubBlock::s_wordIdMask;

const int      CmxEnergySubBlock::s_jemSumBits;
const int      CmxEnergySubBlock::s_jemPaddingBits;
const int      CmxEnergySubBlock::s_sumBitsEtCrate;
const int      CmxEnergySubBlock::s_sumBitsEtSys;
const int      CmxEnergySubBlock::s_sumBitsExEy;
const int      CmxEnergySubBlock::s_bunchCrossingBits;
const int      CmxEnergySubBlock::s_etHitMapsBits;
const int      CmxEnergySubBlock::s_paddingBits;


CmxEnergySubBlock::CmxEnergySubBlock()
{
}

CmxEnergySubBlock::~CmxEnergySubBlock()
{
}

// Clear all data

void CmxEnergySubBlock::clear()
{
  L1CaloSubBlock::clear();
  m_sumsData.clear();
}

// Return energy subsum for given JEM and energy type

unsigned int CmxEnergySubBlock::energy(const int slice, const int jem,
                                       const EnergyType eType) const
{
  unsigned int e = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    if (jem >= 0 && jem < s_maxJems) {
      e = m_sumsData[index(slice, jem) + eType] & s_energyJemMask;
    }
  }
  return e;
}

// Return energy subsum error for given JEM and energy type

int CmxEnergySubBlock::error(const int slice, const int jem,
                             const EnergyType eType) const
{
  int parity = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    if (jem >= 0 && jem < s_maxJems) {
      parity = (m_sumsData[index(slice, jem) + eType] >> s_errorBit)
               & s_errorMask;
    }
  }
  return parity << 1;
}

// Return energy subsum for given source, sum type and energy type

unsigned int CmxEnergySubBlock::energy(const int slice,
                                       const SourceType source,
                                       const SumType    sType,
                                       const EnergyType eType) const
{
  unsigned int e = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    const int pos = s_maxJems + 2 * source + sType;
    e = m_sumsData[index(slice, pos) + eType] & s_energySumMask;
  }
  return e;
}

// Return energy subsum error for given source, sum type and energy type

int CmxEnergySubBlock::error(const int slice,
                             const SourceType source,
                             const SumType    sType,
                             const EnergyType eType) const
{
  int parity = 0;
  int overflow = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    const int pos = s_maxJems + 2 * source + sType;
    const uint32_t word = m_sumsData[index(slice, pos) + eType];
    overflow = (word >> s_overflowBit) & s_overflowMask;
    if (source == REMOTE) parity = (word >> s_errorBit) & s_errorMask;
  }
  return (parity << 1) + overflow;
}

// Return hits map for given hits type and sum type

unsigned int CmxEnergySubBlock::hits(const int slice,
                                     const HitsType hType,
                                     const SumType  sType) const
{
  unsigned int map = 0;
  if (slice >= 0 && slice < timeslices() && !m_sumsData.empty()) {
    const int pos = s_maxJems + 2 * TOTAL + sType;
    map = (m_sumsData[index(slice, pos) + hType] >> s_etHitsBit)
          & s_etHitsMask;
  }
  return map;
}

// Store energy subsums and errors for given JEM

void CmxEnergySubBlock::setSubsums(const int slice, const int jem,
                                   const unsigned int ex, const unsigned int ey,
                                   const unsigned int et, const int exError,
                                   const int eyError, const int etError)
{
  if (slice >= 0 && slice < timeslices() && jem >= 0 && jem < s_maxJems) {
    resize();
    const int ix = index(slice, jem);
    unsigned int energy = 0;
    int          error  = 0;
    int          parity = 0;
    uint32_t     word   = 0;
    for (int eType = 0; eType < MAX_ENERGY_TYPE; ++eType) {
      if (eType == ENERGY_EX) {
        energy = ex;
        error  = exError;
      } else if (eType == ENERGY_EY) {
        energy = ey;
        error  = eyError;
      } else {
        energy = et;
        error = etError;
      }
      parity = (error >> 1) & s_errorMask;
      if (energy || parity) {
        word  = energy & s_energyJemMask;
        word |= parity    << s_errorBit;
        word |= eType     << s_energyTypeJemBit;
        word |= jem       << s_jemBit;
        word |= MODULE_ID << s_wordIdBit;
        m_sumsData[ix + eType] = word;
      }
    }
  }
}

// Store energy subsums and errors for given source and sum type

void CmxEnergySubBlock::setSubsums(const int slice, const SourceType source,
                                   const SumType sType,
                                   const unsigned int ex, const unsigned int ey,
                                   const unsigned int et, const int exError,
                                   const int eyError, const int etError)
{
  if (slice >= 0 && slice < timeslices()) {
    resize();
    const int pos = s_maxJems + 2 * source + sType;
    const int ix = index(slice, pos);
    unsigned int energy   = 0;
    int          error    = 0;
    int          overflow = 0;
    int          parity   = 0;
    uint32_t     word     = 0;
    uint32_t     baseword = (CRATE_SYSTEM_ID << s_wordIdBit) +
                            (source          << s_sourceBit) +
                            (sType           << s_sumTypeBit);
    for (int eType = 0; eType < MAX_ENERGY_TYPE; ++eType) {
      if (eType == ENERGY_EX) {
        energy = ex;
        error  = exError;
      } else if (eType == ENERGY_EY) {
        energy = ey;
        error  = eyError;
      } else {
        energy = et;
        error  = etError;
      }
      overflow = error & s_overflowMask;
      parity   = (source == REMOTE) ? ((error >> 1) & s_errorMask) : 0;
      if (energy || overflow || parity) {
        word  = m_sumsData[ix + eType];
        word |= energy    & s_energySumMask;
        word |= overflow << s_overflowBit;
        word |= parity   << s_errorBit;
        word |= eType    << s_energyTypeBit;
        word |= baseword;
        m_sumsData[ix + eType] = word;
      }
    }
  }
}

// Store hits map for given hits type and sum type

void CmxEnergySubBlock::setEtHits(const int slice, const HitsType hType,
                                  const SumType sType, const unsigned int map)
{
  if (map && slice >= 0 && slice < timeslices()) {
    resize();
    const int pos = s_maxJems + 2 * TOTAL + sType;
    const int ix  = index(slice, pos);
    uint32_t word = m_sumsData[ix + hType];
    word |= (map & s_etHitsMask) << s_etHitsBit;
    word |=  hType               << s_energyTypeBit;
    word |=  sType               << s_sumTypeBit;
    word |=  TOTAL               << s_sourceBit;
    word |=  CRATE_SYSTEM_ID     << s_wordIdBit;
    m_sumsData[ix + hType] = word;
  }
}

// Packing/Unpacking routines

bool CmxEnergySubBlock::pack()
{
  bool rc = false;
  switch (version()) {
  case 3:                                                      //<<== CHECK
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

bool CmxEnergySubBlock::unpack()
{
  bool rc = false;

  switch (version()) {
  case 1:                                                      //<<== CHECK
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

int CmxEnergySubBlock::index(const int slice, const int pos) const
{
  int ix = 3 * pos;
  if (format() == NEUTRAL) ix += slice * s_maxSums;
  return ix;
}

// Resize the sums vector according to format

void CmxEnergySubBlock::resize()
{
  if (m_sumsData.empty()) {
    int size = s_maxSums;
    if (format() == NEUTRAL) size *= timeslices();
    m_sumsData.resize(size);
  }
}

// Pack neutral data

bool CmxEnergySubBlock::packNeutral()
{
  resize();
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    for (int pin = 0; pin < s_maxJems; ++pin) {
      // JEM energy sums (jem == pin); parity errors
      packerNeutral(pin, energy(slice, pin, ENERGY_EX), s_jemSumBits);
      packerNeutral(pin, 0, s_jemPaddingBits);
      packerNeutral(pin, (error(slice, pin, ENERGY_EX) >> 1), 1);
      packerNeutral(pin, energy(slice, pin, ENERGY_EY), s_jemSumBits);
      packerNeutral(pin, 0, s_jemPaddingBits);
      packerNeutral(pin, (error(slice, pin, ENERGY_EY) >> 1), 1);
      packerNeutral(pin, energy(slice, pin, ENERGY_ET), s_jemSumBits);
      packerNeutral(pin, 0, s_jemPaddingBits);
      packerNeutral(pin, (error(slice, pin, ENERGY_ET) >> 1), 1);
      packerNeutral(pin, 0, s_jemSumBits);
      packerNeutral(pin, 0, s_jemPaddingBits);
      packerNeutral(pin, 0, 1);
    }
    // Remote Ex, Ey, Et
    int pin = s_maxJems;
    packerNeutral(pin, energy(slice, REMOTE, STANDARD, ENERGY_EX),
                  s_sumBitsExEy);
    packerNeutral(pin, (error(slice, REMOTE, STANDARD, ENERGY_EX) >> 1), 1);
    packerNeutral(pin, energy(slice, REMOTE, RESTRICTED_WEIGHTED, ENERGY_EX),
                  s_sumBitsExEy);
    packerNeutral(pin, (error(slice, REMOTE, RESTRICTED_WEIGHTED,
                              ENERGY_EX) >> 1), 1);
    packerNeutral(pin, (error(slice, REMOTE, STANDARD, ENERGY_EX) & 0x1), 1); // Seems inconsistent with SLink?
    packerNeutral(pin, energy(slice, REMOTE, STANDARD, ENERGY_EY),
                  s_sumBitsExEy);
    packerNeutral(pin, (error(slice, REMOTE, STANDARD, ENERGY_EY) >> 1), 1);
    packerNeutral(pin, energy(slice, REMOTE, RESTRICTED_WEIGHTED, ENERGY_EY),
                  s_sumBitsExEy);
    packerNeutral(pin, (error(slice, REMOTE, RESTRICTED_WEIGHTED,
                              ENERGY_EY) >> 1), 1);
    packerNeutral(pin, (error(slice, REMOTE, STANDARD, ENERGY_EY) & 0x1), 1);
    packerNeutral(pin, energy(slice, REMOTE, STANDARD, ENERGY_ET),
                  s_sumBitsEtCrate);
    packerNeutral(pin, 0, 1);
    packerNeutral(pin, energy(slice, REMOTE, RESTRICTED_WEIGHTED, ENERGY_ET),
                  s_sumBitsEtCrate);
    packerNeutral(pin, (error(slice, REMOTE, STANDARD, ENERGY_ET) & 0x1), 1);
    // Local Ex, Ey, Et
    ++pin;
    packerNeutral(pin, energy(slice, LOCAL, STANDARD, ENERGY_EX),
                  s_sumBitsExEy);
    packerNeutral(pin, 0, 1);
    packerNeutral(pin, energy(slice, LOCAL, RESTRICTED_WEIGHTED, ENERGY_EX),
                  s_sumBitsExEy);
    packerNeutral(pin, 0, 1);
    packerNeutral(pin, (error(slice, LOCAL, STANDARD, ENERGY_EX) & 0x1), 1);
    packerNeutral(pin, energy(slice, LOCAL, STANDARD, ENERGY_EY),
                  s_sumBitsExEy);
    packerNeutral(pin, 0, 1);
    packerNeutral(pin, energy(slice, LOCAL, RESTRICTED_WEIGHTED, ENERGY_EY),
                  s_sumBitsExEy);
    packerNeutral(pin, 0, 1);
    packerNeutral(pin, (error(slice, LOCAL, STANDARD, ENERGY_EY) & 0x1), 1);
    packerNeutral(pin, energy(slice, LOCAL, STANDARD, ENERGY_ET),
                  s_sumBitsEtCrate);
    packerNeutral(pin, 0, 1);
    packerNeutral(pin, energy(slice, LOCAL, RESTRICTED_WEIGHTED, ENERGY_ET),
                  s_sumBitsEtCrate);
    packerNeutral(pin, (error(slice, LOCAL, STANDARD, ENERGY_ET) & 0x1), 1);
    // Total Ex, Ey, Et
    ++pin;
    packerNeutral(pin, energy(slice, TOTAL, STANDARD, ENERGY_EX),
                  s_sumBitsExEy);
    packerNeutral(pin, 0, 1);
    packerNeutral(pin, energy(slice, TOTAL, RESTRICTED_WEIGHTED, ENERGY_EX),
                  s_sumBitsExEy);
    packerNeutral(pin, 0, 1);
    packerNeutral(pin, (error(slice, TOTAL, STANDARD, ENERGY_EX) & 0x1), 1);
    packerNeutral(pin, energy(slice, TOTAL, STANDARD, ENERGY_EY),
                  s_sumBitsExEy);
    packerNeutral(pin, 0, 1);
    packerNeutral(pin, energy(slice, TOTAL, RESTRICTED_WEIGHTED, ENERGY_EY),
                  s_sumBitsExEy);
    packerNeutral(pin, (error(slice, TOTAL, STANDARD, ENERGY_ET) & 0x1), 1);
    packerNeutral(pin, (error(slice, TOTAL, STANDARD, ENERGY_EY) & 0x1), 1);
    packerNeutral(pin, energy(slice, TOTAL, STANDARD, ENERGY_ET),
                  s_sumBitsEtSys);
    packerNeutral(pin, energy(slice, TOTAL, RESTRICTED_WEIGHTED, ENERGY_ET),
                  s_sumBitsEtSys);
    // Bunchcrossing number, Fifo overflow
    ++pin;
    packerNeutral(pin, bunchCrossing(), s_bunchCrossingBits);
    packerNeutral(pin, daqOverflow(), 1);
    // Et hit maps
    packerNeutral(pin, hits(slice, SUM_ET, STANDARD), s_etHitMapsBits);
    packerNeutral(pin, hits(slice, MISSING_ET, STANDARD), s_etHitMapsBits);
    packerNeutral(pin, hits(slice, MISSING_ET_SIG, STANDARD), s_etHitMapsBits);
    packerNeutral(pin, hits(slice, SUM_ET, RESTRICTED_WEIGHTED),
                  s_etHitMapsBits);
    packerNeutral(pin, hits(slice, MISSING_ET, RESTRICTED_WEIGHTED),
                  s_etHitMapsBits);
    packerNeutral(pin, 0, s_paddingBits);
    // G-Link parity errors
    for (int p = 0; p <= pin; ++p) packerNeutralParity(p);
  }
  return true;
}

// Pack uncompressed data

bool CmxEnergySubBlock::packUncompressed()
{
  std::vector<uint32_t>::const_iterator pos;
  for (pos = m_sumsData.begin(); pos != m_sumsData.end(); ++pos) {
    if (*pos) packer(*pos, s_wordLength);
  }
  packerFlush();
  return true;
}

// Unpack neutral data

bool CmxEnergySubBlock::unpackNeutral()
{
  resize();
  int bunchCrossing = 0;            // BunchCrossing number
  int overflow = 0;                 // FIFO overflow
  int parity = 0;                   // GLink parity
  unsigned int en[MAX_ENERGY_TYPE]; // Energies standard
  unsigned int rn[MAX_ENERGY_TYPE]; // Energies restricted/weighted
  int er[MAX_ENERGY_TYPE];          // Errors standard (bit 0 overflow, bit 1 parity)
  int rr[MAX_ENERGY_TYPE];          // Errors restricted/weighted
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    for (int pin = 0; pin < s_maxJems; ++pin) {
      // JEM energy sums (jem == pin); parity errors
      for (int eType = 0; eType < MAX_ENERGY_TYPE; ++eType) {
        en[eType] = unpackerNeutral(pin, s_jemSumBits);
        unpackerNeutral(pin, s_jemPaddingBits);
        er[eType] = unpackerNeutral(pin, 1) << 1;
      }
      unpackerNeutral(pin, s_jemSumBits);
      unpackerNeutral(pin, s_jemPaddingBits);
      unpackerNeutral(pin, 1);
      setSubsums(slice, pin, en[ENERGY_EX], en[ENERGY_EY], en[ENERGY_ET],
                 er[ENERGY_EX], er[ENERGY_EY], er[ENERGY_ET]);
    }
    // Remote Ex, Ey, Et, parity, overflow
    int pin = s_maxJems;
    en[ENERGY_EX]  = unpackerNeutral(pin, s_sumBitsExEy);
    er[ENERGY_EX]  = unpackerNeutral(pin, 1) << 1;
    er[ENERGY_ET]  = er[ENERGY_EX];
    rn[ENERGY_EX]  = unpackerNeutral(pin, s_sumBitsExEy);
    rr[ENERGY_EX]  = unpackerNeutral(pin, 1) << 1;
    rr[ENERGY_ET]  = rr[ENERGY_EX];
    er[ENERGY_EX] |= unpackerNeutral(pin, 1);                 // Or is it both?
    en[ENERGY_EY]  = unpackerNeutral(pin, s_sumBitsExEy);
    er[ENERGY_EY]  = unpackerNeutral(pin, 1) << 1;
    er[ENERGY_ET] |= er[ENERGY_EY];
    rn[ENERGY_EY]  = unpackerNeutral(pin, s_sumBitsExEy);
    rr[ENERGY_EY]  = unpackerNeutral(pin, 1) << 1;
    rr[ENERGY_ET] |= rr[ENERGY_EY];
    er[ENERGY_EY] |= unpackerNeutral(pin, 1);
    en[ENERGY_ET]  = unpackerNeutral(pin, s_sumBitsEtCrate);
    unpackerNeutral(pin, 1);
    rn[ENERGY_ET]  = unpackerNeutral(pin, s_sumBitsEtCrate);
    er[ENERGY_ET] |= unpackerNeutral(pin, 1);
    setSubsums(slice, REMOTE, STANDARD,
               en[ENERGY_EX], en[ENERGY_EY], en[ENERGY_ET],
               er[ENERGY_EX], er[ENERGY_EY], er[ENERGY_ET]);
    setSubsums(slice, REMOTE, RESTRICTED_WEIGHTED,
               rn[ENERGY_EX], rn[ENERGY_EY], rn[ENERGY_ET],
               rr[ENERGY_EX], rr[ENERGY_EY], rr[ENERGY_ET]);
    // Local Ex, Ey, Et, overflow
    ++pin;
    en[ENERGY_EX] = unpackerNeutral(pin, s_sumBitsExEy);
    unpackerNeutral(pin, 1);
    rn[ENERGY_EX] = unpackerNeutral(pin, s_sumBitsExEy);
    unpackerNeutral(pin, 1);
    er[ENERGY_EX] = unpackerNeutral(pin, 1);                 // Or is it both?
    rr[ENERGY_EX] = 0;
    en[ENERGY_EY] = unpackerNeutral(pin, s_sumBitsExEy);
    unpackerNeutral(pin, 1);
    rn[ENERGY_EY] = unpackerNeutral(pin, s_sumBitsExEy);
    unpackerNeutral(pin, 1);
    er[ENERGY_EY] = unpackerNeutral(pin, 1);
    rr[ENERGY_EY] = 0;
    en[ENERGY_ET] = unpackerNeutral(pin, s_sumBitsEtCrate);
    unpackerNeutral(pin, 1);
    rn[ENERGY_ET] = unpackerNeutral(pin, s_sumBitsEtCrate);
    er[ENERGY_ET] = unpackerNeutral(pin, 1);
    rr[ENERGY_ET] = 0;
    setSubsums(slice, LOCAL, STANDARD,
               en[ENERGY_EX], en[ENERGY_EY], en[ENERGY_ET],
               er[ENERGY_EX], er[ENERGY_EY], er[ENERGY_ET]);
    setSubsums(slice, LOCAL, RESTRICTED_WEIGHTED,
               rn[ENERGY_EX], rn[ENERGY_EY], rn[ENERGY_ET],
               rr[ENERGY_EX], rr[ENERGY_EY], rr[ENERGY_ET]);
    // Total Ex, Ey, Et, overflow
    ++pin;
    en[ENERGY_EX] = unpackerNeutral(pin, s_sumBitsExEy);
    unpackerNeutral(pin, 1);
    rn[ENERGY_EX] = unpackerNeutral(pin, s_sumBitsExEy);
    unpackerNeutral(pin, 1);
    er[ENERGY_EX] = unpackerNeutral(pin, 1);                 // Or is it both?
    rr[ENERGY_EX] = 0;
    en[ENERGY_EY] = unpackerNeutral(pin, s_sumBitsExEy);
    unpackerNeutral(pin, 1);
    rn[ENERGY_EY] = unpackerNeutral(pin, s_sumBitsExEy);
    er[ENERGY_ET] = unpackerNeutral(pin, 1);
    rr[ENERGY_ET] = 0;
    er[ENERGY_EY] = unpackerNeutral(pin, 1);
    rr[ENERGY_EY] = 0;
    en[ENERGY_ET] = unpackerNeutral(pin, s_sumBitsEtSys);
    rn[ENERGY_ET] = unpackerNeutral(pin, s_sumBitsEtSys);
    setSubsums(slice, TOTAL, STANDARD,
               en[ENERGY_EX], en[ENERGY_EY], en[ENERGY_ET],
               er[ENERGY_EX], er[ENERGY_EY], er[ENERGY_ET]);
    setSubsums(slice, TOTAL, RESTRICTED_WEIGHTED,
               rn[ENERGY_EX], rn[ENERGY_EY], rn[ENERGY_ET],
               rr[ENERGY_EX], rr[ENERGY_EY], rr[ENERGY_ET]);
    // Bunchcrossing number, Fifo overflow
    ++pin;
    bunchCrossing = unpackerNeutral(pin, s_bunchCrossingBits);
    overflow = unpackerNeutral(pin, 1);
    // Et hit maps
    setEtHits(slice, SUM_ET, STANDARD,
              unpackerNeutral(pin, s_etHitMapsBits));
    setEtHits(slice, MISSING_ET, STANDARD,
              unpackerNeutral(pin, s_etHitMapsBits));
    setEtHits(slice, MISSING_ET_SIG, STANDARD,
              unpackerNeutral(pin, s_etHitMapsBits));
    setEtHits(slice, SUM_ET, RESTRICTED_WEIGHTED,
              unpackerNeutral(pin, s_etHitMapsBits));
    setEtHits(slice, MISSING_ET, RESTRICTED_WEIGHTED,
              unpackerNeutral(pin, s_etHitMapsBits));
    unpackerNeutral(pin, s_paddingBits);
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

bool CmxEnergySubBlock::unpackUncompressed()
{
  resize();
  unpackerInit();
  bool error = false;
  uint32_t word = unpacker(s_wordLength);
  while (unpackerSuccess()) {
    if (word) {
      const int wordId = (word >> s_wordIdBit) & s_wordIdMask;
      if (wordId == MODULE_ID) {
        const int jem   = (word >> s_jemBit)           & s_jemMask;
        const int eType = (word >> s_energyTypeJemBit) & s_energyTypeMask;
        const int pos = 3 * jem + eType;
        if (eType < MAX_ENERGY_TYPE && m_sumsData[pos] == 0) {
          m_sumsData[pos] = word;
        } else error = true;
      } else if (wordId == CRATE_SYSTEM_ID) {
        const int source = (word >> s_sourceBit)     & s_sourceMask;
        const int sType  = (word >> s_sumTypeBit)    & s_sumTypeMask;
        const int eType  = (word >> s_energyTypeBit) & s_energyTypeMask;
        const int pos = 3 * (s_maxJems + 2 * source + sType) + eType;
        if (source < MAX_SOURCE_TYPE && eType < MAX_ENERGY_TYPE
            && m_sumsData[pos] == 0) {
          m_sumsData[pos] = word;
        } else error = true;
      } else error = true;
      if (error) {
        setUnpackErrorCode(UNPACK_SOURCE_ID);
        return false;
      }
    }
    word = unpacker(s_wordLength);
  }
  return true;
}

} // end namespace
