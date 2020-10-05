/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// TrigT1 include(s):
#include "TrigT1Interfaces/BitOp.h"

// Local include(s):
#include "TrigT1Result/MuCTPIRoI.h"

namespace ROIB {

  /**
   * This constructor should be used in any useful code, as this is the only way of giving
   * a non-zero value to the RoI word variable of the object.
   *
   * @param RoIWord   The 32-bit RoI word as collected by the RoI Builder
   */
  MuCTPIRoI::MuCTPIRoI( uint32_t RoIWord )
    : m_roIWord( RoIWord ) {}

  /**
   * Default constructor needed to be able to use POOL to stream the object. It initialises
   * the RoI word to zero.
   */
  MuCTPIRoI::MuCTPIRoI()
    : m_roIWord( 0 ) {}

  /**
   * This method can be used to access the "original" RoI word.
   *
   * @return The 32-bit RoI word
   */
  uint32_t MuCTPIRoI::roIWord() const {
    return m_roIWord;
  }

  /**
   * The candidate's p<sub>T</sub> is stored as a 3-bit threshold number, with the following meanings:
   *   - 000 : Non-defined behaviour
   *   - 001 : low-p<sub>T</sub> threshold 1
   *   - 010 : low-p<sub>T</sub> threshold 2
   *   - 011 : low-p<sub>T</sub> threshold 3
   *   - 100 : high-p<sub>T</sub> threshold 1
   *   - 101 : high-p<sub>T</sub> threshold 2
   *   - 110 : high-p<sub>T</sub> threshold 3
   *   - 111 : no candidate
   *
   * @return The 3-bit threshold number of the candidate
   */
  unsigned int MuCTPIRoI::pt() const {
    return ((m_roIWord >> MuCTPI_RDO::CAND_PT_SHIFT) &
	    MuCTPI_RDO::CAND_PT_MASK);
  }

  /**
   * Returns the address of the sector that the candidate came from. The sector
   * address is actually a combination of the sector number and 2 bits showing the
   * sector type. Both of which can be accessed by other member functions.
   *
   * @see MuCTPIRoI::getSectorID()
   * @see MuCTPIRoI::getSectorLocation()
   * @return The 8-bit sector address of the candidate
   */
  unsigned int MuCTPIRoI::getSectorAddress() const {
    return ((m_roIWord >> MuCTPIRoI::ROI_CAND_SECTOR_ADDRESS_SHIFT) &
	    MuCTPI_RDO::CAND_SECTOR_ADDRESS_MASK);
  }

  /**
   * Returns the hemisphere in which the candidate was detected.
   * 
   * @return 1 if the candidate has eta > 0, 0 if eta < 0.
   */
  uint16_t MuCTPIRoI::getHemisphere() const {
    return (getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK);
  }

  /**
   * The muon trigger sectors can only transmit information about 2 muon candidates
   * each. If one of them detects more than 2, it transmits the 2 with the highest
   * p<sub>T</sub> threshold, and raises this bit.
   *
   * @return 1 if the overflow bit was set, 0 otherwise
   */
  uint16_t MuCTPIRoI::getSectorOverflow() const {
    return ((m_roIWord >> MuCTPI_RDO::CAND_OVERFLOW_SHIFT) &
	    MuCTPI_RDO::CAND_OVERFLOW_MASK);
  }

  /**
   * The muon trigger sectors can't handle more than one muon candidates in a single
   * RoI. (RoI in this context a part of the muon detector, not the 32-bit word sent
   * by the hardware!) If more than 1 are detected, the one with the highest p<sub>T</sub>
   * threshold is submitted, and this bit is raised.
   *
   * @return 1 if the overflow bit was set, 0 otherwise
   */
  uint16_t MuCTPIRoI::getRoiOverflow() const {
    return ((m_roIWord >> MuCTPI_RDO::ROI_OVERFLOW_SHIFT) &
	    MuCTPI_RDO::ROI_OVERFLOW_MASK);
  }

  /**
   * The muon trigger sectors are divided into "RoIs", which are small parts of the
   * detector (in &eta; and &phi;) in this context. The number of RoIs in the different
   * sectors (especially in the barrel) are varying, but in general:
   *   - barrel sectors have <26 RoIs
   *   - endcap sectors have 148 RoIs
   *   - forward sectors have 64 RoIs
   *
   * @return The RoI in which the muon candidate was detected
   */
  uint16_t MuCTPIRoI::getRoiNumber() const {

    uint16_t roi = 0;
    switch ( getSectorLocation() ) {
    case MuCTPI_RDO::ENDCAP:
      roi = (m_roIWord >> MuCTPI_RDO::ROI_SHIFT) & MuCTPI_RDO::ENDCAP_ROI_MASK;
      break;
    case MuCTPI_RDO::FORWARD:
      roi = (m_roIWord >> MuCTPI_RDO::ROI_SHIFT) & MuCTPI_RDO::FORWARD_ROI_MASK;
      break;
    case MuCTPI_RDO::BARREL:
      roi = (m_roIWord >> MuCTPI_RDO::ROI_SHIFT) & MuCTPI_RDO::BARREL_ROI_MASK;
      break;
    default:
      roi = 99;
      break;
    }

    return roi;
  }

  /**
   * The barrel sectors can flag the muon candidates coming from a part of the trigger
   * sector that overlaps with another one. In theory both the barrel and endcap sectors
   * should be able to flag the candidates from around &eta; = 1.05 that could be
   * detected by both a barrel and endcap sector, but they don't do that.
   *
   * So even though all the following should be possible, only the first two are implemented:
   *   - 00 : the candidate is not coming from an overlap-area
   *   - 01 : the candidate is coming from a part of a barrel sector overlapping with
   *          another barrel sector
   *   - 10 : the candidate is coming from a part of a barrel/endcap sector overlapping
   *          with another endcap/barrel sector
   *   - 11 : the candidate is coming from a part of a barrel sector overlapping both
   *          with another barrel and an endcap sector
   *
   * @return The overlap bits of the candidate
   */
  uint16_t MuCTPIRoI::getOverlapBits() const {

    uint16_t ol = 0;
    switch( getSectorLocation() ) {
    case MuCTPI_RDO::ENDCAP:
      ol = (m_roIWord >> MuCTPI_RDO::ENDCAP_OL_SHIFT) & MuCTPI_RDO::ENDCAP_OL_MASK;
      break;
    case MuCTPI_RDO::FORWARD:
      ol = 0;
      break;
    case MuCTPI_RDO::BARREL:
      ol = (m_roIWord >> MuCTPI_RDO::BARREL_OL_SHIFT) & MuCTPI_RDO::BARREL_OL_MASK;
      break;
    default:
      ol = 99;
      break;
    }

    return ol;
  }

  /**
   * The system producing the muon candidate is encoded in the RoI word by the MuCTPI.
   * Using that, it's quite easy to decode which system produced the candidate.
   *
   * @return The name of the system that produced the muon candidate
   */
  MuCTPI_RDO::SectorLocation MuCTPIRoI::getSectorLocation() const {

    if( getSectorAddress() & MuCTPI_RDO::ENDCAP_ADDRESS_MASK ) {
      return MuCTPI_RDO::ENDCAP;
    } else if(getSectorAddress() & MuCTPI_RDO::FORWARD_ADDRESS_MASK) {
      return MuCTPI_RDO::FORWARD;
    } else {
      return MuCTPI_RDO::BARREL;
    }
  }

  /**
   * This is practically the number of the sector that produced the muon candidate.
   * Sectors are numbered in the following ranges:
   *   - 0 - 31 for the barrel
   *   - 0 - 47 for the endcap
   *   - 0 - 23 for the forward
   *
   * @return Number of the sector producing the muon candidate
   */
  uint16_t MuCTPIRoI::getSectorID() const {

    if( getSectorLocation() == MuCTPI_RDO::ENDCAP ) {
      return ((getSectorAddress() >> 1) & MuCTPI_RDO::ENDCAP_SECTORID_MASK);
    } else {
      return ((getSectorAddress() >> 1) & MuCTPI_RDO::BARREL_SECTORID_MASK);
    }
    return 0;
  }

  /**
   * This method returns bit 22 of the RoI word, just like MuCTPIRoI::firstBit() does.
   * I just realised this duplication while writing the documentation, eventually this should be removed.
   *
   * @see MuCTPIRoI::firstBit()
   * @return 0 or 1 depending on whether the candidate had the highest p<sub>T</sub> in its sector
   */
  uint16_t MuCTPIRoI::getCandidateIsHighestPt() const {
    return ((m_roIWord >> MuCTPIRoI::ROI_CAND_HIGHEST_PT_SHIFT) &
	    MuCTPI_RDO::CAND_HIGHEST_PT_MASK);
  }

} // namespace ROIB
