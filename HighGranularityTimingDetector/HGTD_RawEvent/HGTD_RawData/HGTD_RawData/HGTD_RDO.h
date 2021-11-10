/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RawData/HGTD_RDO.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief This is a first implementation, but needs improvement in the future
 * to be closer to real detector output. For now a format compatible with
 *  HGTD_Cluster was chosen for simplicity.
 *
 * Raw data word as sent from the ASIC.
 *  Contains: - TOA  7 bits [0, 127]
 *            - TOT  9 bits [0, 511]
 *            - BCID ?? (5 bits left in 32 bit word), but seems to be 8 bits
 *            - L1A  4 bits [0-15]
 *            - L1ID 8 bits [0-255]
 * for pixel: 8 (TOT) + 8 (BCID) + 4 (L1A) + 8 (L1ID) = 28 < 32
 * for HGTD: 7 (TOA) + 9 (TOT) + 8 (BCID) + 4 (L1A) + 8 (L1ID) = 36 > 32!!
 * unsigned int -> 32 bits on 64 bit machines
 * unsigned long long -> 64 bits on 64 bit machines

 * unsigned int m_word; //This is used for Pixel, eqv to uint32_t

 * uint64_t m_word; // FIXME might be that I have to use this here!!

 * since is quite large, can I use std::bitset<N> here? =>> wastes space?!

 * maybe better: bit field ??
 * struct HGTD_RAWBitfield {
 *   unsigned int toa : 7;
 *   unsigned int tot : 9;
 *   unsigned int bcid : 8;
 *   unsigned int l1a : 4;
 *   unsigned int l1id : 8;
 * };
 * but: The layout of bit-fields is non-portable && might give issues with
 * ROOT I/O
 * maybe splitting the word into individual data members best?
 * 16 bit for TAO & TOT (uint16_t)
 * >=20 bit needed for bcid, l1a, l1id (uint16_t + uint8_t, grouped)
 * totals to 40 bits
*/

#ifndef HGTD_RAWDATA_HGTD_RDO_H
#define HGTD_RAWDATA_HGTD_RDO_H

#include "Identifier/Identifiable.h"

#include "Identifier/Identifier.h"

#include <cstdint>

class HGTD_RDO : public Identifiable {

public:
  /**
   * @brief Default constructor should NOT be used, needed for pool I/O.
   */
  HGTD_RDO() = default;
  HGTD_RDO(const HGTD_RDO&) = default;
  HGTD_RDO(HGTD_RDO&&) = default;
  HGTD_RDO& operator=(const HGTD_RDO&) = default;
  HGTD_RDO& operator=(HGTD_RDO&&) = default;

  // Destructor:
  virtual ~HGTD_RDO() = default;

  /**
   * @brief   Constructor with parameters
   *
   * @param [in] rdo_id Offline compact identifier of the readout channel.
   * @param [in] toa Time of arrival, 7 bit word.
   * @param [in] tot Time over threshold, 9 bit word.
   * @param [in] bcid Bunch crossing ID.
   * @param [in] l1_id ATLAS LVL1
   * @param [in] l1_a Level 1 accept
   * @param [in]
   */
  HGTD_RDO(const Identifier rdo_id, const float toa, const int tot,
                  const unsigned short bcid, const unsigned short l1_id,
                  const unsigned short l1_a = 0);

  virtual Identifier identify() const;

  virtual float getTOA() const;
  virtual unsigned int getTOT() const;
  virtual unsigned short getBCID() const;
  virtual unsigned short getL1ID() const;
  virtual unsigned short getL1A() const;

private:
  /** @brief Offline ID of the readout channel.
   */
  Identifier m_rdo_id;

  /** @brief Time of arrival, 7 bit word.
   */
  float m_toa;

  /** @brief Time over threshold, 9 bit word.
   */
  int m_tot;

  /** @brief Bunch crossing ID.
   */
  unsigned short m_bcid;

  /** @brief Level 1 accept.
   */
  unsigned short m_l1_id;

  /** @brief ATLAS LVL1.
   */
  unsigned short m_l1_a;
};

inline Identifier HGTD_RDO::identify() const { return m_rdo_id; }

inline float HGTD_RDO::getTOA() const { return m_toa; }

inline unsigned int HGTD_RDO::getTOT() const { return m_tot; }

inline unsigned short HGTD_RDO::getBCID() const { return m_bcid; }

inline unsigned short HGTD_RDO::getL1ID() const { return m_l1_id; }

inline unsigned short HGTD_RDO::getL1A() const { return m_l1_a; }

#endif // HGTD_RAWDATA_HGTD_RDORAWDATA_H
