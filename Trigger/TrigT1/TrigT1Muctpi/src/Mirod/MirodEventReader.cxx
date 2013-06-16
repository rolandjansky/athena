/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodEventReader.cxx 441850 2011-06-06 14:50:52Z krasznaa $

// STL include(s):
#include <cassert>
#include <string>
#include <sstream>
#include <iomanip>

// Local include(s):
#include "MirodEventReader.h"
#include "MirodExtractor.h"
#include "../Mibak/MibakStreamEvent.h"
#include "../Common/BitOp.h"
#include "../Mioct/MioctID.h"
#include "../Common/MuctpiBitMasks.h"

/*******************************************************************
 * implementation of class MirodEventReader
 * @author   Author: Thorsten Wengler
 * @version $Revision: 441850 $
 ******************************************************************/

namespace LVL1MUCTPI {

   // constructor
   MirodEventReader::MirodEventReader()
      : m_logger( "MirodEventReader" ) {

      REPORT_VERBOSE_MSG( "constructor called" );
   }

   void MirodEventReader::readData( const MibakStreamEvent* theStream ) {

      REPORT_VERBOSE_MSG( "reading event from MIBAK" );

      unsigned int theMioctMNBRMask = 0;
      Hemisphere theMioctHem = NEGATIVE;
      unsigned int eventID = 0;
      unsigned int bunchID = 0;
      unsigned int bunchIDandECRC = 0;
      bool eventIDmismatch = false;
      bool bunchIDmismatch = false;
      unsigned int triggerType = 0;
      unsigned int monitorBit = 0;
      bool isMictp = false;
      unsigned int moduleCounter = 0;
      unsigned int sectorAddress = 0;

      // make sure the FiFo's are empty
      m_eventFiFo.clear();
      m_candidateFiFo.clear();
      m_candidateFiFoHighBits.clear();

      std::list<unsigned int>::const_iterator it_high = theStream->highBits.begin();
      std::list<unsigned int>::const_iterator it_data = theStream->dataWords.begin();
      // start loop over input data stream
      bool mictpFirstDataWord = true;
      for( ; it_high != theStream->highBits.end(); ++it_high ) {

         // -----------------------------------------------------------
         if ( ( *it_high & 0xf ) == 0x0000000e ) {            // module start word

            if ( moduleCounter == 0 ) {       //MICTP is first
               isMictp = true;
               eventID = BitOp::getValue( &( *it_data ), MictpEvIDMask );
            } else {                         //the others are the MIOCTs
               isMictp = false;
               if ( BitOp::getValue( &( *it_data ), MioctEvIDMask ) != eventID ) {
                  eventIDmismatch = true;
               }
               if ( BitOp::getValue( &( *it_data ), MioctBcIDMask ) != bunchID ) {
                  bunchIDmismatch = true;
               }
               theMioctMNBRMask = BitOp::getValue( &( *it_data ), MioctMNBRMask );
               theMioctHem = NEGATIVE;
               if ( theMioctMNBRMask > 7 ) {
                  theMioctMNBRMask = theMioctMNBRMask - 8;
                  theMioctHem = POSITIVE;
               }
            }
            ++moduleCounter;               // increase module counter
         } // ---------------------------------------------------------
         else if ( ( *it_high & 0xf ) == 0x0000000f ) {      // module end word

         } // ---------------------------------------------------------
         else {                                    // data words

            if ( isMictp ) {                  // MICTP
               if ( mictpFirstDataWord ) {     // MICTP first data word
                  mictpFirstDataWord = false;
                  bunchID = BitOp::getValue( &( *it_data ), MictpBcIDMask );
                  bunchIDandECRC = BitOp::getValue( &( *it_data ), MictpECRCBcIDMask );
                  triggerType = BitOp::getValue( &( *it_data ), MictpTrTypeMask );
                  monitorBit = BitOp::getValue( &( *it_data ), MictpMonitorMask );
               } else {                      // MICTP further data words
                  m_candidateFiFo.push_back( *it_data );
                  sectorAddress = 0x00000100;
                  m_candidateFiFoHighBits.push_back( sectorAddress );
               }
            } else {                       // The MIOCT data words
               m_candidateFiFo.push_back( *it_data );
               MioctID theMioctID( theMioctMNBRMask, theMioctHem );
               sectorAddress = theMioctID.getMirodSecAddress( *it_high & 0xf );
               // The candidate suppressions bits are now sitting on bits 41 and 42:
               m_candidateFiFoHighBits.push_back( sectorAddress |
                                                  ( ( *it_high & 0x30 ) << 5 ) );
            }

         } // -----------------------------------------------------------

         ++it_data; // increase also the data word iterator
      } // end loop over input data stream

      // Now fill the EventFifo with the information obtained from the
      // MICTP, the internally determined status flag and the word count

      m_eventFiFo.push_back( eventID - 1 ); // The EventReader is the one that
                                            // decreases the EvID (L1A) by one,
                                            // as the official counting starts
                                            // with 0.
      m_eventFiFo.push_back( bunchIDandECRC );

      unsigned int thirdWord = 0 ;
      BitOp::sImposeNBits( &thirdWord, 13,
                           static_cast< unsigned int >( m_candidateFiFoHighBits.size() ) ) ;
      if ( monitorBit != 0 ) { BitOp::setBit( &thirdWord, 12 ); }
      BitOp::sImposeNBits( &thirdWord, 4, triggerType ) ;
      if ( bunchIDmismatch ) { BitOp::setBit( &thirdWord, 1 ); }
      if ( eventIDmismatch ) { BitOp::setBit( &thirdWord, 0 ); }

      m_eventFiFo.push_back( thirdWord );

      return;
   }

   // method to print the event
   void MirodEventReader::printEvent( std::string format ) const {

      MirodExtractor serviceExtractor; // not the real Extractor, just here
                                       // to use some of its services
      unsigned int wordCount = 0;

      // first print the Event Fifo
      std::list<unsigned int>::const_iterator it_EvFifo = m_eventFiFo.begin();
      for( ; it_EvFifo != m_eventFiFo.end(); ++it_EvFifo ) {

         std::ostringstream message;

         if ( format == "hex" ) {
            message << std::hex << std::setw( 6 ) << ( *it_EvFifo );
         } else if ( format == "bin" ) {
            std::string binaryString;
            BitOp::printBinN( ( *it_EvFifo ), 23, binaryString );
            message << binaryString;
         } else if ( format == "human" ) {
            wordCount++;
            if ( wordCount == 1 ) {   // EvID
               message << " =================================================="
                       << "=========================" << std::endl;
               message << "  EvCaFiFo   EvID: " << std::dec << std::setw( 8 )
                       << ( ( *it_EvFifo ) & EvReEvIDMask );
            }
            if ( wordCount == 2 ) {   // BcID
               message << "             BcID: " << std::setw( 4 )
                       << ( ( *it_EvFifo ) & EvReBcIDMask );
            }
            if ( wordCount == 3 ) {
               message << "             NoWords: " << std::setw( 4 )  // #data words
                       << BitOp::getValue( &( *it_EvFifo ), EvReNoWordsMask );
               message << "   Monitor: "                    // Monitor bit
                       << BitOp::getValue( &( *it_EvFifo ), EvReMonitorMask );
               message << "   TrType: "                    // Trigger Type
                       << BitOp::getValue( &( *it_EvFifo ), EvReTrTypeMask );
               message << "   Status: ";                   // Status bits
               std::string statusString;
               BitOp::printBinN( ( *it_EvFifo ), 3, statusString );
               message << statusString << std::endl;
               message << " --------------------------------------------------"
                       << "-------------------------";
            }
         } else {
            REPORT_FATAL_MSG( "Non-existent print method choosen" );
            assert( 0 );
         }

         REPORT_VERBOSE_MSG( message.str() );

      }

      // and now the candidate FiFo
      std::list<unsigned int>::const_iterator it_Canhigh = m_candidateFiFoHighBits.begin();
      std::list<unsigned int>::const_iterator it_CanData = m_candidateFiFo.begin();
      for( ; it_CanData != m_candidateFiFo.end(); ++it_CanData ) {

         std::ostringstream message;

         if ( format == "hex" ) {
            message << std::hex << std::setw( 3 ) << ( *it_Canhigh );
            message << std::hex << std::setw( 8 ) << ( *it_CanData );
         } else if ( format == "bin" ) {
            std::string binaryString;
            BitOp::printBinN( ( *it_Canhigh ), 8, binaryString ) ;
            message << binaryString << " -- ";
            binaryString = "";
            BitOp::printBin( ( *it_CanData ), binaryString ) ;
            message << binaryString;
         } else if ( format == "human" ) {
            if ( ( ( *it_Canhigh ) & 0x00000100 ) != 0 ) {         // MICTP word
               message << "  MICTP -> BcID: " << BitOp::getValue( &( *it_CanData ), BCIDMask );
               message << "  Sum6: " << BitOp::getValue( &( *it_CanData ), ExtrSum6Mask );
               message << "  Sum5: " << BitOp::getValue( &( *it_CanData ), ExtrSum5Mask );
               message << "  Sum4: " << BitOp::getValue( &( *it_CanData ), ExtrSum4Mask );
               message << "  Sum3: " << BitOp::getValue( &( *it_CanData ), ExtrSum3Mask );
               message << "  Sum2: " << BitOp::getValue( &( *it_CanData ), ExtrSum2Mask );
               message << "  Sum1: " << BitOp::getValue( &( *it_CanData ), ExtrSum1Mask );
               message << std::endl;
               message << " --------------------------------------------------"
                       << "-------------------------";
            } else {                                             // MIOCT word
               message << "  0x" << std::hex << std::setw( 3 ) << std::setfill( '0' )
                       << *it_Canhigh << std::hex << std::setw( 8 ) << std::setfill( '0' )
                       << *it_CanData;
               unsigned int firstCand  = serviceExtractor.extractFirstCandidate( ( *it_CanData ),
                                                                                 ( *it_Canhigh ) );
               unsigned int secondCand = serviceExtractor.extractSecondCandidate( ( *it_CanData ),
                                                                                  ( *it_Canhigh ) );
               if ( firstCand != 0 ) {
                  message << MirodExtractor::printSector( firstCand );
               } else {
                  message << " ... no first candidate ... ";
               }
               if ( secondCand != 0 ) {
                  message << MirodExtractor::printSector( secondCand );
               } else {
                  message << " ... no second candidate ...";
               }
            }
         } else {
            REPORT_FATAL_MSG( "Non-existent print method choosen" );
            assert( 0 );
         }

         REPORT_VERBOSE_MSG( message.str() );

         ++it_Canhigh ; // also increase counter for highBits list
      }
      if ( format == "human" ) {
         REPORT_VERBOSE_MSG( " ===================================================="
                             "=======================" );
      }

      return;
   }

} // namespace LVL1MUCTPI
