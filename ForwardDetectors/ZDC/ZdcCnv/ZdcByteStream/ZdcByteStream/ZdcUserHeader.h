/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCUSERHEADER_H
#define ZDCUSERHEADER_H

#include <stdint.h>

/** L1Calo User Header class.
 *
 *  The User Header is the first word of the ROD data and contains
 *  Triggered slice offsets for all the sub-detector types.
 *
 *  @author Peter Faulkner
 *
 *  This is a modifyed version to be used only by ZDC
 *
 *  TRY_LINK_LVL1
 *
 *  Marco Leite
 */

class ZdcUserHeader {

 public:

   /// Constructor - default just sets word ID and number of header words
   ZdcUserHeader(uint32_t header = 0xf0000001);

   /// Return packed header
   uint32_t header() const;

   /// Return number of header words (should be one)
   int words()   const;

   //  Return triggered slice offsets
   int ppmLut()  const;
   int ppmFadc() const;

   /// Return FADC lower bound
   int lowerBound() const;

   //  Set triggered slice offsets
   void setPpmLut(int offset);
   void setPpmFadc(int offset);

   /// Set FADC lower bound
   void setLowerBound(int bound);

   /// Set version flag
   void setVersion(int minorVersion);

   /// Test for valid header word
   static bool isValid(uint32_t word);

 private:
   //  Packed word bit positions version 1
   static const int s_wordIdBit  = 28;
   static const int s_ppmLutBit  = 8;
   static const int s_ppmFadcBit = 4;
   //  Packed word bit positions version 2 (no cmms)
   static const int s_lowerBoundBit = 20;
   static const int s_ppmLutBitV2   = 9;
   static const int s_ppmFadcBitV2  = 4;
   /// Field mask
   static const uint32_t s_mask  = 0xf;
   //  Version 2 masks
   static const uint32_t s_lowerBoundMask = 0xff;
   static const uint32_t s_ppmLutMaskV2   = 0x7;
   static const uint32_t s_ppmFadcMaskV2  = 0x1f;
   /// Version 1 minor format version number
   static const int s_version1 = 0x1001;
   /// Packed Header
   uint32_t m_header;
   /// Version flag
   bool     m_version2;

};

inline uint32_t ZdcUserHeader::header() const
{
  return m_header;
}

inline int ZdcUserHeader::words() const
{
  return m_header & s_mask;
}


inline int ZdcUserHeader::ppmLut() const
{
  return (m_version2) ? (m_header >> s_ppmLutBitV2) & s_ppmLutMaskV2
                      : (m_header >> s_ppmLutBit  ) & s_mask;
}

inline int ZdcUserHeader::ppmFadc() const
{
  return (m_version2) ? (m_header >> s_ppmFadcBitV2) & s_ppmFadcMaskV2
                      : (m_header >> s_ppmFadcBit  ) & s_mask;
}

inline int ZdcUserHeader::lowerBound() const
{
  return (m_version2) ? (m_header >> s_lowerBoundBit) & s_lowerBoundMask
                      : 0;
}

inline void ZdcUserHeader::setPpmLut(const int offset)
{
  m_header |= (m_version2) ? (s_ppmLutMaskV2 & offset) << s_ppmLutBitV2
                           : (s_mask         & offset) << s_ppmLutBit;
}

inline void ZdcUserHeader::setPpmFadc(const int offset)
{
  m_header |= (m_version2) ? (s_ppmFadcMaskV2 & offset) << s_ppmFadcBitV2
                           : (s_mask          & offset) << s_ppmFadcBit;
}

inline void ZdcUserHeader::setLowerBound(const int bound)
{
  if (m_version2) m_header |= (s_lowerBoundMask & bound) << s_lowerBoundBit;
}

inline void ZdcUserHeader::setVersion(const int minorVersion)
{
  m_version2 = (minorVersion > s_version1);
}

#endif
