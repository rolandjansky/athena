// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileTTL1Cell.h
// Author   : Ed Frank, Ambreesh Gupta
//          : University of Chicago
//
//
// DESCRIPTION:
// TTL1Cell is the class for Tile Level 1 Trigger Towers where the energy and time are build from Tile cells
// This container is to add the L1Calo calibration where they can compare their tower energy to the tower energy
// from Tile's readout
// Each tower is contains a sum of Tile cells energies
// The time is the average time of all cells
// the correction factor is any pC to GeV correction factors that L1-Calo wants to account for separately
// the quality factor is defined as
// bit [0] -- individual PMTs bad
// bit [1] -- at least one cell bad
// bit [2] -- full tower is bad
// HISTORY:
//
// BUGS:
//
// ***************************************************************************

#ifndef TILEEVENT_TILETTL1CELL_H
#define TILEEVENT_TILETTL1CELL_H

#include "Identifier/Identifier.h"

#include <string>

class TileTTL1Cell {

public:

  /** @brief definition of various bits in quality  */
  enum QUALITY_BITS {  
    MASK_BADTOWER = 0x04,
    MASK_BADCELL  = 0x02, 
    MASK_BADCHAN  = 0x01, 

    SHIFT_BADTOWER = 2,
    SHIFT_BADCELL  = 1, 
    SHIFT_BADCHAN  = 0 
  };

  /** @brief default constructor */
  TileTTL1Cell()
    : m_eneTower(0.0),
      m_timeAve(0.0),
      m_corrFactor(0.0),
      m_quality(0)
  {}

  /** @brief constructor for TileTTL1Cell*/
  TileTTL1Cell(const Identifier& id,
	       float energy, float time, float correction_factor, 
	       uint16_t quality);

  /** @brief destructor */
  ~TileTTL1Cell() {}

  /** all set methods */
  /** @brief set total tower energy*/
  void setEnergy(float ene);
  /** @brief set average cell time */
  void setTime(float tave);
  /** @brief set correction factor */
  void setCorrectionFactor(float corrfact);
  /** @brief set quality bits for one tower */
  void setQuality(uint16_t qual);

  /** all get methods */
  /** @brief get identifier */
  const Identifier & TTL1_ID() const;
  inline Identifier identify(void) const { return m_ID; }

  /** @brief get energy for tower (data member)*/
  inline float   eneTower(void)  const  { return m_eneTower; }
  /** @brief get time average for all cells in tower (data member) */
  inline float   timeAve(void)  const  { return m_timeAve; }
  /** @brief get tower correction factor (data member) */
  inline float   corrFactor(void)  const  { return m_corrFactor; }
    /** @brief get quality factor for (data member) */
  inline uint16_t  qualTower(void)  const  { return m_quality; }

  /** @brief check if tower has bad cell */
  bool cellbad (void) const  { return ((m_quality&TileTTL1Cell::MASK_BADCELL) != 0); }
  /** @brief check if tower is bad */
  bool towerbad (void) const { return ((m_quality&TileTTL1Cell::MASK_BADTOWER) != 0); }
  /** @brief check if a pmt is bad */
  bool chanbad (void) const  { return ((m_quality&TileTTL1Cell::MASK_BADCHAN) != 0); }

  /** @brief get name of the object */
  std::string whoami   (void) const { return "TileTTL1Cell"; }
  /** @brief print all cell data memebers to stdout */
  void        print    (void) const;
  /** @brief Convertion operator to a std::string 
      Can be used in a cast operation : (std::string) TileTTL1Cell */
  operator std::string() const;

private:

  /** @brief ID = trigger tower id */
  Identifier m_ID;
  /** @brief eneTower = sum of cell energies */
  float      m_eneTower;
  /** @brief timeAve = time average of cell times. */
  float      m_timeAve; 
  /** @brief corrFactor = energy correction factors applied to tower */
  float      m_corrFactor; 
  /** @brief  quality = quality bit for cell and PMTs in tower  */
  uint16_t  m_quality; 
  
  
};

#endif  // TILEEVENT_TILETTL1CELL_H
