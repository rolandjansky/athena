/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_Chip.h
 * Header file storing infomration on the SCT chips: id, config, mask
 * @author Carl Gwilliam <gwilliam@mail.cern.ch>
**/

#ifndef SCT_ConditionServices_SCT_Chip_h
#define SCT_ConditionServices_SCT_Chip_h

#include <iosfwd>
#include <iomanip>
#include <stdint.h>
#include <vector>
#include <string>
#include <bitset>
#include <iostream>

/**
 * @class SCT_Chip
 * Class which stores infomration on the SCT chips: id, config, mask
**/

class SCT_Chip {

/** Outputstream operator for chip */
friend std::ostream& operator<<(std::ostream& output, const SCT_Chip& chip);
 
public:

  /** Default constructor */
  SCT_Chip();
  /** Actual constructor: create chip from id, config and four 32-bit masks */
  SCT_Chip(short id, short config, uint32_t mask0, uint32_t mask1, uint32_t mask2, uint32_t mask3);  
  /** Default destructor */
  ~SCT_Chip(){}

  /** Chip Id*/
  inline short id()          const {return m_id;}
  /** Active input port*/
  inline short inPort()      const {return m_in;}
  /** Active output port*/
  inline short outPort()     const {return m_out;}
  /** Is chip an end*/
  inline bool  isEnd()       const {return m_end;}
  /** Is chip a master*/
  inline bool  isMaster()    const {return m_master;}
  /** Can chip be a master (i.e position 0 or 6)*/
  inline bool  canBeMaster() const {return (m_id == 0 || m_id == 6);}

  /** Is this a slave chip mistakenly configured as a master*/
  inline bool  slaveConfiguredAsMaster() const {return (!canBeMaster() && isMaster());}

  /** Initialize channel mask from four ins (as in DB)*/
  bool         initializeMaskFromInts(uint32_t mask0, uint32_t mask1, uint32_t mask2, uint32_t mask3);
  /** Initialize channel mask from a string */
  bool         initializeMaskFromString(const std::string & maskString);
  /** Gives status of channel at channelNumber*/
  bool         channelIsMasked (const unsigned int channelNumber) const;
  /** Number of masked channels*/
  unsigned int numberOfMaskedChannels() const;
  /** Append masked channels' numbers to a user-supplied vector*/
  void         appendBadStripsToVector(std::vector<int> & maskedChannelVector) const;

private:

  /** Enum for lengths of words*/
  enum {nSubwords = 4, lenSubword = 32, nBitsMask = 128, nBitsConfig = 16};

  /** Enum describing the bits in the chip configuration*/
  enum {RO_BIT   = 0, CAL_BIT = 2, TR_BIT  = 4,  EDGE_BIT   = 6,  MASK_BIT = 7,
        ACC_BIT  = 8, IN_BIT  = 9, OUT_BIT = 10, MASTER_BIT = 11, END_BIT  = 12,
        FEED_BIT = 13};

  short                     m_id;          //!< Chip Id
  std::bitset<nBitsConfig>  m_config;      //!< Chip configuration mask
  short                     m_in;          //!< Active input port
  short                     m_out;         //!< Active output port
  bool                      m_end;         //!< Is chip an end
  bool                      m_master;      //!< Is chip a master
  std::bitset<nBitsMask>    m_mask;        //!< Chip strip mask 
  //unsigned int              m_numMasked;   //!< Number of masked chips
};

/** Output stream for printing chip information*/
inline std::ostream& operator<<(std::ostream& output, const SCT_Chip& chip) {
  output << "Chip " << std::setw(2) << chip.m_id << ":\t" 
         << (chip.m_master ? "MASTER" : "SLAVE") << (chip.m_end ? " END" : "") 
         << ", IN PORT = " << chip.m_in << ", OUT PORT = " << chip.m_out << "\n"
         << "Config = " << chip.m_config << "\n"
         << "Mask   = " << chip.m_mask;
  return output;
}

#endif

