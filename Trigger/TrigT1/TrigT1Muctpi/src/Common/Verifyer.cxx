/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Verifyer.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// STL include(s):
#include <iostream>

// Local include(s):
#include "Verifyer.h"

/*
 *******************************************************************
 * $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
 *
 * Implementation of class Verifyer
 * @author   Author: Thorsten Wengler
 * @version $Revision: 362102 $
 ******************************************************************/

namespace LVL1MUCTPI {

  // constructor of class Verifyer
  Verifyer::Verifyer()
    : m_lvl2CandidateMask( 0 ), m_daqMictpMask( 0 ), m_daqCandidateMask( 0 ) {

    for( int i = 0 ; i < 8 ; ++i ) {
      m_lvl2HeaderMask.push_back( 0 );
      m_daqHeaderMask.push_back( 0 );
      if ( i < 5 ) {
        m_lvl2TrailerMask.push_back( 0 );
        m_daqTrailerMask.push_back( 0 );
      }
    }

  }

  // Compare Daq Out simulation/hardware
  unsigned int Verifyer::daqOutIsEqual( const std::list< unsigned int >& simOut, const std::list< unsigned int >& hardwareOut,
                                        bool printToScreen ) const {

    unsigned int areEqual = 0;

    // bit 0 of return code = same buffer length
    if( simOut.size() != hardwareOut.size() ) areEqual |= 1;
    unsigned int bufferSize = simOut.size();

    // iterate over the list and check the sector ID's
    std::list<unsigned int>::const_iterator it_sim;
    std::list<unsigned int>::const_iterator it_hard;
    it_hard = hardwareOut.begin();

    unsigned int wordCount = 0 ;
    unsigned int headerCount = 0 ;
    unsigned int trailerCount = 0 ;
    unsigned int candidateCount = 0 ;
    for( it_sim = simOut.begin(); it_sim != simOut.end(); ++it_sim ) {

      ++wordCount;

      if( headerCount < 8 ) {
        ++headerCount;
        // bit number 2-9: differences in the header words
        unsigned int bitTest = 0;
        if ( ( bitTest = ( ( *it_sim ) ^ ( *it_hard ) ) ) != 0 ) {
          if ( ( bitTest & ( bitTest ^ this->daqHeaderMask( headerCount - 1 ) ) ) != 0 ) {
            areEqual |= ( 1 << ( headerCount ) );
          }
        }
      } else if ( wordCount == 9 ) {
        // bit number 19: differences in the MICTP word
        unsigned int bitTest = 0;
        if ( ( bitTest = ( ( *it_sim ) ^ ( *it_hard ) ) ) != 0 ) {
          if ( ( bitTest & ( bitTest ^ this->daqMictpMask() ) ) != 0 ) {
            areEqual |= ( 1 << ( 19 ) );
          }
        }
      } else if ( wordCount >= ( bufferSize - 4 ) ) {
        trailerCount++;
        // bit number 10-14: differences in the trailer words
        unsigned int bitTest = 0;
        if ( ( bitTest = ( ( *it_sim ) ^ ( *it_hard ) ) ) != 0 ) {
          if ( ( bitTest & ( bitTest ^
                             this->daqTrailerMask( trailerCount - 1 ) ) ) != 0 ) {
            areEqual |= ( 1 << ( trailerCount ) );
          }
        }
      } else {
        candidateCount++;
        // bit number 15: differences in a candidate word
        unsigned int bitTest = 0;
        if ( ( bitTest = ( ( *it_sim ) ^ ( *it_hard ) ) ) != 0 ) {
          if ( ( bitTest & ( bitTest ^ this->daqCandidateMask() ) ) != 0 ) {
            areEqual |= ( 1 << ( 20 ) );
          }
        }
      }
      if ( printToScreen ) {
        std::cout << std::hex << ( *it_sim ) << std::endl;
        std::cout << std::hex << ( *it_hard ) << std::endl << std::endl;
      }

      ++it_hard;
    }

    return areEqual;

  }

  // Compare Lvl2 Out simulation/hardware
  unsigned int Verifyer::lvl2OutIsEqual( const std::list< unsigned int >& simOut, const std::list< unsigned int >& hardwareOut,
                                         bool printToScreen ) const {

    unsigned int areEqual = 0;

    // bit 0 of return code = same buffer length
    if( simOut.size() != hardwareOut.size() ) areEqual |= 1;
    unsigned int bufferSize = simOut.size();

    // iterate over the list and check the sector ID's
    std::list< unsigned int >::const_iterator it_sim;
    std::list< unsigned int >::const_iterator it_hard;
    it_hard = hardwareOut.begin();

    unsigned int wordCount = 0;
    unsigned int headerCount = 0;
    unsigned int trailerCount = 0;
    unsigned int candidateCount = 0;
    for ( it_sim = simOut.begin(); it_sim != simOut.end(); ++it_sim ) {

      ++wordCount;

      if( headerCount < 8 ) {
        ++headerCount;
        // bit number 2-9: differences in the header words
        unsigned int bitTest = 0;
        if ( ( bitTest = ( ( *it_sim ) ^ ( *it_hard ) ) ) != 0 ) {
          if ( ( bitTest & ( bitTest ^
                             this->lvl2HeaderMask( headerCount - 1 ) ) ) != 0 ) {
            areEqual |= ( 1 << ( headerCount ) );
          }
        }
      } else if ( wordCount >= ( bufferSize - 4 ) ) {
        trailerCount++;
        // bit number 10-14: differences in the trailer words
        unsigned int bitTest = 0;
        if ( ( bitTest = ( ( *it_sim ) ^ ( *it_hard ) ) ) != 0 ) {
          if ( ( bitTest & ( bitTest ^
                             this->lvl2TrailerMask( trailerCount - 1 ) ) ) != 0 ) {
            areEqual |= ( 1 << ( trailerCount ) );
          }
        }
      } else {
        candidateCount++;
        // bit number 15: differences in a candidate word
        unsigned int bitTest = 0;
        if ( ( bitTest = ( ( *it_sim ) ^ ( *it_hard ) ) ) != 0 ) {
          if ( ( bitTest & ( bitTest ^ this->lvl2CandidateMask() ) ) != 0 ) {
            areEqual |= ( 1 << ( 20 ) );
          }
        }
      }
      if ( printToScreen ) {
        std::cout << std::hex << ( *it_sim ) << std::endl;
        std::cout << std::hex << ( *it_hard ) << std::endl << std::endl;
      }

      ++it_hard;
    }

    return areEqual;

  }

  // compare the MirodFiFos
  bool Verifyer::mirodFiFosAreEqual( MirodFiFoContent & mirodFiFos, MirodFiFoContent & simMirodFiFos ) const {

    if( lvl2OutIsEqual( simMirodFiFos.lvl2FiFo(), mirodFiFos.lvl2FiFo(), false ) == 0 &&
        daqOutIsEqual( simMirodFiFos.daqFiFo(), mirodFiFos.daqFiFo(), false ) == 0 ) {
      return true;
    }

    return false;
  }


  //destructor of class Verifyer
  Verifyer::~Verifyer() {

  }

} // namespace LVL1MUCTPI
