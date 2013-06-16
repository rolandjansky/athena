/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MibakStreamEvent.cxx 364083 2011-05-06 09:09:55Z krasznaa $

// STL include(s):
#include <cassert>
#include <iomanip>
#include <fstream>
#include <sstream>

// Local include(s):
#include "MibakStreamEvent.h"
#include "../Mirod/MirodExtractor.h"
#include "../Common/BitOp.h"
#include "../Common/MuctpiBitMasks.h"
#include "../Mirod/MirodTestRamIn.h"

/*******************************************************************
 * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
 *
 * Implementation of class MibakStreamEvent
 * @author   Author: Thorsten Wengler
 * @version $Revision: 364083 $
 ******************************************************************/

namespace LVL1MUCTPI {

   // the constructor
   MibakStreamEvent::MibakStreamEvent()
      : m_logger( "MibakStreamEvent" ) {

   }

   // method to print the event
   void MibakStreamEvent::printEvent( const std::string& format ) const {

      unsigned int wordCount = 0;
      unsigned int moduleCount = 0;
      std::list< unsigned int >::const_iterator it_high = highBits.begin();
      std::list< unsigned int >::const_iterator it_data = dataWords.begin();
      for( ; it_data != dataWords.end(); ++it_data ) {

         std::ostringstream message;

         if( format == "hex" ) {
            message << std::hex << ( *it_high ) << std::setw( 8 ) << ( *it_data );
         } else if( format == "bin" ) {

            std::string printout;
            BitOp::printBinN( ( *it_high ), 3, printout );
            BitOp::printBin( ( *it_data ), printout ) ;
            message << printout;

         } else if( format == "human" ) {

            if( ( *it_high ) == MioctStartMask ) {     // Module start

               ++moduleCount;
               wordCount = 0;
               if( moduleCount == 1 ) {           // MICTP header
                  message << " =================================================="
                          << "=========================" << std::endl;
                  message << " MiBaK | MICTP Header -> ID: "
                          << BitOp::getValue( &( *it_data ), MictpIDPositionMask );
                  message << "  EvID: " << std::dec << std::setw( 8 )
                          << BitOp::getValue( &( *it_data ), MictpEvIDMask );
               } else {                            // MIOCT header
                  message << " --------------------------------------------------"
                          << "-------------------------" << std::endl;
                  message << "  MIOCT Header -> MNBR: " << std::setw( 2 )
                          << BitOp::getValue( &( *it_data ), MioctMNBRMask );
                  message << "    EvID: " << std::dec << std::setw( 8 )
                          << BitOp::getValue( &( *it_data ), MioctEvIDMask );
                  message << "    BcID: " << std::dec << std::setw( 4 )
                          << BitOp::getValue( &( *it_data ), MioctBcIDMask );
                  message << std::endl;
               }

            } else if( ( *it_high ) == MioctEndMask ) { // Module End

               if( moduleCount == 1 ) {           // MICTP trailer
                  message << "  MICTP Trailer -> ";
                  message << "  Sum6: " << BitOp::getValue( &( *it_data ), MictpTrailerSum6Mask );
                  message << "  Sum5: " << BitOp::getValue( &( *it_data ), MictpTrailerSum5Mask );
                  message << "  Sum4: " << BitOp::getValue( &( *it_data ), MictpTrailerSum4Mask );
                  message << "  Sum3: " << BitOp::getValue( &( *it_data ), MictpTrailerSum3Mask );
                  message << "  Sum2: " << BitOp::getValue( &( *it_data ), MictpTrailerSum2Mask );
                  message << "  Sum1: " << BitOp::getValue( &( *it_data ), MictpTrailerSum1Mask );
                  message << std::endl;
                  message << "  MICTP Trailer -> Status: ";
                  std::string messageString;
                  BitOp::printBinN( BitOp::getValue( &( *it_data ), MictpTrailerFlagsMask ), 3, messageString );
                  message << messageString;
                  message << "  WordCount: " << BitOp::getValue( &( *it_data ), MictpTrailerWordsMask );
                  message << std::endl;
               } else {                            // MIOCT trailer
                  message << "  MIOCT Trailer -> ";
                  message << "  Sum6: " << BitOp::getValue( &( *it_data ), MictpTrailerSum6Mask );
                  message << "  Sum5: " << BitOp::getValue( &( *it_data ), MictpTrailerSum5Mask );
                  message << "  Sum4: " << BitOp::getValue( &( *it_data ), MictpTrailerSum4Mask );
                  message << "  Sum3: " << BitOp::getValue( &( *it_data ), MictpTrailerSum3Mask );
                  message << "  Sum2: " << BitOp::getValue( &( *it_data ), MictpTrailerSum2Mask );
                  message << "  Sum1: " << BitOp::getValue( &( *it_data ), MictpTrailerSum1Mask );
                  message << std::endl;
                  message << "  MIOCT Trailer -> Status: ";
                  std::string messageString;
                  BitOp::printBinN( BitOp::getValue( &( *it_data ), MictpTrailerFlagsMask ), 3, messageString );
                  message << messageString;
                  message << "  WordCount: " << BitOp::getValue( &( *it_data ), MictpTrailerWordsMask );
                  message << std::endl;
               }

            } else {                              // Module data

               ++wordCount;
               if( moduleCount == 1 ) {           // MICTP data

                  if( wordCount == 1 ) {            // second MICTP header word

                     message << "  BcID: " << std::dec << std::setw( 4 )
                             << BitOp::getValue( &( *it_data ), MictpBcIDMask );
                     message << "  ECRC: " << std::dec << std::setw( 4 )
                             << BitOp::getValue( &( *it_data ), MictpECRCMask );
                     message << "  Mon: " << std::dec << std::setw( 1 )
                             << BitOp::getValue( &( *it_data ), MictpMonitorMask );
                     message << "  TrTy: " << std::dec << std::setw( 1 )
                             << BitOp::getValue( &( *it_data ), MictpTrTypeMask );
                     message << std::endl;

                  } else {                          // MICTP data word

                     message << "  MICTP ->  BcID: " << BitOp::getValue( &( *it_data ), BCIDMask );
                     message << "  Sum6: " << BitOp::getValue( &( *it_data ), MictpSum6Mask );
                     message << "  Sum5: " << BitOp::getValue( &( *it_data ), MictpSum5Mask );
                     message << "  Sum4: " << BitOp::getValue( &( *it_data ), MictpSum4Mask );
                     message << "  Sum3: " << BitOp::getValue( &( *it_data ), MictpSum3Mask );
                     message << "  Sum2: " << BitOp::getValue( &( *it_data ), MictpSum2Mask );
                     message << "  Sum1: " << BitOp::getValue( &( *it_data ), MictpSum1Mask );
                     message << std::endl;

                  }

               } else {                            // MIOCT data

                  message << "    SNBR: " << std::dec << ( *it_high ) << "  data: ";
                  std::string messageString;
                  BitOp::printBin( *it_data, messageString );
                  message << messageString << std::endl;

               }

               REPORT_VERBOSE_MSG( message.str() );

            }

         } else {
            REPORT_FATAL_MSG( "Non-existent print method choosen" );
            assert( 0 );
         }
         ++it_high; // also increase counter for highBits list
      }
      if ( format == "human" ) {
         REPORT_VERBOSE_MSG( " ===========================================================================" );
      }

      return;
   }

   void MibakStreamEvent::setEvent( const std::list< unsigned int >& inputHighBits,
                                    const std::list< unsigned int >& inputDataWords ) {

      highBits.clear();
      dataWords.clear();

      highBits = inputHighBits;
      dataWords = inputDataWords;

      prepareMirodTestRamInput();

      return;
   }

   void MibakStreamEvent::readFromFile( const std::string& fileName ) {

      unsigned int long long inputWord = 0;
      unsigned int dataWordLow;
      unsigned int dataWordHigh;
      std::ifstream inFile( fileName.c_str() );

      // clear the lists holding the event information
      //    m_internalDataWords.clear();
      highBits.clear();
      dataWords.clear();

      // read the file and fill the lists
      inFile >> std::hex >> inputWord;
      while( ! inFile.eof() ) {

         dataWordLow = inputWord & 0xffffffff;
         dataWordHigh = ( inputWord >> 32 ) & 0x0000000f;

         //      m_internalDataWords.push_back( dataWordLow );
         highBits.push_back( dataWordHigh );
         dataWords.push_back( dataWordLow );
         inFile >> std::hex >> inputWord;
      }

      /*
        list<unsigned int>::const_iterator it_intDaWo ;
        for ( it_intDaWo = m_internalDataWords.begin();
           it_intDaWo != m_internalDataWords.end();
           ++it_intDaWo ) {
      dataWords.push_back( &( *it_intDaWo ) ) ;
    }
    */

      return;
   }

   // get the input for the Mirod test ram
   MirodTestRamIn MibakStreamEvent::getMirodTestRamInput() const {

      MirodTestRamIn testRamInput;
      this->prepareMirodTestRamInput();
      testRamInput.dataLow( m_mirodTestDataLow );
      testRamInput.dataHigh( m_mirodTestDataHigh );
      testRamInput.dataControl( m_mirodTestControl );

      return testRamInput;
   }

   // method to print lists that can be used as input for the MIROD testram
   void MibakStreamEvent::printMirodTestRamInput( const std::string& fileName ) {

      std::list< unsigned int >::const_iterator it_TeDaHi;
      std::list< unsigned int >::const_iterator it_TeDaLo;
      std::list< unsigned int >::const_iterator it_TeCtrl;

      if( fileName == "screen" ) {

         it_TeDaHi = m_mirodTestDataHigh.begin();
         it_TeCtrl = m_mirodTestControl.begin();
         for( it_TeDaLo = m_mirodTestDataLow.begin(); it_TeDaLo != m_mirodTestDataLow.end(); ++it_TeDaLo ) {

            m_logger << INFO << std::hex << std::setw( 3 ) << ( *it_TeDaHi ) << " "
                     << std::setw( 6 ) << ( *it_TeDaLo )
                     << " " << ( *it_TeCtrl ) << MsgLogger::endmsg;

            ++it_TeDaHi;
            ++it_TeCtrl;

         }

      } else {

         const std::string dataFile = fileName + "_data.dat";
         const std::string ctrlFile = fileName + "_ctrl.dat";
         std::ofstream outDataFile( dataFile.c_str() );
         std::ofstream outCtrlFile( ctrlFile.c_str() );

         if( outDataFile.fail() || outCtrlFile.fail() ) {
            REPORT_FATAL_MSG( "Could not open files for MIROD testram" );
            assert( 0 );
         }
         it_TeDaHi = m_mirodTestDataHigh.begin();
         it_TeCtrl = m_mirodTestControl.begin();
         for( it_TeDaLo = m_mirodTestDataLow.begin(); it_TeDaLo != m_mirodTestDataLow.end(); ++it_TeDaLo ) {

            outDataFile << std::hex << std::setw( 3 ) << ( *it_TeDaHi ) << std::setw( 6 )
                        << ( *it_TeDaLo ) << std::endl;
            outCtrlFile << std::hex << std::setw( 2 ) << ( *it_TeCtrl ) << std::endl;
            ++it_TeDaHi;
            ++it_TeCtrl;
         }
      }

      return;
   }

   // method to prepare lists that can be used as input for the MIROD testram
   void MibakStreamEvent::prepareMirodTestRamInput() const {

      m_mirodTestDataLow.clear();
      m_mirodTestDataHigh.clear();
      m_mirodTestControl.clear();

      unsigned int inputDataLow = 0;
      unsigned int inputDataHigh = 0;
      unsigned int controlWord = 0;
      unsigned int wordCount = 0;
      // fill the testData lists from the Mibak Event lists
      // first an empty data word and XDTRDY on
      controlWord = 0x0000000e;                    // enable XDTRDY
      m_mirodTestDataLow.push_back( 0x00000000 );
      m_mirodTestDataHigh.push_back( 0x00000000 );
      m_mirodTestControl.push_back( controlWord );

      // now loop over data words and fill lists
      std::list< unsigned int >::const_iterator it_HiBi = highBits.begin();
      std::list< unsigned int >::const_iterator it_DaWo = dataWords.begin();
      for ( ; it_DaWo != dataWords.end(); ++it_DaWo ) {

         ++wordCount;
         // inputDataHigh consits of bits 0-3 from highBits shifted to
         // 8-11, and 24-31 from dataWords shifted to 0-7
         inputDataHigh = 0;
         inputDataHigh = ( ( *it_HiBi ) << 8 );
         inputDataHigh |= ( ( ( *it_DaWo ) & 0xff000000 ) >> 24 );
         m_mirodTestDataHigh.push_back( inputDataHigh );

         // inputDataLow are bits 0-23 from the dataWords list
         inputDataLow = 0;
         inputDataLow = ( ( *it_DaWo ) & 0x00ffffff );
         m_mirodTestDataLow.push_back( inputDataLow );

         // handle the control word
         // begin module - enable data valid
         if ( ( *it_HiBi ) == 0x0000000e ) {
            BitOp::clearBit( &controlWord, 2 );
         }

         // at third word XDTRDY is disabled
         if( wordCount == 3 ) {
            BitOp::clearBit( &controlWord, 1 );
         }

         // for last word token back must be enabled and disabled again
         if( ( ( *it_HiBi ) == 0x0000000f ) && ( it_DaWo == --( dataWords.end() ) ) ) {
            BitOp::setBit( &controlWord, 0 );         // enable token back
         }

         m_mirodTestControl.push_back( controlWord );

         // end module - add additional empty word with data valid disabled
         if( ( *it_HiBi ) == 0x0000000f ) {
            m_mirodTestDataLow.push_back( 0x00000000 );
            m_mirodTestDataHigh.push_back( 0x00000000 );
            BitOp::clearBit( &controlWord, 0 );               // disable token back
            BitOp::setBit( &controlWord, 2 );
            m_mirodTestControl.push_back( controlWord );
         }

         ++it_HiBi; // also increase iterator for high bits
      }

      // add one more empty word in the end
      controlWord = 0x0000000c;                    // data valid off
      m_mirodTestDataLow.push_back( 0x00000000 );
      m_mirodTestDataHigh.push_back( 0x00000000 );
      m_mirodTestControl.push_back( controlWord );

      return;
   }

} // namespace LVL1MUCTPI
