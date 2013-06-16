/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BitOp.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// STL include(s):
#include <iostream>

// Local include(s):
#include "BitOp.h"

namespace LVL1MUCTPI {

  BitOp::BitOp() {

  }

  BitOp::~BitOp() {

  }

  unsigned int BitOp::getValue( const unsigned int * uintValue, const unsigned int mask ) {

    unsigned int result;
    unsigned int maskcopy;
    // make a copy of the mask, because a mask is a mask and
    // one should stay a mask (i.e. should be something constant!)
    maskcopy = mask;
    result   = *uintValue & mask;
    if ( mask != 0 ) {
      while ( (maskcopy & 0x00000001) == 0) {
        maskcopy =  maskcopy >> 1;
        result   =  result   >> 1;
      }
    }

    return result;
  }

  void BitOp::printBinN( unsigned int uintValue, int nbits ) {

    for( int i = nbits; i >= 0; --i ) {
      if( uintValue & ( 1 << i ) ) {
        std::cout << "1";
      } else {
        std::cout << "0";
      }
      if( (i % 4) == 0 ) std::cout << " ";
    }

  }

  void BitOp::printBinN( int intValue, int nbits ) {

    for( int i = nbits; i >= 0; --i ) {
      if( intValue & ( 1 << i ) ) {
        std::cout << "1";
      } else {
        std::cout << "0";
      }
      if( (i % 4) == 0 ) std::cout << " ";
    }

  }

  void BitOp::printBinN( unsigned int uintValue, int nbits, std::string& destination ) {

    for( int i = nbits; i >= 0; --i ) {
      if( uintValue & ( 1 << i ) ) {
        destination += "1";
      } else {
        destination += "0";
      }
      if( (i % 4) == 0 ) destination += " ";
    }

  }

  void BitOp::printBinN( int intValue, int nbits, std::string& destination ) {

    for( int i = nbits; i >= 0; --i ) {
      if( intValue & ( 1 << i ) ) {
        destination += "1";
      } else {
        destination += "0";
      }
      if( (i % 4) == 0 ) destination += " ";
    }

  }

} // namespace LVL1MUCTPI
