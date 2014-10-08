/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    unsigned int offset;    /// position of the first bit manipulated by the BitField
    unsigned int bits;      /// number of bits that the BitField is allowed to manipulate
    unsigned int maxValue;  /// maximum allow value 
    unsigned int mask;      /// bit mask for the allowed range
  };
  
  template< class T>
  bool BitField<T>::encode( unsigned int value, T& id ) const {
    // check that the value is in range
    if( value >= maxValue ) return false;
    
    // clear bits
    id &= ~mask;
    
    // set bits
    id |= (value<<offset);
    
    return true;
  }

  template< class T>
  unsigned int BitField<T>::decode( T id ) const {
    // apply mask and shift with offset
    return (id & mask) >> offset;
  }

  template< class T>
  BitField<T>::BitField( unsigned int offset_,unsigned int bits_ ) : offset(offset_),bits(bits_) {
    // calculate mask
    mask = 0;
    for( unsigned int bit = offset; bit < offset + bits; ++bit ) {
      mask |= (1<<bit);
    }
    // silly way of calculating 2^bits
    maxValue = 1;
    for( unsigned int i=0;i<bits;++i ) maxValue *= 2;
  }
}

#endif
