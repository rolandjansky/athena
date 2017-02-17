/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BitOp.cxx 796872 2017-02-13 15:03:25Z fwinkl $

// STL include(s):
#include <iostream>

// Local include(s):
#include "BitOp.h"

namespace LVL1MUCTPI {

  BitOp::BitOp() {

  }

  BitOp::~BitOp() {

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
