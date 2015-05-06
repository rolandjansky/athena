/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CmxJetSubBlock.h"

namespace LVL1BS {

// Constant definitions

const int      CmxJetSubBlock::s_wordLength;

const int      CmxJetSubBlock::s_tobEnergyLgBit;
const int      CmxJetSubBlock::s_tobEnergySmBit;
const int      CmxJetSubBlock::s_tobErrorBit;
const int      CmxJetSubBlock::s_tobCoordBit;
const int      CmxJetSubBlock::s_tobFrameBit;
const int      CmxJetSubBlock::s_tobJemBit;
const int      CmxJetSubBlock::s_tobWordId;
const uint32_t CmxJetSubBlock::s_tobEnergyLgMask;
const uint32_t CmxJetSubBlock::s_tobEnergySmMask;
const uint32_t CmxJetSubBlock::s_tobErrorMask;
const uint32_t CmxJetSubBlock::s_tobCoordMask;
const uint32_t CmxJetSubBlock::s_tobFrameMask;
const uint32_t CmxJetSubBlock::s_tobJemMask;

const int      CmxJetSubBlock::s_threshBit;
const int      CmxJetSubBlock::s_threshErrorBit;
const int      CmxJetSubBlock::s_hlFlagBit;
const int      CmxJetSubBlock::s_sourceIdBit;
const int      CmxJetSubBlock::s_dataWordIdBit;
const int      CmxJetSubBlock::s_threshWordId;
const uint32_t CmxJetSubBlock::s_threshMainMask;
const uint32_t CmxJetSubBlock::s_threshFwdLMask;
const uint32_t CmxJetSubBlock::s_threshFwdHMask;
const uint32_t CmxJetSubBlock::s_errorMask;
const uint32_t CmxJetSubBlock::s_topoCheckMask;
const uint32_t CmxJetSubBlock::s_topoMapMask;
const uint32_t CmxJetSubBlock::s_topoCountsMask;
const uint32_t CmxJetSubBlock::s_hlFlagMask;
const uint32_t CmxJetSubBlock::s_sourceIdMask;
const uint32_t CmxJetSubBlock::s_dataWordIdMask;

const int      CmxJetSubBlock::s_presenceBits;
const int      CmxJetSubBlock::s_coordBits;
const int      CmxJetSubBlock::s_energyLgBits;
//const int      CmxJetSubBlock::s_energySmBits[8];
const int      CmxJetSubBlock::s_energySmBits[8] = {3, 6, 5, 4, 7, 2, 9, 0};
const int      CmxJetSubBlock::s_threshMainBits;
const int      CmxJetSubBlock::s_threshFwdLBits;
const int      CmxJetSubBlock::s_threshFwdHBits;
const int      CmxJetSubBlock::s_parityErrorBits;
const int      CmxJetSubBlock::s_roiOverflowBits;
const int      CmxJetSubBlock::s_paddingBits;
const int      CmxJetSubBlock::s_bunchCrossingBits;
const int      CmxJetSubBlock::s_fifoOverflowBits;
const int      CmxJetSubBlock::s_topoChecksumBits;
const int      CmxJetSubBlock::s_topoMapBits;
const int      CmxJetSubBlock::s_topoCountsBits;
const int      CmxJetSubBlock::s_topoPaddingBits;
const int      CmxJetSubBlock::s_glinkPins;
const int      CmxJetSubBlock::s_modules;
const int      CmxJetSubBlock::s_tobsPerModule;
const int      CmxJetSubBlock::s_muxPhases;


CmxJetSubBlock::CmxJetSubBlock()
{
}

CmxJetSubBlock::~CmxJetSubBlock()
{
}

// Clear all data

void CmxJetSubBlock::clear()
{
  L1CaloSubBlock::clear();
  m_tobData.clear();
  m_hitsData.clear();
  m_presenceMaps.clear();
  m_parityBits.clear();
}

// Return presence map for given JEM

unsigned int CmxJetSubBlock::presenceMap(const int slice,
    const int jem) const
{
  unsigned int map = 0;
  const unsigned int ix = mapIndex(slice, jem);
  if (ix < m_presenceMaps.size()) map = m_presenceMaps[ix];
  return map;
}

// Return frame for given jem and tob

int CmxJetSubBlock::frame(const int slice, const int jem, const int tob) const
{
  int fr = 0;
  const unsigned int ix = tobIndex(slice, jem, tob);
  if (ix < m_tobData.size()) {
    fr = (m_tobData[ix] >> s_tobFrameBit) & s_tobFrameMask;
  }
  return fr;
}

// Return Local coordinate for given jem and tob

int CmxJetSubBlock::localCoord(const int slice, const int jem, const int tob) const
{
  int coord = 0;
  const unsigned int ix = tobIndex(slice, jem, tob);
  if (ix < m_tobData.size()) {
    coord = (m_tobData[ix] >> s_tobCoordBit) & s_tobCoordMask;
  }
  return coord;
}

// Return energy large window size for given jem and tob

int CmxJetSubBlock::energyLarge(const int slice, const int jem, const int tob) const
{
  int et = 0;
  const unsigned int ix = tobIndex(slice, jem, tob);
  if (ix < m_tobData.size()) {
    et = (m_tobData[ix] >> s_tobEnergyLgBit) & s_tobEnergyLgMask;
  }
  return et;
}

// Return energy small window size for given jem and tob

int CmxJetSubBlock::energySmall(const int slice, const int jem, const int tob) const
{
  int et = 0;
  const unsigned int ix = tobIndex(slice, jem, tob);
  if (ix < m_tobData.size()) {
    et = (m_tobData[ix] >> s_tobEnergySmBit) & s_tobEnergySmMask;
  }
  return et;
}

// Return error bits for given jem and tob

int  CmxJetSubBlock::tobError(int slice, int jem, int tob) const
{
  int error = 0;
  const unsigned int ix = tobIndex(slice, jem, tob);
  if (ix < m_tobData.size()) {
    error = (m_tobData[ix] >> s_tobErrorBit) & s_tobErrorMask;
  }
  return error;
}

// Return parity bits for given JEM

int CmxJetSubBlock::parityBits(const int slice, const int jem) const
{
  int parity = 0;
  const unsigned int ix = parIndex(slice, jem);
  if (ix < m_parityBits.size()) parity = m_parityBits[ix];
  return parity;
}

// Return hit/topo counts for given source ID and HL flag

unsigned int CmxJetSubBlock::hits(const int slice, const int source,
                                  const int flag) const
{
  unsigned int hits = 0;
  const unsigned int ix = hitIndex(slice, source, flag);
  if (ix < m_hitsData.size()) {
    uint32_t mask = s_threshMainMask;
    if (source == REMOTE_FORWARD || source == LOCAL_FORWARD
        || source == TOTAL_FORWARD) {
      mask = (flag) ? s_threshFwdHMask : s_threshFwdLMask;
    }
    else if (source == TOPO_CHECKSUM)         mask = s_topoCheckMask;
    else if (source == TOPO_OCCUPANCY_MAP)    mask = s_topoMapMask;
    else if (source == TOPO_OCCUPANCY_COUNTS) mask = s_topoCountsMask;
    hits = (m_hitsData[ix] >> s_threshBit) & mask;
  }
  return hits;
}

// Return hit error for given source ID and HL flag

int CmxJetSubBlock::hitsError(const int slice, const int source,
                              const int flag) const
{
  int error = 0;
  if (source == REMOTE_MAIN || source == LOCAL_MAIN || source == TOTAL_MAIN ||
      source == REMOTE_FORWARD || source == LOCAL_FORWARD ||
      source == TOTAL_FORWARD) {
    const unsigned int ix = hitIndex(slice, source, flag);
    if (ix < m_hitsData.size()) {
      error = (m_hitsData[ix] >> s_threshErrorBit) & s_errorMask;
    }
  }
  return error;
}

// Store presence map

void CmxJetSubBlock::setPresenceMap(const int slice, const int jem,
                                    const unsigned int map)
{
  resize();
  if (map) {
    const unsigned int ix = mapIndex(slice, jem);
    if (ix < m_presenceMaps.size()) m_presenceMaps[ix] = map;
  }
}

// Store TOB (RoI) data for given JEM, frame, local coord

void CmxJetSubBlock::setTob(const int slice, const int jem, const int frame,
                            const int loc, const int energyLarge,
                            const int energySmall, const int error)
{
  resize();
  if (energyLarge || energySmall || error) {
    uint32_t word = 0;
    word |= (energyLarge & s_tobEnergyLgMask) << s_tobEnergyLgBit;
    word |= (energySmall & s_tobEnergySmMask) << s_tobEnergySmBit;
    word |= (error       & s_tobErrorMask)    << s_tobErrorBit;
    word |= (loc         & s_tobCoordMask)    << s_tobCoordBit;
    word |= (frame       & s_tobFrameMask)    << s_tobFrameBit;
    word |= (jem         & s_tobJemMask)      << s_tobJemBit;
    word |= (s_tobWordId)                     << s_dataWordIdBit;
    // Order by frame == presence bit                                          // <<== CHECK
    for (int tob = 0; tob < s_tobsPerModule; ++tob) {
      const unsigned int ix = tobIndex(slice, jem, tob);
      if (m_tobData[ix] == 0) {
        m_tobData[ix] = word;
        break;
      } else {
        const int frameOld = (m_tobData[ix] >> s_tobFrameBit)&s_tobFrameMask;
        if (frame < frameOld) {
          for (int i = s_tobsPerModule - tob - 1; i > 0; --i) {
            m_tobData[ix + i] = m_tobData[ix + i - 1];
          }
          m_tobData[ix] = word;
          break;
        }
      }
    }
  }
}

// Store parity bits for neutral format

void CmxJetSubBlock::setParityBits(const int slice, const int jem,
                                   const int parity)
{
  resize();
  if (parity) {
    const unsigned int ix = parIndex(slice, jem);
    if (ix < m_parityBits.size()) m_parityBits[ix] = parity;
  }
}

// Store hit counts for given source ID and HL flag

void CmxJetSubBlock::setHits(const int slice, const int source, const int flag,
                             const unsigned int hits, const int error)
{
  resize();
  const unsigned int ix = hitIndex(slice, source, flag);
  if (ix < m_hitsData.size() && (hits || error)) {
    uint32_t word = m_hitsData[ix];
    uint32_t mask = s_threshMainMask;
    if (source == REMOTE_FORWARD || source == LOCAL_FORWARD ||
        source == TOTAL_FORWARD) {
      mask = (flag) ? s_threshFwdHMask : s_threshFwdLMask;
    }
    else if (source == TOPO_CHECKSUM)         mask = s_topoCheckMask;
    else if (source == TOPO_OCCUPANCY_MAP)    mask = s_topoMapMask;
    else if (source == TOPO_OCCUPANCY_COUNTS) mask = s_topoCountsMask;
    word |= (hits & mask)             << s_threshBit;
    word |= (error & s_errorMask)     << s_threshErrorBit;
    word |= (flag & s_hlFlagMask)     << s_hlFlagBit;
    word |= (source & s_sourceIdMask) << s_sourceIdBit;
    word |= (s_threshWordId)          << s_dataWordIdBit;
    m_hitsData[ix] = word;
  }
}

// Packing/Unpacking routines

bool CmxJetSubBlock::pack()
{
  bool rc = false;
  switch (version()) {
  case 1:
  case 2:                                                  // <<== CHECK
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

bool CmxJetSubBlock::unpack()
{
  bool rc = false;
  switch (version()) {
  case 1:
  case 2:                                                  // <<== CHECK
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

// Return presence map index appropriate to format

unsigned int CmxJetSubBlock::mapIndex(const int slice,
                                      const int jem) const
{
  unsigned int ix = jem;
  if (format() == NEUTRAL) ix += slice * s_modules;
  return ix;
}

// Return parity bits index appropriate to format

unsigned int CmxJetSubBlock::parIndex(const int slice,
                                      const int jem) const
{
  unsigned int ix = jem;
  if (format() == NEUTRAL) ix += slice * s_modules;
  return ix;
}

// Return tob data index appropriate to format

unsigned int CmxJetSubBlock::tobIndex(const int slice, const int jem,
                                      const int tob) const
{
  unsigned int ix = jem * s_tobsPerModule + tob;
  if (format() == NEUTRAL) ix += slice * s_modules * s_tobsPerModule;
  return ix;
}

// Return hits data index appropriate to format

unsigned int CmxJetSubBlock::hitIndex(const int slice, const int source,
                                      const int flag) const
{
  unsigned int ix = (source << 1) | flag;
  if (format() == NEUTRAL) ix += slice * 2 * MAX_SOURCE_ID;
  return ix;
}

// Resize the data vectors according to format

void CmxJetSubBlock::resize()
{
  if (m_tobData.empty()) {
    int size1 = s_modules * s_tobsPerModule;
    int size2 = 2 * MAX_SOURCE_ID;
    int size3 = s_modules;
    int size4 = s_modules;
    if (format() == NEUTRAL) {
      size1 *= timeslices();
      size2 *= timeslices();
      size3 *= timeslices();
      size4 *= timeslices();
    }
    m_tobData.resize(size1);
    m_hitsData.resize(size2);
    m_presenceMaps.resize(size3);
    m_parityBits.resize(size4);
  }
}

// Pack neutral data

bool CmxJetSubBlock::packNeutral()
{
  resize();
  std::vector<int> locVec(s_tobsPerModule);
  std::vector<int> energyLgVec(s_tobsPerModule);
  std::vector<int> energySmVec(s_tobsPerModule);
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      if (pin < s_modules) { // TOB data
        const int jem = pin;
        // Presence map
        packerNeutral(pin, presenceMap(slice, jem), s_presenceBits);
        // Get tob data for this jem
        locVec.clear();
        energyLgVec.clear();
        energySmVec.clear();
        for (int tob = 0; tob < s_tobsPerModule; ++tob) {
          locVec.push_back(localCoord(slice, jem, tob));
          energyLgVec.push_back(energyLarge(slice, jem, tob));
          energySmVec.push_back(energySmall(slice, jem, tob));
        }
        const int parity = parityBits(slice, jem);
        // And pack
        for (int tob = 0; tob < s_tobsPerModule; ++tob) {
          // Energy small window LSB
          const int nbitsL = s_energySmBits[2 * tob];
          packerNeutral(pin, energySmVec[tob], nbitsL);
          // Local coordinates
          packerNeutral(pin, locVec[tob], s_coordBits);
          // Energy large window
          packerNeutral(pin, energyLgVec[tob], s_energyLgBits);
          // Backplane parity error
          packerNeutral(pin, (parity >> tob), s_parityErrorBits);
          // Energy small window HSB
          const int nbitsH = s_energySmBits[2 * tob + 1];
          if (nbitsH > 0) {
            packerNeutral(pin, (energySmVec[tob] >> nbitsL), nbitsH);
          }
        }
      } else { // Hits and Topo data
        if (pin < s_glinkPins - 1) {
          // Remote, local and total hits; parity error
          const int source1 = pin - s_modules + REMOTE_MAIN;
          const int source2 = pin - s_modules + REMOTE_FORWARD;
          packerNeutral(pin, hits(slice, source1, 0), s_threshMainBits);
          packerNeutral(pin, hitsError(slice, source1, 0), s_parityErrorBits);
          packerNeutral(pin, hits(slice, source1, 1), s_threshMainBits);
          packerNeutral(pin, (hitsError(slice, source1, 1) >> 1), s_parityErrorBits);
          packerNeutral(pin, hits(slice, source2, 0), s_threshFwdLBits);
          packerNeutral(pin, hitsError(slice, source2, 0), s_parityErrorBits);
          packerNeutral(pin, hits(slice, source2, 1), s_threshFwdHBits);
          packerNeutral(pin, (hitsError(slice, source2, 1) >> 1), s_parityErrorBits);
          packerNeutral(pin, (hitsError(slice, source1, 0) >> 2), s_roiOverflowBits);
          packerNeutral(pin, 0, s_paddingBits);
        } else {
          // Bunch crossing number, Fifo overflow and Topo data
          packerNeutral(pin, bunchCrossing(), s_bunchCrossingBits);
          packerNeutral(pin, daqOverflow(), s_fifoOverflowBits);
          packerNeutral(pin, hits(slice, TOPO_CHECKSUM, 0), s_topoChecksumBits);
          packerNeutral(pin, hits(slice, TOPO_OCCUPANCY_MAP, 0), s_topoMapBits);
          packerNeutral(pin, hits(slice, TOPO_OCCUPANCY_COUNTS, 0), s_topoCountsBits);
          packerNeutral(pin, hits(slice, TOPO_OCCUPANCY_COUNTS, 1), s_topoCountsBits);
          packerNeutral(pin, 0, s_topoPaddingBits);
        }
      }
      // G-Link parity
      packerNeutralParity(pin);
    }
  }
  return true;
}

// Pack uncompressed data

bool CmxJetSubBlock::packUncompressed()
{
  std::vector<uint32_t>::const_iterator pos;
  for (pos = m_tobData.begin(); pos != m_tobData.end(); ++pos) {
    if (*pos) packer(*pos, s_wordLength);
  }
  for (pos = m_hitsData.begin(); pos != m_hitsData.end(); ++pos) {
    if (*pos) packer(*pos, s_wordLength);
  }
  packerFlush();
  return true;
}

// Unpack neutral data

bool CmxJetSubBlock::unpackNeutral()
{
  resize();
  int bunchCrossing = 0;
  int fifoOverflow = 0;
  int glinkParity = 0;
  std::vector<int> locVec(s_tobsPerModule);
  std::vector<int> energyLgVec(s_tobsPerModule);
  std::vector<int> energySmVec(s_tobsPerModule);
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      if (pin < s_modules) { // TOB data
        // Presence map
        const unsigned int map = unpackerNeutral(pin, s_presenceBits);
        locVec.assign(s_tobsPerModule, 0);
        energyLgVec.assign(s_tobsPerModule, 0);
        energySmVec.assign(s_tobsPerModule, 0);
        int parity = 0;
        for (int tob = 0; tob < s_tobsPerModule; ++tob) {
          // Energy small window LSB
          const int nbitsL = s_energySmBits[2 * tob];
          energySmVec[tob] = unpackerNeutral(pin, nbitsL);
          // Local coordinates
          locVec[tob] = unpackerNeutral(pin, s_coordBits);
          // Energy large window
          energyLgVec[tob] = unpackerNeutral(pin, s_energyLgBits);
          // Backplane parity error
          parity |= (unpackerNeutral(pin, s_parityErrorBits) << tob);
          // Energy small window HSB
          const int nbitsH = s_energySmBits[2 * tob + 1];
          if (nbitsH > 0) {
            energySmVec[tob] += (unpackerNeutral(pin, nbitsH) << nbitsL);
          }
        }
        const int error = (parity) ? 1 : 0;
        int tob = 0;
        const int jem = pin;
        for (int frame = 0; frame < s_presenceBits && tob < s_tobsPerModule; ++frame) {  // <<== CHECK - assuming bit==frame
          if ((map >> frame) & 1) {
            setTob(slice, jem, frame, locVec[tob], energyLgVec[tob],
                   energySmVec[tob], error);
            ++tob;
          }
        }
        setPresenceMap(slice, jem, map);
        setParityBits(slice, jem, parity);
      } else {  // Hits and Topo data
        if (pin < s_glinkPins - 1) {
          // Remote, local and total hits; parity error, RoI overflow
          const int source1 = pin - s_modules + REMOTE_MAIN;
          const int source2 = pin - s_modules + REMOTE_FORWARD;
          const unsigned int main0 = unpackerNeutral(pin, s_threshMainBits);
          int errorMain = unpackerNeutral(pin, s_parityErrorBits);
          const unsigned int main1 = unpackerNeutral(pin, s_threshMainBits);
          errorMain |= (unpackerNeutral(pin, s_parityErrorBits) << 1);
          const unsigned int fwd0 = unpackerNeutral(pin, s_threshFwdLBits);
          int errorFwd = unpackerNeutral(pin, s_parityErrorBits);
          const unsigned int fwd1 = unpackerNeutral(pin, s_threshFwdHBits);
          errorFwd |= (unpackerNeutral(pin, s_parityErrorBits) << 1);
          const int overflow = (unpackerNeutral(pin, s_roiOverflowBits) << 2);
          errorMain |= overflow;
          errorFwd  |= overflow;
          setHits(slice, source1, 0, main0, errorMain);
          setHits(slice, source1, 1, main1, errorMain);
          setHits(slice, source2, 0, fwd0,  errorFwd);
          setHits(slice, source2, 1, fwd1,  errorFwd);
          unpackerNeutral(pin, s_paddingBits);
        } else {
          // Bunch crossing number, Fifo overflow and Topo data
          bunchCrossing = unpackerNeutral(pin, s_bunchCrossingBits);
          fifoOverflow |= unpackerNeutral(pin, s_fifoOverflowBits);
          unsigned int hits = unpackerNeutral(pin, s_topoChecksumBits);
          int error = 0;
          setHits(slice, TOPO_CHECKSUM, 0, hits, error);
          hits = unpackerNeutral(pin, s_topoMapBits);
          setHits(slice, TOPO_OCCUPANCY_MAP, 0, hits, error);
          hits = unpackerNeutral(pin, s_topoCountsBits);
          setHits(slice, TOPO_OCCUPANCY_COUNTS, 0, hits, error);
          hits = unpackerNeutral(pin, s_topoCountsBits);
          setHits(slice, TOPO_OCCUPANCY_COUNTS, 1, hits, error);
          unpackerNeutral(pin, s_topoPaddingBits);
        }
      }
      // G-Link parity errors
      glinkParity |= unpackerNeutralParityError(pin);
    }
  }
  setBunchCrossing(bunchCrossing);
  setDaqOverflow(fifoOverflow);
  setGlinkParity(glinkParity);

  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
  return rc;
}

// Unpack uncompressed data

bool CmxJetSubBlock::unpackUncompressed()
{
  resize();
  const int maxHits = m_hitsData.size();
  m_jemTobCount.assign(s_modules, 0);
  unpackerInit();
  uint32_t word = unpacker(s_wordLength);
  while (unpackerSuccess()) {
    const int id = dataWordId(word);
    if (id == s_tobWordId) {  // TOB data
      const int index = jem(word);
      const int count = m_jemTobCount[index];
      const int index2 = index * s_tobsPerModule + count;
      if (count < s_tobsPerModule) {
        m_tobData[index2] = word;
        ++m_jemTobCount[index];
      } else {
        setUnpackErrorCode(UNPACK_EXCESS_TOBS);             // New code.  Check consequences
        return false;
      }
    } else if (id == s_threshWordId) {  // Hits and Topo data
      const int index = (sourceId(word) << 1) | hlFlag(word);
      if (index < maxHits && m_hitsData[index] == 0) {
        m_hitsData[index] = word;
      } else {
        setUnpackErrorCode(UNPACK_SOURCE_ID);
        return false;
      }
    } else {
      setUnpackErrorCode(UNPACK_DATA_ID);                // New code
      return false;
    }
    word = unpacker(s_wordLength);
  }
  return true;
}

} // end namespace
