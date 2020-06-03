/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <algorithm>
#include <sstream>

#include "TrigT1Interfaces/BitOp.h"

/*******************************************************************
 * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $ 
 *
 * Implementation of class BitOp
 * @author   Author: Thorsten Wengler
 * @version $Revision: 187728 $
 ******************************************************************/

void BitOp::printBinN( unsigned int uintValue, int nbits ) {

  std::string resultString( "" );
  for ( int i = nbits; i >= 0; i-- ) {
    if ( uintValue & ( 1 << i ) ) {
      resultString += "1";
    } else {
      resultString += "0";
    }
    if ( ( i % 4 ) == 0 ) resultString += " ";
  }

}

void BitOp::printBinN( int intValue, int nbits ) {

  std::string resultString( "" );
  for ( int i = nbits; i >= 0; i-- ) {
    if ( intValue & ( 1 << i ) ) {
      resultString += "1";
    } else {
      resultString += "0";
    }
    if ( ( i % 4 ) == 0 ) resultString += " ";
  }

}

unsigned int BitOp::getValue( const unsigned int * uintValue,
                              const unsigned int mask ) {

  unsigned int result;
  unsigned int maskcopy;
  // make a copy of the mask, because a mask is a mask and
  // one should stay a mask (i.e. should be something constant!)
  maskcopy = mask;
  result = *uintValue & mask;
  if ( mask != 0 ) {
    while ( ( maskcopy & 0x00000001 ) == 0 ) {
      maskcopy = maskcopy >> 1;
      result = result >> 1;
    }
  }

  return result;

}

unsigned int BitOp::alignBits( const int value, const int startbit, const int endbit )
{
  int ostart = std::min( startbit, endbit );
  int oend   = std::max( startbit, endbit );
  unsigned int start  = std::max( ostart, 0 );
  unsigned int end    = std::min( oend, 32 );

  unsigned int result = value;
  if ( ostart < 0 ) {
    result >>= std::abs( ostart );
  } else {
    result <<= ostart;
  }
  result &= createMask( start, end );

  return result;
}

unsigned int BitOp::createMask( const int startbit,  const int endbit )
{
  unsigned int start = std::min( std::max( startbit,  0 ), std::max( endbit,  0 ) );
  unsigned int end   = std::max( std::min( startbit, 32 ), std::min( endbit, 32 ) );

  unsigned int result = 0;
  for( unsigned int i = 0; i <= ( end - start ); ++i ) {
    result <<= 1;
    result |= 1;
  }
  result <<= start;

  return result;
}

const std::string BitOp::printBits( const int value, const int startbit, const int endbit )
{
  int ostart = std::min( startbit, endbit );
  int oend   = std::max( startbit, endbit );
  unsigned int start  = std::max( ostart, 0 );
  unsigned int end    = std::min( oend, 32 );

  std::ostringstream s;

  for( unsigned int i = start; i <= end; ++i ) {
    if ( isSet(&value, i) ) {
      s << "1";
    } else {
      s << "0";
    }
  }

  return s.str();
}

