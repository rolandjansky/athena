/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ZDCL1CALOUSERHEADER_H
#define TRIGT1CALOBYTESTREAM_ZDCL1CALOUSERHEADER_H

#include <stdint.h>

//namespace LVL1BS {

/** L1Calo User Header class.
 *
 *  The User Header is the first word of the ROD data and contains
 *  Triggered slice offsets for all the sub-detector types.
 *
 *  @author Peter Faulkner
 */
                                                    // Remove pre version 2 stuff ?
class ZdcL1CaloUserHeader {

 public:

   /// Constructor - default just sets word ID and number of header words
   ZdcL1CaloUserHeader(uint32_t header = 0xf0000001);

   /// Return packed header
   uint32_t header() const;

   /// Return number of header words (should be one)
   int words()   const;

   //  Return triggered slice offsets
   int jepCmm()  const;
   int cpCmm()   const;
   int jem()     const;
   int cpm()     const;
   int ppmLut()  const;
   int ppmFadc() const;

   /// Return FADC lower bound
   int lowerBound() const;

   //  Set triggered slice offsets
   void setJepCmm(int offset);
   void setCpCmm(int offset);
   void setJem(int offset);
   void setCpm(int offset);
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
   static const int s_jepCmmBit  = 24;
   static const int s_cpCmmBit   = 20;
   static const int s_jemBit     = 16;
   static const int s_cpmBit     = 12;
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

inline uint32_t ZdcL1CaloUserHeader::header() const
{
  return m_header;
}

inline int ZdcL1CaloUserHeader::words() const
{
  return m_header & s_mask;
}

inline int ZdcL1CaloUserHeader::jepCmm() const
{
  return (m_version2) ? (m_header >> s_jemBit   ) & s_mask
                      : (m_header >> s_jepCmmBit) & s_mask;
}

inline int ZdcL1CaloUserHeader::cpCmm() const
{
  return (m_version2) ? (m_header >> s_cpmBit  ) & s_mask
                      : (m_header >> s_cpCmmBit) & s_mask;
}

inline int ZdcL1CaloUserHeader::jem() const
{
  return (m_header >> s_jemBit) & s_mask;
}

inline int ZdcL1CaloUserHeader::cpm() const
{
  return (m_header >> s_cpmBit) & s_mask;
}

inline int ZdcL1CaloUserHeader::ppmLut() const
{
  return (m_version2) ? (m_header >> s_ppmLutBitV2) & s_ppmLutMaskV2
                      : (m_header >> s_ppmLutBit  ) & s_mask;
}

inline int ZdcL1CaloUserHeader::ppmFadc() const
{
  return (m_version2) ? (m_header >> s_ppmFadcBitV2) & s_ppmFadcMaskV2
                      : (m_header >> s_ppmFadcBit  ) & s_mask;
}

inline int ZdcL1CaloUserHeader::lowerBound() const
{
  return (m_version2) ? (m_header >> s_lowerBoundBit) & s_lowerBoundMask
                      : 0;
}

inline void ZdcL1CaloUserHeader::setJepCmm(const int offset)
{
  if (!m_version2) m_header |= (s_mask & offset) << s_jepCmmBit;
}

inline void ZdcL1CaloUserHeader::setCpCmm(const int offset)
{
  if (!m_version2) m_header |= (s_mask & offset) << s_cpCmmBit;
}

inline void ZdcL1CaloUserHeader::setJem(const int offset)
{
  m_header |= (s_mask & offset) << s_jemBit;
}

inline void ZdcL1CaloUserHeader::setCpm(const int offset)
{
  m_header |= (s_mask & offset) << s_cpmBit;
}

inline void ZdcL1CaloUserHeader::setPpmLut(const int offset)
{
  m_header |= (m_version2) ? (s_ppmLutMaskV2 & offset) << s_ppmLutBitV2
                           : (s_mask         & offset) << s_ppmLutBit;
}

inline void ZdcL1CaloUserHeader::setPpmFadc(const int offset)
{
  m_header |= (m_version2) ? (s_ppmFadcMaskV2 & offset) << s_ppmFadcBitV2
                           : (s_mask          & offset) << s_ppmFadcBit;
}

inline void ZdcL1CaloUserHeader::setLowerBound(const int bound)
{
  if (m_version2) m_header |= (s_lowerBoundMask & bound) << s_lowerBoundBit;
}

inline void ZdcL1CaloUserHeader::setVersion(const int minorVersion)
{
  m_version2 = (minorVersion > s_version1);
}

//} // end namespace

#endif
