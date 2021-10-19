/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileCell.h
// Author   : Ed Frank, Ambreesh Gupta
//          : University of Chicago
//
//
// DESCRIPTION:
//    A TileCell is a CaloCell, the latter representing the basic observation
// in the Atlas calorimeters:  An energy, position, time and quality.  A
// CaloCell has been calibrated so that energy() returns the physical energy
// deposit _in the cell_ with units of MeV, but without any kind of
// leakage corrections.  Time represents when the feature extraction thinks
// the deposit occured, in nanoseconds, relative to the trigger.  It ought
// to be zero for good hits. 
// Quality is an 32-bits integer number, which is split in 4 unsigned chars
// m_tileQual[4] like this:
//   m_tileQual[0] and m_tileQual[1] are  unsigned quality from TileRawChannel for first and second PMT
//   m_tileQual[2] and m_tileQual[3] contains special bits for first and second PMT respectively:
//     bit [0-2] -  the algorithm used to reconstruct given channel
//                  (one of TileFragHash::TYPE)
//     bit 3      - the channel is marked as bad in COOL DB
//     bit 4      - signal overflow detected in ADC samples
//     bit 5      - amplitude is large enough so that time will be saved in ESD
//     bit 6      - TileCell was read from Compact CaloCellContainer
//     bit 7      - TileCell contains non-zero time
//
//
// HISTORY:
//    nnFeb00  Created by Ambreesh
//    24Jan01  Made to inherit from CaloCell.  This breaks TileG3Cell.
//
//    12June02 [Zhifang Wu] Completely changed.  Remove duplicate variable definitions caused by improper inheritance.
//    14June02 [FSM] remove constructors taking (eta, phi) or (x,y,z).  Only allow stantard constructor, default.
//    16July04 [ALupi] new constructor added for Cells created from TileBeamElems
//    25Aug04  [Solodkov] timeDiff added
//    20Oct04  [Solodkov] new approach for quality 
//
//    20-May-2008 [Solodkov] migrating to floats 
//    20-Jul-2008 [Solodkov] store quality in unit8 quality[4] array 
//
// BUGS:
//
// ***************************************************************************

#ifndef TILEEVENT_TILECELL_H
#define TILEEVENT_TILECELL_H

#include "CaloEvent/CaloCell.h"
#include "Identifier/IdentifierHash.h"

#include <string>

class TileCell : public CaloCell {
public:

  /** @brief definition of various bits in quality  */
  enum QUALITY_BITS { 
    MASK_ALGO = 0x07,
    MASK_BADCH= 0x08,
    MASK_OVER = 0x10,
    MASK_AMPL = 0x20,
    MASK_CMPC = 0x40,
    MASK_TIME = 0x80,

    SHIFT_ALGO = 0,
    SHIFT_BADCH= 3,
    SHIFT_OVER = 4,
    SHIFT_AMPL = 5,
    SHIFT_CMPC = 6,
    SHIFT_TIME = 7,

    // these 5 bits will be saved in compact CaloCellContainer and provenance for every PMT
    // they are usually the same for all channels except bad channels
    MASK_PROV = MASK_ALGO + MASK_BADCH + MASK_OVER,

    // time saved in compact CaloCellContainer
    // if provenance for one PMT is greater or equal KEEP_TIME
    // i.e. at least bit[7] and bit[5]  or  bit[7] and bit[6] should be set
    KEEP_TIME = MASK_TIME + MASK_AMPL
  };

  /** @brief default constructor */
  TileCell();

  /** @brief constructor for TileCell when we don't care about
      PMT energies separetely (e.g. cells from Atlfast)
      only cell energy,time,quality here, no channel quality */
  TileCell(const CaloDetDescrElement* const & caloDDE,
           float energy, float time=0.0, 
	   uint16_t quality = 0, uint16_t provenance = 0,
           CaloGain::CaloGain gain=CaloGain::INVALIDGAIN);

  /** @brief constructor which also works for cells without CaloDDE, e.g. MBTS
      (passing null CaloDDE pointer and valid cell identifier for them) */ 
  TileCell(const CaloDetDescrElement* const & caloDDE,
           const Identifier & cell_ID,
           float energy, float time=0.0,
	   uint16_t quality = 0, uint16_t provenance = 0,
           CaloGain::CaloGain gain=CaloGain::INVALIDGAIN);

  /** @brief constructor using full info from both channels separately */
  TileCell(const CaloDetDescrElement* const & caloDDE,
           float ene1, float ene2, 
           float time1, float time2,
           int qual1, int qual2,
           int qbit1, int qbit2,
           int gain1, int gain2);

  /** @brief constructor used in clone() method - just copy everything */
  TileCell(const CaloDetDescrElement* const & caloDDE,
           const Identifier & cell_ID,
           float energy, float time,
           uint16_t quality, uint16_t provenance,
           CaloGain::CaloGain,
           float eneDiff, float timeDiff);

  /** @brief copy constructor */
  TileCell(const TileCell *cell);

  /** @brief destructor */
  virtual ~TileCell();

  /** @brief clone */
  virtual std::unique_ptr<CaloCell> clone() const ;

  /** all set methods */

  /** @brief set CaloDDE pointer */
  virtual void setDDE(CaloDetDescrElement* const & caloDDE) { m_caloDDE = caloDDE; }

  /** @brief set total energy, reset eneDiff to zero */
  virtual void setEnergy(float ene);
  /** @brief set energy and gain for both PMTs */
  virtual void setEnergy(float e1, float e2, int gain1, int gain2); // signal from 2 PMTs
  void setEnergy_nonvirt(float e1, float e2, int gain1, int gain2); // signal from 2 PMTs
  /** @brief set energy for both PMTs */
  virtual void setEnergy(float e1, float e2);
  /** @brief add energy, keep eneDiff */
  virtual void addEnergy(float e);
  /** @brief scale energy and eneDiff */
  virtual void scaleEnergy(float scale);
  /** @brief set energy and gain for one PMT */
  virtual void addEnergy(float e, int pmt, int gain);
  /** @brief set the same gain for two PMTs and set energy diff to zero */
  void setEqual_nonvirt(int gain);

  /** @brief set cell time, reset timeDiff */
  virtual void setTime(float t);
  void setTime_nonvirt(float t);
  /** @brief set time for one PMT, correct timeDiff */
  virtual void setTime(float t, int pmt);

  /** @brief set quality value and quality bits for one PMT */
  virtual void setQuality(unsigned char qual, unsigned char qbit, int pmt);
  void setQuality_nonvirt(unsigned char qual, unsigned char qbit, int pmt);

  /** @brief set quality word (from CaloCell base class) */
  virtual void setQuality (uint16_t quality);
 
  /** @brief set Quality (from CaloCell base class, obsolete) */
  virtual void setQuality (double quality);

  /** @brief set quality of first PMT */
  void setQual1 (unsigned char qual) { m_tileQual[0] = qual; }
  /** @brief set quality of second PMT */
  // cppcheck-suppress objectIndex
  void setQual2 (unsigned char qual) { m_tileQual[1] = qual; }
  /** @brief set quality bits of first PMT */
  // cppcheck-suppress objectIndex
  void setQbit1 (unsigned char qbit) { m_tileQual[2] = qbit; }
  /** @brief set quality bits of second PMT */
  // cppcheck-suppress objectIndex
  void setQbit2 (unsigned char qbit) { m_tileQual[3] = qbit; }
  /** @brief set quality and quality bits of first PMT */
  // cppcheck-suppress objectIndex
  void setQual1 (unsigned char qual, unsigned char qbit) { m_tileQual[0] = qual; m_tileQual[2] = qbit; }
  /** @brief set quality and quality bits of second PMT */
  // cppcheck-suppress objectIndex
  void setQual2 (unsigned char qual, unsigned char qbit) { m_tileQual[1] = qual; m_tileQual[3] = qbit; }

  /** all get methods */

  /** @brief get energy diff for two PMTs (data member) */
  virtual float   eneDiff      (void)  const  { return m_eneDiff; }
  /** @brief get time diff for two PMTs (data member) */
  virtual float   timeDiff     (void)  const  { return m_timeDiff; }
  
  /** @brief get energy of first PMT */
  virtual float   ene1         (void)  const  { return (m_energy+m_eneDiff)/2.; }
  /** @brief get energy of second PMT */
  virtual float   ene2         (void)  const  { return (m_energy-m_eneDiff)/2.; }
  
  /** @brief get time of first PMT */
  virtual float   time1        (void)  const  { return (m_time+m_timeDiff); }
  /** @brief get time of second PMT */
  virtual float   time2        (void)  const  { return (m_time-m_timeDiff); }

  /** @brief get quality of first PMT (data member) */
  virtual uint8_t qual1 (void) const   { return m_tileQual[0]; }
  /** @brief get quality of second PMT (data member) */
  // cppcheck-suppress objectIndex
  virtual uint8_t qual2 (void) const   { return m_tileQual[1]; }
  /** @brief get quality bits of first PMT (data member) */
  // cppcheck-suppress objectIndex
  virtual uint8_t qbit1 (void) const   { return m_tileQual[2]; }
  /** @brief get quality bits of second PMT (data member) */
  // cppcheck-suppress objectIndex
  virtual uint8_t qbit2 (void) const   { return m_tileQual[3]; }
  /** @brief check if first PMT is in bad channel list and masked */
  // cppcheck-suppress objectIndex
  virtual bool badch1 (void) const   { return ((m_tileQual[2]&TileCell::MASK_BADCH) != 0); }
  /** @brief check if second PMT is in bad channel list and masked */
  // cppcheck-suppress objectIndex
  virtual bool badch2 (void) const   { return ((m_tileQual[3]&TileCell::MASK_BADCH) != 0); }
  /** @brief check if whole cell is bad (i.e. no energy measurement at all in this cell) */
  virtual bool badcell (void) const   { return (badch1() & badch2()); }

  /** @brief get gain of first PMT */
  virtual int     gain1        (void)  const;
  /** @brief get gain of second PMT */
  virtual int     gain2        (void)  const;
  
  /** @brief get subcalo hash for TileCal cells */
  IdentifierHash  subcalo_hash (void)  const  { return m_caloDDE->subcalo_hash(); }

  /** @brief get name of the object */
  virtual std::string whoami   (void) const { return "TileCell"; }
  /** @brief print all cell data memebers to stdout */
  virtual void        print    (void) const;
  /** @brief Convertion operator to a std::string 
      Can be used in a cast operation : (std::string) TileCell */
  virtual operator std::string() const;

private:

  /** @brief eneDiff = ene1 - ene2 */
  float      m_eneDiff;
  /** @brief timeDiff = (time1 - time2)/2. */
  float      m_timeDiff; 
  
  /** @brief merge quality of 2 PMTs in single cell quality */
  void updateQuality();
};

inline
void TileCell::setTime_nonvirt(float t)
{ 
   m_time = t;
   m_timeDiff = 0.0;
}

inline
void TileCell::setEnergy_nonvirt(float e1, float e2, int gain1, int gain2)
{ 
   m_eneDiff = e1-e2;
   m_energy  = e1+e2;
   m_gain = (CaloGain::CaloGain) ( 0xFFFFFFF0 | (static_cast<unsigned int>(gain2) << 2) | (gain1 & 3) );
}

inline
void TileCell::setQuality_nonvirt(unsigned char qual, unsigned char qbit, int pmt) {
  // cppcheck-suppress objectIndex
  m_tileQual[0+pmt] = qual;
  // cppcheck-suppress objectIndex
  m_tileQual[2+pmt] = qbit;
}

inline
void TileCell::setEqual_nonvirt(int gain) {
   m_eneDiff = 0;
   m_gain = (CaloGain::CaloGain) ( 0xFFFFFFF0 | (static_cast<unsigned int>(gain) << 2) | (gain & 3) );
}

#endif  // TILEEVENT_TILECELL_H
