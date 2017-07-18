/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiCellId.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SICELLID_H
#define INDETREADOUTGEOMETRY_SICELLID_H

#include <iostream>

namespace InDetDD {

  /**  @class SiCellId
    
    Identifier for the strip or pixel cell.
    Just the strip number for SCT or phi_index, eta_index for pixel.
    Packs the numbers in an int.
    Supports negative indices and is initialized in an invalid state 
    when using the default constructor.

    @author Grant Gorfine 
  */

  class SiCellId {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
    public:

  /// Constructor. Set in invalid state. 
  SiCellId(); 
  /// Construct with strip number. 
  SiCellId(int strip);
  /// Construct with phi, eta index. 
  SiCellId(int phiIndex, int etaIndex);

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // Get the indices
  // NB. phiIndex() and strip() are equivalent.
  /// Get eta index.
  int etaIndex() const;
  /// Get phi index. Equivalent to strip().
  int phiIndex() const;
  /// Get strip number. Equivalent to phiIndex().
  int strip() const;

  /// Test if its in a valid state
  bool isValid() const;

  /// Test equality.
  bool operator==(const SiCellId & other) const;
  /// Test inequality.
  bool operator!=(const SiCellId & other) const;
  
  /// Operator for sorting
  bool operator<(const SiCellId & other) const;

  unsigned int word() const { return m_word; } 
  
private:
  unsigned int m_word;

  // Masks and shift values.
  // bits 0 - 15  : phi_index
  // bits 16 - 30 : eta_index
  // bit  31      : valid flag
  enum MaskAndShiftConstants {
    PHI_MASK  = 0x0000ffff,
    PHI_NEG   = 0x00008000, //  (PHI_MASK >> 1 + 1)
    ETA_MASK  = 0x00007fff,
    ETA_SHIFT = 16,
    ETA_NEG   = 0x40000000, // ((ETA_MASK >> 1 + 1) << ETA_SHIFT) 
    VALID     = 0x80000000
  };

};

std::ostream & operator << (std::ostream & os, const SiCellId & cellId);

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline SiCellId::SiCellId()
{
  // set in invalid state
  // This sets the invalid bit plus set phi and eta index to there most negative value
  m_word = VALID | ETA_NEG | PHI_NEG ;
}

inline SiCellId::SiCellId(int phiIndex, int etaIndex)
{
  // The negative check is not really necessary if negative numbers are represented as twos complement but
  // I don't think that assumption is guaranteed to be portable.
  if (phiIndex < 0) phiIndex = PHI_MASK + phiIndex + 1; // For negative number store as two's complement
  if (etaIndex < 0) etaIndex = ETA_MASK + etaIndex + 1;
  m_word = (phiIndex  & PHI_MASK) | ((etaIndex & ETA_MASK) << ETA_SHIFT);
}

inline SiCellId::SiCellId(int strip)
{
  if (strip < 0) strip = PHI_MASK + strip + 1;
  m_word = strip & PHI_MASK;
}

inline int SiCellId::etaIndex() const
{
  if (m_word & ETA_NEG) { // Negative index
    return ((m_word >> ETA_SHIFT) & ETA_MASK) - ETA_MASK - 1;
  } 
  return (m_word >> ETA_SHIFT) & ETA_MASK;
}

inline int SiCellId::phiIndex() const
{
  if (m_word & PHI_NEG) { // Negative index
    return (m_word & PHI_MASK) - PHI_MASK - 1;
  } 
  return (m_word & PHI_MASK);

}

inline int SiCellId::strip() const
{
  return phiIndex();
}

inline bool SiCellId::isValid() const
{
  return !(m_word & VALID);
}

inline bool SiCellId::operator==(const SiCellId & other) const
{
  return (m_word == other.m_word);
} 

inline bool SiCellId::operator!=(const SiCellId & other) const
{
  return (m_word != other.m_word);
} 

inline bool SiCellId::operator<(const SiCellId & other) const
{
  return (m_word < other.m_word);
} 


} // namespace InDetDD


#endif // INDETREADOUTGEOMETRY_SICELLID_H
