/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CmxCpSubBlock.h"

namespace LVL1BS {

// Constant definitions

const int      CmxCpSubBlock::s_wordLength;

const int      CmxCpSubBlock::s_tobEnergyBit;
const int      CmxCpSubBlock::s_tobIsolationBit;
const int      CmxCpSubBlock::s_tobErrorBit;
const int      CmxCpSubBlock::s_tobOverflowBit;
const int      CmxCpSubBlock::s_tobCoordBit;
const int      CmxCpSubBlock::s_tobChipBit;
const int      CmxCpSubBlock::s_tobCpmBit;
const int      CmxCpSubBlock::s_tobWordId;
const uint32_t CmxCpSubBlock::s_tobEnergyMask;
const uint32_t CmxCpSubBlock::s_tobIsolationMask;
const uint32_t CmxCpSubBlock::s_tobErrorMask;
const uint32_t CmxCpSubBlock::s_tobCoordMask;
const uint32_t CmxCpSubBlock::s_tobChipMask;
const uint32_t CmxCpSubBlock::s_tobCpmMask;

const int      CmxCpSubBlock::s_threshBit;
const int      CmxCpSubBlock::s_threshErrorBit;
const int      CmxCpSubBlock::s_hlFlagBit;
const int      CmxCpSubBlock::s_sourceIdBit;
const int      CmxCpSubBlock::s_dataWordIdBit;
const int      CmxCpSubBlock::s_threshWordId;
const uint32_t CmxCpSubBlock::s_threshMask;
const uint32_t CmxCpSubBlock::s_errorMask;
const uint32_t CmxCpSubBlock::s_hlFlagMask;
const uint32_t CmxCpSubBlock::s_sourceIdMask;
const uint32_t CmxCpSubBlock::s_dataWordIdMask;

const int      CmxCpSubBlock::s_presenceBits;
const int      CmxCpSubBlock::s_coordBits;
const int      CmxCpSubBlock::s_isolationBits;
const int      CmxCpSubBlock::s_parityErrorBits;
const int      CmxCpSubBlock::s_parityErrorMask;
const int      CmxCpSubBlock::s_energyBits;
const int      CmxCpSubBlock::s_hitsBits;
const int      CmxCpSubBlock::s_hitsErrorBits;
const int      CmxCpSubBlock::s_roiOverflowBits;
const int      CmxCpSubBlock::s_paddingBits;
const int      CmxCpSubBlock::s_bunchCrossingBits;
const int      CmxCpSubBlock::s_fifoOverflowBits;
const int      CmxCpSubBlock::s_topoChecksumBits;
const int      CmxCpSubBlock::s_topoMapBits;
const int      CmxCpSubBlock::s_topoCountsBits;
const int      CmxCpSubBlock::s_topoPaddingBits;
const int      CmxCpSubBlock::s_glinkPins;
const int      CmxCpSubBlock::s_modules;
const int      CmxCpSubBlock::s_tobsPerModule;
const int      CmxCpSubBlock::s_muxPhases;


CmxCpSubBlock::CmxCpSubBlock()
{
}

CmxCpSubBlock::~CmxCpSubBlock()
{
}

// Clear all data

void CmxCpSubBlock::clear()
{
  L1CaloSubBlock::clear();
  m_tobData.clear();
  m_hitsData.clear();
  m_presenceMaps.clear();
  m_overflow.clear();
}

// Return presence map for given CPM

unsigned int CmxCpSubBlock::presenceMap(const int slice,
                                        const int cpm) const
{
  unsigned int map = 0;
  const unsigned int ix = mapIndex(slice, cpm);
  if (ix < m_presenceMaps.size()) map = m_presenceMaps[ix];
  return map;
}

// Return chip for given cpm and tob

int CmxCpSubBlock::chip(const int slice, const int cpm, const int tob) const
{
  int ch = 0;
  const unsigned int ix = tobIndex(slice, cpm, tob);
  if (ix < m_tobData.size()) {
    ch = (m_tobData[ix] >> s_tobChipBit) & s_tobChipMask;
  }
  return ch;
}

// Return Local coordinate for given cpm and tob

int CmxCpSubBlock::localCoord(const int slice, const int cpm, const int tob) const
{
  int coord = 0;
  const unsigned int ix = tobIndex(slice, cpm, tob);
  if (ix < m_tobData.size()) {
    coord = (m_tobData[ix] >> s_tobCoordBit) & s_tobCoordMask;
  }
  return coord;
}

// Return isolation for given cpm and tob

int CmxCpSubBlock::isolation(const int slice, const int cpm, const int tob) const
{
  int isol = 0;
  const unsigned int ix = tobIndex(slice, cpm, tob);
  if (ix < m_tobData.size()) {
    isol = (m_tobData[ix] >> s_tobIsolationBit) & s_tobIsolationMask;
  }
  return isol;
}

// Return energy for given cpm and tob

int CmxCpSubBlock::energy(const int slice, const int cpm, const int tob) const
{
  int et = 0;
  const unsigned int ix = tobIndex(slice, cpm, tob);
  if (ix < m_tobData.size()) {
    et = (m_tobData[ix] >> s_tobEnergyBit) & s_tobEnergyMask;
  }
  return et;
}

// Return error bits for given cpm and tob

int  CmxCpSubBlock::tobError(int slice, int cpm, int tob) const
{
  int error = 0;
  const unsigned int ix = tobIndex(slice, cpm, tob);
  if (ix < m_tobData.size()) {
    error = (m_tobData[ix] >> s_tobErrorBit) & s_tobErrorMask;
  }
  return error;
}

// Return hit/topo counts for given source ID and HL flag

unsigned int CmxCpSubBlock::hits(const int slice, const int source,
                                                  const int flag) const
{
  unsigned int hits = 0;
  const unsigned int ix = hitIndex(slice, source, flag);
  if (ix < m_hitsData.size()) {
    hits = (m_hitsData[ix] >> s_threshBit) & s_threshMask;
  }
  return hits;
}

// Return hit error for given source ID and HL flag

int CmxCpSubBlock::hitsError(const int slice, const int source,
                                              const int flag) const
{
  int error = 0;
  const unsigned int ix = hitIndex(slice, source, flag);
  if (ix < m_hitsData.size()) {
    error = (m_hitsData[ix] >> s_threshErrorBit) & s_errorMask;
  }
  return error;
}

// Return RoI overflow for given source ID

int CmxCpSubBlock::roiOverflow(const int slice, const int source) const
{
  int overflow = 0;
  const unsigned int ix = ovfIndex(slice, source);
  if (ix < m_overflow.size()) overflow = m_overflow[ix];
  return overflow;
}

// Store presence map

void CmxCpSubBlock::setPresenceMap(const int slice, const int cpm,
                                                    const unsigned int map)
{
  resize();
  if (map) {
    const unsigned int ix = mapIndex(slice, cpm);
    if (ix < m_presenceMaps.size()) m_presenceMaps[ix] = map;
  }
}

// Store TOB (RoI) data for given CPM, chip, local coord

void CmxCpSubBlock::setTob(const int slice, const int cpm, const int chip,
                           const int loc, const int energy, const int isol,
			   const int error)
{
  resize();
  if (energy || isol || error) {
    uint32_t word = 0;
    word |= (energy & s_tobEnergyMask)    << s_tobEnergyBit;
    word |= (isol   & s_tobIsolationMask) << s_tobIsolationBit;
    word |= (error  & s_tobErrorMask)     << s_tobErrorBit;
    word |= (loc    & s_tobCoordMask)     << s_tobCoordBit;
    word |= (chip   & s_tobChipMask)      << s_tobChipBit;
    word |= (cpm    & s_tobCpmMask)       << s_tobCpmBit;
    word |= (s_tobWordId)                 << s_dataWordIdBit;
    // Order by chip == presence bit                                          // <<== CHECK
    for (int tob = 0; tob < s_tobsPerModule; ++tob) {
      const unsigned int ix = tobIndex(slice, cpm, tob);
      if (m_tobData[ix] == 0) {
        m_tobData[ix] = word;
	break;
      } else {
        const int chipOld = (m_tobData[ix]>>s_tobChipBit)&s_tobChipMask;
	if (chip < chipOld) {
          for (int i = s_tobsPerModule-tob-1; i > 0; --i) {
	    m_tobData[ix + i] = m_tobData[ix + i - 1];
          }
	  m_tobData[ix] = word;
	  break;
        }
      }
    }
  }
}

// Store hit counts for given source ID and HL flag

void CmxCpSubBlock::setHits(const int slice, const int source, const int flag,
                            const unsigned int hits, const int error)
{
  resize();
  const unsigned int ix = hitIndex(slice, source, flag);
  if (ix < m_hitsData.size() && (hits || error)) {
    uint32_t word = m_hitsData[ix];
    word |= (hits & s_threshMask)     << s_threshBit;
    word |= (error & s_errorMask)     << s_threshErrorBit;
    word |= (flag & s_hlFlagMask)     << s_hlFlagBit;
    word |= (source & s_sourceIdMask) << s_sourceIdBit;
    word |= (s_threshWordId)          << s_dataWordIdBit;
    m_hitsData[ix] = word;
  }
}

// Store RoI overflow for given source ID

void CmxCpSubBlock::setRoiOverflow(const int slice, const int source,
                                                    const int overflow)
{
  resize();
  if (overflow) {
    const unsigned int ix = ovfIndex(slice, source);
    if (ix < m_overflow.size()) m_overflow[ix] = overflow;
  }
}

// Packing/Unpacking routines

bool CmxCpSubBlock::pack()
{
  bool rc = false;
  switch (version()) {
    case 1:  
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

bool CmxCpSubBlock::unpack()
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

unsigned int CmxCpSubBlock::mapIndex(const int slice,
                                     const int cpm) const
{
  unsigned int ix = cpm - 1;
  if (format() == NEUTRAL) ix += slice * s_modules;
  return ix;
}

// Return tob data index appropriate to format

unsigned int CmxCpSubBlock::tobIndex(const int slice, const int cpm,
                                             const int tob) const
{
  unsigned int ix = (cpm - 1) * s_tobsPerModule + tob;
  if (format() == NEUTRAL) ix += slice * s_modules * s_tobsPerModule;
  return ix;
}

// Return hits data index appropriate to format

unsigned int CmxCpSubBlock::hitIndex(const int slice, const int source,
                                             const int flag) const
{
  unsigned int ix = (source<<1)|flag;
  if (format() == NEUTRAL) ix += slice * 2 * MAX_SOURCE_ID;
  return ix;
}

// Return overflow index appropriate to format

unsigned int CmxCpSubBlock::ovfIndex(const int slice, const int source) const
{
  unsigned int ix = source;
  if (format() == NEUTRAL) ix += slice * MAX_SOURCE_ID;
  return ix;
}

// Resize the data vectors according to format

void CmxCpSubBlock::resize()
{
  if (m_tobData.empty()) {
    int size1 = s_modules * s_tobsPerModule;
    int size2 = 2 * MAX_SOURCE_ID;
    int size3 = s_modules;
    int size4 = MAX_SOURCE_ID;
    if (format() == NEUTRAL) {
      size1 *= timeslices();
      size2 *= timeslices();
      size3 *= timeslices();
      size4 *= timeslices();
    }
    m_tobData.resize(size1);
    m_hitsData.resize(size2);
    m_presenceMaps.resize(size3);
    m_overflow.resize(size4);
  }
}

// Pack neutral data

bool CmxCpSubBlock::packNeutral()
{
  resize();
  std::vector<int> locVec(s_tobsPerModule);
  std::vector<int> isolVec(s_tobsPerModule);
  std::vector<int> parityVec(s_muxPhases);
  std::vector<int> energyVec(s_tobsPerModule);
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      if (pin < s_modules) { // TOB data
	const int cpm = pin + 1;
        // Presence map
        packerNeutral(pin, presenceMap(slice, cpm), s_presenceBits);
	// Get tob data for this cpm
	locVec.clear();
	isolVec.clear();
	parityVec.clear();
	energyVec.clear();
	int parityMerge = 0;
	for (int tob = 0; tob < s_tobsPerModule; ++tob) {
	  locVec.push_back(localCoord(slice, cpm, tob));
	  isolVec.push_back(isolation(slice, cpm, tob));
	  energyVec.push_back(energy(slice, cpm, tob));
	  if (tob == 0) {
	    const int err = tobError(slice, cpm, tob);
	    parityMerge = err&s_parityErrorMask;
	    for (int mux = 0; mux < s_muxPhases; ++mux) {
	      parityVec.push_back((err>>(mux+s_parityErrorBits))&s_parityErrorMask);
	    }
          }
	}
	// And pack
	int locCount = 0;
	int isolCount = 0;
	int parityCount = 0;
        int energyCount = 0;
        for (int tob = 0; tob < s_tobsPerModule-2; ++tob) {
          // Local coordinates (2 bit)
	  packerNeutral(pin, locVec[locCount++], s_coordBits);
	  // isolation
	  packerNeutral(pin, isolVec[isolCount++], s_isolationBits);
	  // backplane parity error
	  packerNeutral(pin, parityVec[parityCount++], s_parityErrorBits);
	  // energy
	  packerNeutral(pin, energyVec[energyCount++], s_energyBits);
	  if (tob < s_tobsPerModule-3) {
	    packerNeutral(pin, energyVec[energyCount++], s_energyBits);
          } else {
	    packerNeutral(pin, locVec[locCount++], s_coordBits);
	    packerNeutral(pin, isolVec[isolCount++], s_isolationBits);
	    packerNeutral(pin, parityMerge, s_parityErrorBits);
	    packerNeutral(pin, locVec[locCount++], s_coordBits);
	    packerNeutral(pin, isolVec[isolCount++], s_isolationBits);
	    packerNeutral(pin, parityVec[parityCount++], s_parityErrorBits);
	  }
        }
      } else { // Hits and Topo data
        if (pin < s_glinkPins-1) {
          // Remote(3), local and total hits; parity error
          const int source = pin - s_modules;
          packerNeutral(pin, hits(slice, source, 0), s_hitsBits);
          packerNeutral(pin, hitsError(slice, source, 0), s_hitsErrorBits);
          packerNeutral(pin, hits(slice, source, 1), s_hitsBits);
          packerNeutral(pin, hitsError(slice, source, 1), s_hitsErrorBits);
          packerNeutral(pin, roiOverflow(slice, source), s_hitsErrorBits);
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

bool CmxCpSubBlock::packUncompressed()
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

bool CmxCpSubBlock::unpackNeutral()
{
  resize();
  int bunchCrossing = 0;
  int fifoOverflow = 0;
  int glinkParity = 0;
  std::vector<int> locVec(s_tobsPerModule);
  std::vector<int> isolVec(s_tobsPerModule);
  std::vector<int> parityVec(s_muxPhases);
  std::vector<int> energyVec(s_tobsPerModule);
  const int slices = timeslices();
  for (int slice = 0; slice < slices; ++slice) {
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      if (pin < s_modules) { // TOB data
        // Presence map
	const unsigned int map = unpackerNeutral(pin, s_presenceBits);
	locVec.clear();
	isolVec.clear();
	parityVec.clear();
	energyVec.clear();
	int parityMerge = 0;
	for (int tob = 0; tob < s_tobsPerModule-2; ++tob) {
	  // Local coordinates (2 bit)
	  locVec.push_back(unpackerNeutral(pin, s_coordBits));
	  // isolation
	  isolVec.push_back(unpackerNeutral(pin, s_isolationBits));
	  // backplane parity error
	  parityVec.push_back(unpackerNeutral(pin, s_parityErrorBits));
	  // energy
	  energyVec.push_back(unpackerNeutral(pin, s_energyBits));
	  if (tob < s_tobsPerModule-3) {
	    energyVec.push_back(unpackerNeutral(pin, s_energyBits));
          } else {
	    locVec.push_back(unpackerNeutral(pin, s_coordBits));
	    isolVec.push_back(unpackerNeutral(pin, s_isolationBits));
	    parityMerge = unpackerNeutral(pin, s_parityErrorBits);
	    locVec.push_back(unpackerNeutral(pin, s_coordBits));
	    isolVec.push_back(unpackerNeutral(pin, s_isolationBits));
	    parityVec.push_back(unpackerNeutral(pin, s_parityErrorBits));
          }
        }
	int ntobs = 0;
	for (int bit = 0; bit < s_presenceBits; ++bit) ntobs += (map>>bit)&1;
	int error = parityMerge;
	for (int mux = 0; mux < s_muxPhases; ++mux) {
	  error |= (parityVec[mux]<<(mux+s_parityErrorBits));
	}
	if (ntobs > s_tobsPerModule) error |= (1<<(s_muxPhases+s_parityErrorBits)); // overflow
	int tob = 0;
	const int cpm = pin + 1;
	for (int chip = 0; chip < s_presenceBits && tob < s_tobsPerModule; ++chip) {  // <<== CHECK - assuming bit==chip
	  if ((map>>chip)&1) {
	    setTob(slice, cpm, chip, locVec[tob], energyVec[tob], isolVec[tob], error);
	    ++tob;
          }
        }
        setPresenceMap(slice, cpm, map);
      } else {  // Hits and Topo data
        if (pin < s_glinkPins-1) {
	  // Remote(3), local and total hits; parity error
	  const int source = pin - s_modules;
	  unsigned int hits = unpackerNeutral(pin, s_hitsBits);
	  int error = unpackerNeutral(pin, s_hitsErrorBits);
	  setHits(slice, source, 0, hits, error);
	  hits = unpackerNeutral(pin, s_hitsBits);
	  error = unpackerNeutral(pin, s_hitsErrorBits);
	  setHits(slice, source, 1, hits, error);
	  error = unpackerNeutral(pin, s_hitsErrorBits);
	  setRoiOverflow(slice, source, error);
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

bool CmxCpSubBlock::unpackUncompressed()
{
  resize();
  const int maxHits = m_hitsData.size();
  m_cpmTobCount.assign(s_modules, 0);
  unpackerInit();
  uint32_t word = unpacker(s_wordLength);
  while (unpackerSuccess()) {
    const int id = dataWordId(word);
    if (id == s_tobWordId) {  // TOB data
      const int index = cpm(word)-1;
      const int count = m_cpmTobCount[index];
      const int index2 = index*s_tobsPerModule + count;
      if (count < s_tobsPerModule) {
        m_tobData[index2] = word;
        ++m_cpmTobCount[index];
      } else {
        setUnpackErrorCode(UNPACK_EXCESS_TOBS);             // New code.  Check consequences
	return false;
      }
    } else if (id == s_threshWordId) {  // Hits and Topo data
      const int index = (sourceId(word)<<1) | hlFlag(word);
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
