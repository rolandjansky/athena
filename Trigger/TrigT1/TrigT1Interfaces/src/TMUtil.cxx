/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <cmath>

#include "xercesc/util/XMLString.hpp"

#include "TrigT1Interfaces/TMUtil.h"

namespace LVL1CTP {

  int TMUtil::getEMThresholdMult( const unsigned int word,
                                        const int threshold, const int length ) {

    int returnValue;
    const int nbit( 32 );
    std::string bitPattern( intToBits( word, nbit ) );
    // bit  01234567890123456789012345678901
    // info xxxxxxP888777666555444333222111x
    bitPattern = bitPattern.substr( 7 + ( 8 - threshold ) * length, length );
    returnValue = static_cast<int>( uintFromString( bitPattern ) );
    return returnValue;

  }

  int TMUtil::getMuonThresholdMult( const unsigned int word,
                                          const int threshold, const int length ) {

    int returnValue;
    const int nbit( 32 );
    std::string bitPattern( intToBits( word, nbit ) );
    // bit  01234567890123456789012345678901
    // info xxxxxxxxxxxxxx666555444333222111
    bitPattern = bitPattern.substr( 14 + ( 6 - threshold ) * length, length );
    returnValue = static_cast<int>( uintFromString( bitPattern ) );
    return returnValue;

  }

  int TMUtil::getEnergyThreshold( const unsigned int word,
                                        const int threshold, const int offSet,
                                        const int number ) {

    int returnValue;
    const int nbit( 32 );
    std::string bitPattern( intToBits( word, nbit ) );
    // bit  01234567890123456789012345678901
    // info xxxxxxP888777666555444333222111x
    bitPattern = bitPattern.substr( ( offSet + ( number - threshold ) ), 1 );
    returnValue = static_cast<int>( uintFromString( bitPattern ) );
    return returnValue;

  }

  //const std::string TMUtil::intToBits(const int i,const int nbit) const
  std::string TMUtil::intToBits( unsigned int i, int nbit ) {

    std::string bitString = "";
    std::string ONE = "1";
    std::string ZERO = "0";
    if ( i > ( pow( 2, 31 ) - 1 ) ) {
      bitString += "1";
      nbit--;
      i -= static_cast<unsigned int>( pow( 2, 31 ) );
    }
    for ( int j = nbit - 1;j >= 0;j-- )
      if ( pow( 2, j ) <= i ) {
        i -= ( int ) pow( 2, j );
        bitString += ONE;
      } else
        bitString += ZERO;
    return bitString;

  }

  int TMUtil::powerOfInt( const int i ) {
    int out;
    if ( i < 0 || i > 9 ) {
      exit( i );
    } else {
      out = 1;
      for ( int j = 0;j < i;j++ )
        out *= 10;
    }
    return out;
  }

  int TMUtil::DOMToInt( const XMLCh * thisDOMString ) {

    int returnValue( 0 );
    int sign( 1 );
    std::string helpString( TMUtil::utilTranscode( thisDOMString ) );
    if ( helpString.substr( 0, 1 ) == "-" ) {
      sign = -1;
      helpString = helpString.substr( 1, std::string::npos );
    }

    std::istringstream is( helpString );
    //   istrstream is(helpString.c_str(),helpString.size());
    // I think this ^^^ is a gcc32 fix. Don't have time to test it now. EJWM.
    is >> returnValue;

    return sign*returnValue;

  }

  std::string TMUtil::utilTranscode( const XMLCh * t ) {

    char* cstr = xercesc::XMLString::transcode( t );
    std::string str = cstr;
    xercesc::XMLString::release( &cstr );
    return str;

  }

  unsigned int TMUtil::uintFromString( std::string pattern ) {

    int size = pattern.size() - 1;
    unsigned int result = 0;
    for ( unsigned int i = 0;i < pattern.size();i++ ) {
      if ( pattern[ i ] == '1' )
        result += static_cast<int>( pow( 2, size ) );
      size--;
    }

    return result;

  }

  bool TMUtil::compareNodeNameString( const xercesc::DOMNode * domNode, const std::string & string2 ) {

    char* nodename = xercesc::XMLString::transcode( domNode->getNodeName() );
    bool result = ( 0 == xercesc::XMLString::compareString( nodename, string2.c_str() ) );
    xercesc::XMLString::release( &nodename );
    return result;

  }

  float TMUtil::getGevValue( const XMLCh* xmlStr ) {

    // convert XMLCh* to string
    const std::string str = utilTranscode( xmlStr );

    // look for "GeV"
    if ( std::string::npos == str.find( "GeV" ) )
      return 0;

    // look for "*"
    std::string::size_type idx = str.find( "*" );
    if ( std::string::npos == idx )
      return 0;

    // convert to float
    std::istringstream is( str.substr( 0, idx ) );
    float ret;
    is >> ret;

    return ret;

  }

} // namespace LVL1CTP
