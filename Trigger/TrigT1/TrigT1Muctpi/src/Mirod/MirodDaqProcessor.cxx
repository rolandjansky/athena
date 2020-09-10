/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodDaqProcessor.cxx 624535 2014-10-28 10:02:49Z stelzer $

// STL include(s):
#include <iomanip>
#include <sstream>
#include <cassert>
#include <string>

// Local include(s):
#include "MirodDaqProcessor.h"
#include "MirodExtractor.h"
#include "../Common/BitOp.h"
#include "../Common/MuctpiBitMasks.h"
#include "../Common/EventID.h"

/*******************************************************************
 * $Date: 2014-10-28 11:02:49 +0100 (Tue, 28 Oct 2014) $
 *
 * Implementation of class MirodDaqProcessor
 * @author   Author: Thorsten Wengler
 * @version $Revision: 624535 $
 ******************************************************************/

namespace LVL1MUCTPI {

   // constructor of class MirodDaqProcessor
   MirodDaqProcessor::MirodDaqProcessor()
      : m_logger( "MirodDaqProcessor" ) {

      REPORT_VERBOSE_MSG( "constructor called" );
   }

   // constructor of class MirodDaqProcessor with unsigned int list
   // as argument used to set the TheDaqOutputData data member
   MirodDaqProcessor::MirodDaqProcessor( std::list<unsigned int> daqOut )
      : m_logger( "MirodDaqProcessor" ) {

      m_daqOutputData = std::move(daqOut);
   }

   // implementation of main action method
   void MirodDaqProcessor::processData( const MirodExtractor* theExtractorData ) {

      REPORT_VERBOSE_MSG( "reading event from Extractor" );

      unsigned int inputWord = 0;
      unsigned int extractorStatus = 0;
      unsigned int monitorFlag = 0;
//       unsigned int thresholdFirst = 0;
//       unsigned int thresholdSecond = 0;
      unsigned int suppressedFirst = 0;
      unsigned int suppressedSecond = 0;
//       unsigned int suppressedEither = 0;
      unsigned int headerBCID = 0;
      m_daqOutputData.clear();

      // Get the run number:
      EventID eventID;
      unsigned int runNumber = eventID.getRunNumber();

      //get pointer to Extractor data
      const std::list<unsigned int>* extractorData = theExtractorData->getExtractorData();

      // the fixed parts of the header to be send to the ROB
      m_daqOutputData.push_back( DaqHeaderStartMask );  // Header marker
      m_daqOutputData.push_back( DaqHeaderSizeMask );   // Header size bytes
      m_daqOutputData.push_back( DaqHeaderFormatMask ); // format version
      m_daqOutputData.push_back( DaqHeaderDetIDMask );  // Det. ID
      m_daqOutputData.push_back( runNumber );           // Run number

      // now loop over the Extractor data, except last word
      std::list<unsigned int>::const_iterator it_Extr;

      // I now handle the first 3 data words (coming originally from
      // the event FIFO) separately. This is mainly because of the
      // use of the Extended L1A ID.
      it_Extr = extractorData->begin();

      // The first word is the "plain" L1A (aka. EVID):
      unsigned int l1a = ( *it_Extr );
      BitOp::clearBit( &l1a, 31 );
      ++it_Extr;

      // this is the reference BCID - the candidates have to have
      // the same or a BCID at a fixed offset
      headerBCID = ( ( *it_Extr ) & ExtrHeaderBcID );
      unsigned int ECRC = ( ( ( *it_Extr ) & ExtrHeaderECRC ) >> 12 );
      ++it_Extr;

      inputWord = ( *it_Extr );
      monitorFlag = BitOp::getValue( &inputWord, ExtrMonitorMask ) ;
      extractorStatus = BitOp::getValue( &inputWord, ExtrStatusMask ) ;
      //    BitOp::clearBit(&inputWord,12) ;
      //    BitOp::clearBit(&inputWord,31);
      unsigned int trType = ( ( inputWord & ExtrTrTypeMask ) >> 4 ) ;
      ++it_Extr;

      // Now add the acumulated information to the Lvl2 output:
      unsigned int extendedL1A = ECRC;
      extendedL1A = extendedL1A << 24;
      extendedL1A |= l1a;
      m_daqOutputData.push_back( extendedL1A );
      m_daqOutputData.push_back( headerBCID );
      m_daqOutputData.push_back( trType );
      m_daqOutputData.push_back( 0x0 ); // this is the event type

      for( ; it_Extr != ( --extractorData->end() ) ; ++it_Extr ) {

         inputWord = 0;
         inputWord = ( *it_Extr );

         // put it to data stream
         m_daqOutputData.push_back( inputWord );
      } // end of loop over Extractor data

      // retrieve information from last word in extractor data
      inputWord = 0 ;
      inputWord = extractorData->back() ;
//       thresholdFirst = BitOp::getValue( &inputWord, ExtrThresholdOneMask );
//       thresholdSecond = BitOp::getValue( &inputWord, ExtrThresholdSecondMask );
      suppressedFirst = BitOp::getValue( &inputWord, ExtrSuppressedFirstMask );
      suppressedSecond = BitOp::getValue( &inputWord, ExtrSuppressedSecondMask );
//       suppressedEither = BitOp::getValue( &inputWord, ExtrSuppressedEitherMask );

      // build up error status word and add to data stream
      inputWord = 0 ;
      if ( BitOp::isSet( &extractorStatus, 0 ) ) {
         BitOp::setBit( &inputWord, 1 ); // EvIDMM
      }
      if ( BitOp::isSet( &extractorStatus, 1 ) ) {
         BitOp::setBit( &inputWord, 0 ); // BcIDMM
      }
      m_daqOutputData.push_back( inputWord );

      // build up status word and add to data stream
      inputWord = 0 ;
      if ( monitorFlag != 0 ) {BitOp::setBit( &inputWord, 20 );}
      if ( suppressedFirst != 0 ) {BitOp::setBit( &inputWord, 16 );}
      if ( suppressedSecond != 0 ) {BitOp::setBit( &inputWord, 17 );}
      m_daqOutputData.push_back( inputWord );

      // add number of status elements
      m_daqOutputData.push_back( 0x00000002 );

      // fill in number of data words
      inputWord = 0 ;
      inputWord = m_daqOutputData.size() - 12; // 12 comes from the 9 header words
                                               // and the 3 status words already added
                                               // to the list.
      m_daqOutputData.push_back( inputWord );

      // fill in status block position
      m_daqOutputData.push_back( 0x00000001 );

      return;
   }

   // method to print the event
   void MirodDaqProcessor::printEvent( std::string format ) const {

      REPORT_VERBOSE_MSG( "printing output of DaqProcessor" );

      bool mioctWord = false ;
      bool trailerWord = false ;
      unsigned int wordCount = 0 ;
      unsigned int trailerWordCount = 0 ;
      std::list<unsigned int>::const_iterator it_DaqPr = m_daqOutputData.begin();
      std::list<unsigned int>::const_iterator it_DaqPrMioctStart = m_daqOutputData.begin();
      std::list<unsigned int>::const_iterator it_DaqPrMioctEnd = m_daqOutputData.end();
      for( int i = 0; i < 10; ++i ) {
         ++it_DaqPrMioctStart;
      }
      for( int i = 0; i < 5; ++i ) {
         --it_DaqPrMioctEnd;
      }

      for( ; it_DaqPr != m_daqOutputData.end(); ++it_DaqPr ) {

         std::ostringstream message;

         if ( format == "hex" ) {
            message << std::hex << std::setw( 8 ) << ( *it_DaqPr );
         } else if ( format == "bin" ) {
            std::string binaryString;
            BitOp::printBin( ( *it_DaqPr ), binaryString );
            message << binaryString;
         } else if ( format == "human" ) {
            wordCount++;
            if ( wordCount == 1 ) {                                // Header 1
               message << " =================================================="
                       << "=========================" << std::endl;
               message << "  DaqOut: 0x" << std::hex << ( *it_DaqPr );
            }
            if ( wordCount == 2 ) {                                // Header 2
               message << "  HeaderBytes: " << ( *it_DaqPr );
            }
            if ( wordCount == 3 ) {                                // Header 3
               message << "  Format: 0x" << std::hex << ( *it_DaqPr );
            }
            if ( wordCount == 4 ) {                                // Header 4
               message << "  DetID: 0x" << std::hex << ( *it_DaqPr );
            }
            if ( wordCount == 5 ) {                                // Header 5
               message << "  Run Number: " << ( *it_DaqPr );
            }
            if ( wordCount == 6 ) {                                // Header 6
               message << "  EvID: " << ( *it_DaqPr );
        }
            if ( wordCount == 7 ) {                                // Header 7
               message << "  BcID: " << ( ( *it_DaqPr ) & DaqHeaderBcIDMask );
            }
            if ( wordCount == 8 ) {                                // Header 8
               message << "  TrType: " << ( ( *it_DaqPr ) & DaqHeaderTrTypeMask );
            }
            if ( wordCount == 9 ) {                                // Header 9
               message << "  EvType: " << ( *it_DaqPr ) << std::endl ;
               message << " --------------------------------------------------"
                       << "-------------------------";
            }
            if ( wordCount == 10 ) {                                // MICTP word
               message << "  MICTP -> BcID: " << BitOp::getValue( &( *it_DaqPr ),
                                                                  ExtrMictpBCIDMask );
               message << "  Sum6: " << BitOp::getValue( &( *it_DaqPr ), ExtrSum6Mask );
               message << "  Sum5: " << BitOp::getValue( &( *it_DaqPr ), ExtrSum5Mask );
               message << "  Sum4: " << BitOp::getValue( &( *it_DaqPr ), ExtrSum4Mask );
               message << "  Sum3: " << BitOp::getValue( &( *it_DaqPr ), ExtrSum3Mask );
               message << "  Sum2: " << BitOp::getValue( &( *it_DaqPr ), ExtrSum2Mask );
               message << "  Sum1: " << BitOp::getValue( &( *it_DaqPr ), ExtrSum1Mask );
               message << std::endl;
               message << " --------------------------------------------------"
                       << "-------------------------";
            }
            if ( ( it_DaqPr  == it_DaqPrMioctStart ) || mioctWord ) { // MIOCT's
               mioctWord = true;
               if ( it_DaqPr  == it_DaqPrMioctEnd ) {
                  mioctWord = false;
                  trailerWord = true;
               } else {
                  message << this->printSector( *it_DaqPr );
               }
            }
            if ( trailerWord ) {                                 //Trailer
               trailerWordCount++;
               if ( trailerWordCount == 1 ) {
                  message << " --------------------------------------------------"
                          << "-------------------------" << std::endl;
                  message << "  Errors: ";
                  std::string errorString;
                  BitOp::printBin( ( *it_DaqPr ), errorString );
                  message << errorString;
               }
               if ( trailerWordCount == 2 ) {
                  message << "  Status: ";
                  std::string statusString;
                  BitOp::printBin( ( *it_DaqPr ), statusString );
                  message << statusString;
               }
               if ( trailerWordCount == 3 ) {
                  message << "  Status Elements: " << ( *it_DaqPr );
               }
               if ( trailerWordCount == 4 ) {
                  message << "  Data Elements: " << ( ( *it_DaqPr ) & DaqTrailerCandMask );
               }
               if ( trailerWordCount == 5 ) {
                  message << "  StatusBlockPos: " << ( *it_DaqPr ) << std::endl;
                  message << " =================================================="
                          << "=========================";
               }
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
   std::string MirodDaqProcessor::printSector( const unsigned int & sectorWord ) const {

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
         hemisphere = "+";
      }

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

      return outStream.str();
   }

} // namespace LVL1MUCTPI
