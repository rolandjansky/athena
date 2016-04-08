/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_RawData.h
//   Header file for class BCM_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to define BCM raw data objects
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
// Version 1.1 19/03/2008 Daniel Dobos for BCM
// Version 1.2 14/05/2008 Daniel Dobos
// Version 1.3 23/05/2008 Dominique Tardif
// Version 1.4 03/02/2009 Daniel Dobos
///////////////////////////////////////////////////////////////////

////////////////////////
// Coding of the RDO
// Channel        : word1 : 0x0000000F :  4-bit
// Pulse1Position : word1 : 0x000003F0 :  6-bit
// Pulse1Width    : word1 : 0x00007C00 :  5-bit
// Pulse2Position : word1 : 0x001F8000 :  6-bit
// Pulse2Width    : word1 : 0x03E00000 :  5-bit
// LVL1A          : word1 : 0xFC000000 :  6-bit
// BCID           : word2 : 0x00000FFF : 12-bit
// LVL1ID         : word2 : 0x0FFFF000 : 16-bit
// Error          : word2 : 0xF0000000 :  4-bit
////////////////////////
 
#ifndef INDETBCM_RAWDATA_BCM_RAWDATA_H
#define INDETBCM_RAWDATA_BCM_RAWDATA_H

class BCM_RawData {

public:

  // public default constructor needed for I/O, but should not be called from an alg
  BCM_RawData();

  // constructor with parameters: word1, word2
  BCM_RawData(const unsigned int word1, const unsigned int word2);

  // constructor with parameter list: Channel, Pulse1Position, Pulse1Width, Pulse2Position, Pulse2Width, LVL1A, BCID, LVL1ID 
  BCM_RawData(const unsigned int Channel,
	      const unsigned int Pulse1Position,
	      const unsigned int Pulse1Width,
              const unsigned int Pulse2Position,
              const unsigned int Pulse2Width,
              const unsigned int LVL1A,
              const unsigned int BCID,
              const unsigned int LVL1ID);

  // constructor with full parameter list: Channel, Pulse1Position, Pulse1Width, Pulse2Position, Pulse2Width, LVL1A, BCID, LVL1ID, Error
  BCM_RawData(const unsigned int Channel,
              const unsigned int Pulse1Position,
              const unsigned int Pulse1Width,
              const unsigned int Pulse2Position,
              const unsigned int Pulse2Width,
              const unsigned int LVL1A,
              const unsigned int BCID,
              const unsigned int LVL1ID,
              const unsigned int Error);

  // destructor
  virtual ~BCM_RawData();

  // virtual methods
  virtual int getWord1() const { return m_word1; }
  virtual int getWord2() const { return m_word2; }
  virtual int getChannel() const { return (m_word1 & 0xF); }                // Channel identifier       0-15
  virtual int getPulse1Position() const { return ((m_word1>>4) & 0x3F); }   // Position of first pulse  0-63
  virtual int getPulse1Width() const { return ((m_word1>>10) & 0x1F); }     // Width of first pulse     0-31
  virtual int getPulse2Position() const { return ((m_word1>>15) & 0x3F); }  // Position of second pulse 0-63
  virtual int getPulse2Width() const { return ((m_word1>>21) & 0x1F); }     // Width of second pulse    0-31
  virtual int getLVL1A() const { return ((m_word1>>26) & 0x3F); }           // Level 1 accept,          0-63
  virtual int getBCID() const { return (m_word2 & 0xFFF); }                 // Bunch Crossing ID        0-4096
  virtual int getLVL1ID() const { return ((m_word2>>12) & 0xFFFF); }        // Level 1 ID               0-65535
  virtual int getError() const { return ((m_word2>>28) & 0xF); }            // Error                    0-15

private:

  unsigned int m_word1;
  unsigned int m_word2;

};

#endif // INDETBCM_RAWDATA_BCM_RAWDATA_H
