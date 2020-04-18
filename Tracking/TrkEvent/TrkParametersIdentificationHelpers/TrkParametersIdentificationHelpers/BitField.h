/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_BITFIELD_H
#define TRK_BITFIELD_H

namespace Trk {

  /** 
      A class managing bits belonging to a range of bits 
   */
  template<class T>
  class BitField {
  public:
    /** constructor, taking the offset (position of the first bit) and the number of bits 
        No range checking is performed so the caller has to ensure that the field does not 
        fall outside the range of the memory of class T
        Class T must support bit operations
     */
    BitField( unsigned int offset_,unsigned int bits_ );

    /** encode a value into id, return false if the value is out of range */
    bool encode( unsigned int value, T& id ) const;

    /** returns the result of decode the input id  */
    unsigned int decode( T id ) const;

  private:
    unsigned int m_offset;    /// position of the first bit manipulated by the BitField
    unsigned int m_bits;      /// number of m_bits that the BitField is allowed to manipulate
    unsigned int m_maxValue;  /// maximum allow value 
    unsigned int m_mask;      /// bit m_mask for the allowed range
  };
  
  template< class T>
  bool BitField<T>::encode( unsigned int value, T& id ) const {
    // check that the value is in range
    if( value >= m_maxValue ) { return false;}
    
    // clear m_bits
    id &= ~m_mask;
    
    // set m_bits
    id |= (value<<m_offset);
    
    return true;
  }

  template< class T>
  unsigned int BitField<T>::decode( T id ) const {
    // apply m_mask and shift with m_offset
    return (id & m_mask) >> m_offset;
  }

  template< class T>
  BitField<T>::BitField( unsigned int offset_,unsigned int bits_ ) : m_offset(offset_),m_bits(bits_) {
    // calculate m_mask
    m_mask = 0;
    for( unsigned int bit = m_offset; bit < m_offset + m_bits; ++bit ) {
      m_mask |= (1<<bit);
    }
    // silly way of calculating 2^m_bits
    m_maxValue = 1;
    for( unsigned int i=0;i<m_bits;++i ) { m_maxValue *= 2;}
  }
}

#endif
