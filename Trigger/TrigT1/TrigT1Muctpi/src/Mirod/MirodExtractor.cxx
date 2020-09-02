/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodExtractor.cxx 441850 2011-06-06 14:50:52Z krasznaa $

// STL include(s):
#include <cassert>
#include <sstream>
#include <iostream>
#include <iomanip>

// Local include(s):
#include "MirodExtractor.h"
#include "MirodEventReader.h"
#include "../Common/MuctpiBitMasks.h"
#include "../Common/SectorConstants.h"
#include "../Common/Configuration.h"
#include "../Common/BitOp.h"

/*******************************************************************
 * $Date: 2011-06-06 16:50:52 +0200 (Mon, 06 Jun 2011) $
 *
 * Implementation of class MirodExtractor
 * @author   Author: Thorsten Wengler
 * @version $Revision: 441850 $
 ******************************************************************/

namespace LVL1MUCTPI {

   // constructor of class MirodExtractor
   MirodExtractor::MirodExtractor()
      : m_logger( "MirodExtractor" ),
        m_threshold1Candidate( 0 ),  m_threshold2Candidate( 0 ), m_suppressionMode( 0 ) {

   }

   // alternative constructor of class MirodExtractor with setting
   // the output by hand - to use analysis methods
   MirodExtractor::MirodExtractor( std::list< unsigned int > extractorOut )
      : m_logger( "MirodExtractor" ), m_extractorData( std::move(extractorOut) ),
        m_threshold1Candidate( 0 ),  m_threshold2Candidate( 0 ), m_suppressionMode( 0 ) {

   }

   // set Configuration
   void MirodExtractor::setConfiguration( const Configuration &conf ) {

      m_threshold1Candidate = conf.getThreshold1Candidate();
      m_threshold2Candidate = conf.getThreshold2Candidate();
      m_suppressionMode = conf.getSuppressionMode();

      return;
   }

   void MirodExtractor::processData( const MirodEventReader* theReader ) {

      REPORT_VERBOSE_MSG( "reading event from EventReader" );

      unsigned int inputWord = 0;
      m_extractorData.clear();
      bool thresholdFirstFailed = false;
      bool thresholdSecondFailed = false;
      unsigned int candidateCount = 0;

      // get Pointer to the MirodEventReader output data
      const std::list<unsigned int>* theEventFiFo =
         theReader->getEventFifo();
      const std::list<unsigned int>* theCandidateHighBits =
         theReader->getCandidateFifoHighBits();
      const std::list<unsigned int>* theCandidateFiFo =
         theReader->getCandidateFifo();

      // first three words of the extractor format are filled from the
      // EventFiFo
      if ( theEventFiFo->size() != 3 ) {
         REPORT_FATAL_MSG( " <> 3 words in EventFiFo" );
         assert( 0 );
      }
      std::list<unsigned int>::const_iterator it_EvFiFo ;
      it_EvFiFo = theEventFiFo->begin();

      inputWord = ( *it_EvFiFo );                // first word from EventFiFo
      BitOp::setBit( &inputWord, 31 );
      m_extractorData.push_back( inputWord );

      ++it_EvFiFo;
      inputWord = 0;
      inputWord = ( *it_EvFiFo );                // second word from EventFiFo
      BitOp::setBit( &inputWord, 31 );
      m_extractorData.push_back( inputWord );

      ++it_EvFiFo;
      inputWord = 0;
      inputWord = ( *it_EvFiFo );                // third word from EventFiFo
      BitOp::setBit( &inputWord, 31 );
      for( int i = 13;i <= 23;++i ) { BitOp::clearBit( &inputWord, i ); }
      m_extractorData.push_back( inputWord );

      // Now loop over the Candidate Fifo and process the data words
      std::list<unsigned int>::const_iterator it_CaFiFo     = theCandidateFiFo->begin();
      std::list<unsigned int>::const_iterator it_CaFiFoHiBi = theCandidateHighBits->begin();
      for( ; it_CaFiFo != theCandidateFiFo->end(); ++it_CaFiFo ) {

         inputWord = 0 ;
         if ( ( *it_CaFiFoHiBi ) == 0x00000100 ) {    // this is a MICTP word

            inputWord = ( *it_CaFiFo );
            unsigned int tmpBits = ( inputWord & BCIDMask ) >> 9; //shift BCID
            for ( int i = 27; i <= 29; ++i ) { BitOp::clearBit( &inputWord, i ); }
            inputWord |= tmpBits;
            BitOp::setBit( &inputWord, 29 );
            m_extractorData.push_back( inputWord );

         } else {   //  MIOCT word

            //second candidate
            inputWord = 0 ;
            inputWord = ( this )->extractSecondCandidate( ( *it_CaFiFo ), ( *it_CaFiFoHiBi ) );
            if ( inputWord != 0 ) {
               candidateCount++;
               m_extractorData.push_back( inputWord );
               // ...  if the candidate failed its threshold, set the
               //      corresponding global bit
               if ( !( BitOp::isSet( &inputWord, 26 ) ) ) {
                  thresholdSecondFailed = true;
               }
            }
            // first candidate
            inputWord = ( this )->extractFirstCandidate( ( *it_CaFiFo ), ( *it_CaFiFoHiBi ) );
            if ( inputWord != 0 ) {
               candidateCount++;
               m_extractorData.push_back( inputWord );
               // ...  if the candidate failed its threshold, set the
               //      corresponding global bit
               if ( !( BitOp::isSet( &inputWord, 26 ) ) ) {
                  thresholdFirstFailed = true;
               }
            }
         }

         ++it_CaFiFoHiBi;   // also increment the HighBits list
      }

      // Finally construct the trailer word of the Extractor format
      inputWord = 0 ;
      if ( ( m_suppressionMode == 0 ) &&
           ( thresholdFirstFailed || thresholdSecondFailed ) ) {
         BitOp::setBit( &inputWord, 0 ) ;
      } else if ( ( m_suppressionMode != 0 ) && thresholdFirstFailed ) {
         BitOp::setBit( &inputWord, 0 ) ;
      }
      if ( thresholdFirstFailed ) { BitOp::setBit( &inputWord, 2 ) ; }
      if ( thresholdSecondFailed ) { BitOp::setBit( &inputWord, 1 ) ; }

      BitOp::sImposeNBits( &inputWord, 3, m_threshold1Candidate );
      BitOp::sImposeNBits( &inputWord, 6, m_threshold2Candidate );
      BitOp::sImposeNBits( &inputWord, 9, candidateCount );
      BitOp::setBit( &inputWord, 30 );

      m_extractorData.push_back( inputWord );

      return;
   }

   // implementation of method to extract first candidate from sector word
   unsigned int
   MirodExtractor::extractFirstCandidate( const unsigned int candidateFiFo,
                                          const unsigned int candidateFiFoHighBits ) {

      unsigned int result = 0;
      unsigned int ptValue = 0;

      // first get the candiates Pt value and jump out if it is 0 or 7
      // e.g. no candidate
      ptValue = ( BitOp::getValue( &candidateFiFo, Pt1Mask ) );
      if ( ( ptValue < 1 ) || ( ptValue > MAX_NUMBER_OF_THRESHOLDS ) ) { return result; }

      // check and set the bit telling if there where more than 2
      // candidates in the sector
      if ( ( candidateFiFo & SectorOverflowMask ) != 0 ) { BitOp::setBit( &result, 0 ); }

      // check and set the bit telling if the candiate is from
      // a pad with more than 1 candidates in pad
      if ( ( candidateFiFo & ROI1OverflowMask ) != 0 ) { BitOp::setBit( &result, 1 ); }

      // get RoI and Overlap in one go and superimpose it, as it is
      // not otherwise used in this context
      result |= ( ( BitOp::getValue( &candidateFiFo, RoIOvl1Mask ) ) << 2 );

      // now superimpose the Pt value
      BitOp::sImposeNBits( &result, 11, ptValue );

      // superimpose the BCID
      result |= ( ( candidateFiFo & BCIDMask ) >> 13 ) ;

      // superimpose the sector address - bit 8 in the address is always
      // zero for a word from the MIOCT - so I can just stick it on
      BitOp::sImposeNBits( &result, 17, candidateFiFoHighBits ) ;

      // this is the first (most energetic) candidate in this sector - so
      // set the corresponding bit
      BitOp::setBit( &result, 25 );

      // Set the sign of the candidate:
      if( candidateFiFo & Candidate1Sign ) result |= ExtrCandidateSign;

      // Set the suppression flag of the candidate on bit 28:
      if( candidateFiFoHighBits & 0x200 ) BitOp::setBit( &result, 28 );

      // finally compare the ptValue to the threshold for the highest
      // candidate in the sector and set the pass bit
      if ( ptValue >= m_threshold1Candidate ) {
         BitOp::setBit( &result, 26 );
      }

      return result;
   }

   // implementation of method to extract second candidate from sector word
   unsigned int
   MirodExtractor::extractSecondCandidate( const unsigned int candidateFiFo,
                                           const unsigned int candidateFiFoHighBits ) {

      unsigned int result = 0;
      unsigned int ptValue = 0;

      // first get the candiates Pt value and jump out if it is 0 or 7
      // e.g. no candidate
      ptValue = ( BitOp::getValue( &candidateFiFo, Pt2Mask ) );
      if ( ( ptValue < 1 ) || ( ptValue > MAX_NUMBER_OF_THRESHOLDS ) ) { return result; }

      // check and set the bit telling if there where more than 2
      // candidates in the sector
      if ( ( candidateFiFo & SectorOverflowMask ) != 0 ) { BitOp::setBit( &result, 0 ); }

      // check and set the bit telling if the candiate is from
      // a pad with more than 1 candidates in pad
      if ( ( candidateFiFo & ROI2OverflowMask ) != 0 ) { BitOp::setBit( &result, 1 ); }

      // get RoI and Overlap in one go and superimpose it, as it is
      // not otherwise used in this context
      result |= ( ( BitOp::getValue( &candidateFiFo, RoIOvl2Mask ) ) << 2 );

      // now superimpose the Pt value
      BitOp::sImposeNBits( &result, 11, ptValue );

      // superimpose the BCID
      result |= ( ( candidateFiFo & BCIDMask ) >> 13 );

      // superimpose the sector address - bit 8 in the address is always
      // zero for a word from the MIOCT - so I can just stick it on
      BitOp::sImposeNBits( &result, 17, candidateFiFoHighBits );

      // this is the second (second energetic) candidate in this sector
      // - so make sure the corresponding bit is not set
      BitOp::clearBit( &result, 25 );

      // Set the sign of the candidate:
      if( candidateFiFo & Candidate2Sign ) result |= ExtrCandidateSign;

      // Set the suppression flag of the candidate on bit 28:
      if( candidateFiFoHighBits & 0x400 ) BitOp::setBit( &result, 28 );

      // finally compare the ptValue to the threshold for the second
      // candidate in the sector and set the pass bit
      if ( ptValue >= m_threshold2Candidate ) {
         BitOp::setBit( &result, 26 );
      }

      return result;
   }

   // method to print the event
   void MirodExtractor::printEvent( std::string format ) const {

      unsigned int wordCount = 0;
      std::list<unsigned int>::const_iterator it_Extr = m_extractorData.begin();
      for( ; it_Extr != m_extractorData.end(); ++it_Extr ) {

         std::ostringstream message;

         if ( format == "hex" ) {
            message << std::hex << std::setw( 8 ) << ( *it_Extr );
         } else if ( format == "bin" ) {
            std::string binaryString;
            BitOp::printBin( ( *it_Extr ), binaryString );
            message << binaryString;
         } else if ( format == "human" ) {
            wordCount++;
            if ( wordCount == 1 ) {
               message << " =================================================="
                       << "=========================" << std::endl;
               message << "  Extr EvID: " << std::dec << std::setw( 8 )
                       << ( ( *it_Extr ) & ExtrHeaderEvID );
            }
            if ( wordCount == 2 ) {
               message << "  BcID: " << std::setw( 4 ) << ( ( *it_Extr ) & ExtrHeaderBcID );
            }
            if ( wordCount == 3 ) {
               message << "  Monitor: " << BitOp::getValue( &( *it_Extr ), ExtrMonitorMask );
               message << "  Trigger_Type: " << BitOp::getValue( &( *it_Extr ), ExtrTrTypeMask );
               message << "  Status: ";
               std::string statusString;
               BitOp::printBinN( ( *it_Extr ), 3, statusString );
               message << statusString << std::endl;
               message << " --------------------------------------------------"
                       << "-------------------------";
            }
            if ( ( *it_Extr ) & 0x20000000 ) {   // Mictp
               message << "  MICTP -> BcID: " << BitOp::getValue( &( *it_Extr ),
                                                                  ExtrMictpBCIDMask );
               message << "  Sum6: " << BitOp::getValue( &( *it_Extr ), ExtrSum6Mask );
               message << "  Sum5: " << BitOp::getValue( &( *it_Extr ), ExtrSum5Mask );
               message << "  Sum4: " << BitOp::getValue( &( *it_Extr ), ExtrSum4Mask );
               message << "  Sum3: " << BitOp::getValue( &( *it_Extr ), ExtrSum3Mask );
               message << "  Sum2: " << BitOp::getValue( &( *it_Extr ), ExtrSum2Mask );
               message << "  Sum1: " << BitOp::getValue( &( *it_Extr ), ExtrSum1Mask );
               message << std::endl;
               message << " --------------------------------------------------"
                       << "-------------------------";
            }
            if ( ( ( *it_Extr ) & 0xe0000000 ) == 0 ) {
               message << printSector( *it_Extr ); }  // print Sector info
            if ( ( *it_Extr ) & 0x40000000 ) {   // Trailer
               message << " --------------------------------------------------"
                       << "-------------------------" << std::endl;
               message << "  CandCount: " << BitOp::getValue( &( *it_Extr ),
                                                              ExtrCandCountMask );
               message << "    Thresh_1: " << BitOp::getValue( &( *it_Extr ),
                                                               ExtrThresh1Mask );
               message << "    Thresh_2: " << BitOp::getValue( &( *it_Extr ),
                                                               ExtrThresh2Mask );
               message << "    S1: " << BitOp::getValue( &( *it_Extr ),
                                                         Extr1stCandSupprMask );
               message << "    S2: " << BitOp::getValue( &( *it_Extr ),
                                                         Extr2ndCandSupprMask );
               message << "    S: " << BitOp::getValue( &( *it_Extr ),
                                                        ExtrEitherCandSupprMask );
               message << std::endl;
               message << " =================================================="
                       << "=========================";
            }
         } else {
            REPORT_FATAL_MSG( "Non-existent print method choosen" );
            assert( 0 );
         }

         REPORT_VERBOSE_MSG( message.str() );

      }

      return;
   }

   // method to print one sector in human readable format
   std::string MirodExtractor::printSector( const unsigned int & sectorWord ) {

      std::string sectorString = "";
      std::ostringstream outStream;
      std::string system;
      std::string hemisphere = "-";
      unsigned int sectorNumber = 0;
      unsigned int regionOfInterest = 0;
      unsigned int overlap = 0;

      // get the values out of the bit mask
      const unsigned int candPassed = BitOp::getValue( &sectorWord, ExtrPassedMask );
      const unsigned int candFirst = BitOp::getValue( &sectorWord, ExtrFirstCandMask );
      const unsigned int sectorAddress =
         BitOp::getValue( &sectorWord, ExtrSectorAddressMask );
      const unsigned int sectorBCID = BitOp::getValue( &sectorWord, ExtrBCIDMask );
      const unsigned int ptValue = BitOp::getValue( &sectorWord, ExtrPtValueMask );
      const unsigned int sectorRoIOvl = BitOp::getValue( &sectorWord, ExtrRoIOvlMask );
      const unsigned int padOverflow =
         BitOp::getValue( &sectorWord, ExtrPadOverflowMask );
      const unsigned int sectorOverflow =
         BitOp::getValue( &sectorWord, ExtrSectorOverflowMask );
      const bool veto = sectorWord & 0x10000000;

      // sector address and RoI and Overlap must be taken apart agin
      // to extract the readable information
      // first find out if this is Barrel, endcap, forward, get sector
      // number, RoI and Overlap information
      if ( BitOp::getValue( &sectorAddress, EvReSecAddressSystemMask ) == 0 ) {
         system = "B"; // Barrel
         sectorNumber = BitOp::getValue( &sectorAddress, EvReSecNumberBarrelMask );
         regionOfInterest = BitOp::getValue( &sectorRoIOvl, EvReBarrelRoIMask );
         overlap = BitOp::getValue( &sectorRoIOvl, EvReBarrelOvlMask );
      } else if ( BitOp::getValue( &sectorAddress, EvReSecAddressSystemMask ) == 1 ) {
         system = "F"; // Forward
         sectorNumber = BitOp::getValue( &sectorAddress, EvReSecNumberForwardMask );
         regionOfInterest = BitOp::getValue( &sectorRoIOvl, EvReForwardRoIMask );
         overlap = 0;
      } else if ( BitOp::getValue( &sectorAddress, EvReSecAddressSystemMask ) > 1 ) {
         system = "E"; // Endcap
         sectorNumber = BitOp::getValue( &sectorAddress, EvReSecNumberEndcapMask );
         regionOfInterest = BitOp::getValue( &sectorRoIOvl, EvReEndcapRoIMask );
         overlap = BitOp::getValue( &sectorRoIOvl, EvReEndcapOvlMask );
      }
      // get the Hemisphere
      if ( BitOp::getValue( &sectorAddress, EvReSecAddressSubSysMask ) == 1 ) {
         hemisphere = "+"; }

      outStream << "  Addr: " << hemisphere << system << " "
                << std::setw( 2 ) << std::dec << sectorNumber
                << " Pt/RoI: " << ptValue
                << " " << std::setw( 2 ) << regionOfInterest
                << " BcID: " << std::dec << std::setw( 4 )
                << std::dec << sectorBCID
                << " Ovl: " << overlap
                << " pad/secOF: " << std::setw( 2 ) << padOverflow
                << std::setw( 2 ) << sectorOverflow
                << " pass: " << candPassed
                << " First: " << candFirst
                << " veto: " << veto;

      sectorString = outStream.str();

      return sectorString;
   }

} // namespace LVL1MUCTPI
