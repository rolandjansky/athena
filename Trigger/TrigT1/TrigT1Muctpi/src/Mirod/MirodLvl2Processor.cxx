/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodLvl2Processor.cxx 726107 2016-02-25 11:04:42Z wengler $

// STL include(s):
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>

// Local include(s):
#include "MirodLvl2Processor.h"
#include "MirodExtractor.h"
#include "../Common/Configuration.h"
#include "../Common/BitOp.h"
#include "../Common/MuctpiBitMasks.h"
#include "../Common/EventID.h"
#include "../Common/SectorConstants.h"

/*******************************************************************
 * $Date: 2016-02-25 12:04:42 +0100 (Thu, 25 Feb 2016) $
 *
 * Implementation of class MirodLvl2Processor
 * @author   Author: Thorsten Wengler
 * @version $Revision: 726107 $
 ******************************************************************/

namespace LVL1MUCTPI {

   // default constructor of class MirodLvl2Processor
   MirodLvl2Processor::MirodLvl2Processor()
      : m_logger( "MirodLvl2Processor" ),
        m_ptSorterBuckets( MAX_NUMBER_OF_THRESHOLDS ), 
	m_maxCandPerPtvalue(0),  m_maxCandSendToRoib(0), m_candBcidOffset(0)  {

      REPORT_VERBOSE_MSG( "constructor called" );
   }

   // constructor of class MirodLvl2Processor with unsigned int list
   // as argument used to set the TheLvl2OutputData data member
   MirodLvl2Processor::MirodLvl2Processor( std::list<unsigned int> lvl2Out )
      : m_logger( "MirodLvl2Processor" ),
        m_ptSorterBuckets( MAX_NUMBER_OF_THRESHOLDS ), 
	m_maxCandPerPtvalue(0), m_maxCandSendToRoib(0), m_candBcidOffset(0)  {

      REPORT_VERBOSE_MSG( "in constructor" );
      m_lvl2OutputData = lvl2Out;
   }

   // set Configuration
   void MirodLvl2Processor::setConfiguration( const Configuration& conf ) {

      m_maxCandPerPtvalue = conf.getMaxCandPerPtvalue();
      m_maxCandSendToRoib = conf.getMaxCandSendToRoib();
      m_candBcidOffset = conf.getCandBcidOffset();
      for( unsigned int i = 0; i < MAX_NUMBER_OF_THRESHOLDS; ++i ) {
         m_ptSorterBuckets[ i ].reserve( m_maxCandPerPtvalue );
      }

      return;
   }

   // implementation of main action method
   void MirodLvl2Processor::processData( const MirodExtractor* theExtractorData ) {

      REPORT_VERBOSE_MSG( "reading event from Extractor" );

      unsigned int inputWord = 0;
      unsigned int extractorStatus = 0;
      unsigned int monitorFlag = 0;
//       unsigned int thresholdFirst = 0;
//       unsigned int thresholdSecond = 0;
      unsigned int suppressedFirst = 0;
      unsigned int suppressedSecond = 0;
      unsigned int suppressedEither = 0;
      unsigned int candidateCounterOverflow = 0;
      unsigned int candidateSorterOverflow = 0;

      unsigned int headerBCID = 0;

      // Get the run number:
      EventID eventID;
      unsigned int runNumber = eventID.getRunNumber();

      m_lvl2OutputData.clear();
      for( unsigned int i = 0; i < MAX_NUMBER_OF_THRESHOLDS; ++i ) {
         m_ptSorterBuckets[ i ].clear();
      }
      m_ptCombinedFiFo.clear();

      //get pointer to Extractor data
      const std::list< unsigned int >* extractorData = theExtractorData->getExtractorData();

      // the fixed parts of the header to be send to the ROB
      m_lvl2OutputData.push_back( Lvl2HeaderStartMask );  // Header marker
      m_lvl2OutputData.push_back( Lvl2HeaderSizeMask );   // Header size bytes
      m_lvl2OutputData.push_back( Lvl2HeaderFormatMask ); // format version
      m_lvl2OutputData.push_back( Lvl2HeaderDetIDMask );  // Det. ID
      m_lvl2OutputData.push_back( runNumber );            // Run number

      // now loop over the Extractor data, except last word
      std::list< unsigned int >::const_iterator it_Extr;

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
      extendedL1A <<= 24;
      extendedL1A |= l1a;
      m_lvl2OutputData.push_back( extendedL1A );
      m_lvl2OutputData.push_back( headerBCID );
      m_lvl2OutputData.push_back( trType );
      m_lvl2OutputData.push_back( 0x0 ); // this is the event type

      for ( ; it_Extr != ( --extractorData->end() ) ; ++it_Extr ) {
         inputWord = 0 ;
         inputWord = ( *it_Extr ) ;

         // now here the rest of the data is not just copied to the
         // output stream as in the DaqProcessor, but the MICTP word
         // is not used at all, and the sector candidates are only
         // taken if they passed the threshold in the Extractor , checked
         // to see if the have the expected BCID and
         // are sorted for their pt values
         if ( ( !BitOp::isSet( &inputWord, 29 ) ) &&         // not the MICTP word
              ( ( inputWord & ExtrPassedMask ) != 0 ) &&    // passed Thresh.?
              ( BitOp::getValue( &inputWord, ExtrBCIDMask ) ==
                ( ( headerBCID + m_candBcidOffset ) & 0x7 ) ) ) {  // cand. w/ correct BCID ?
            // for the Level 2 output, the BCID is not part anymore of the
            // sector word, and also the bit saying that it passed the
            // threshold is gone (it would not be here otherwise)
            // so the sector address gets shifted by three bits and the pass
            // bit is reset

            const bool candSign = inputWord & ExtrCandidateSign;

            const unsigned int tmpSectAddFirst =
               BitOp::getValue( &inputWord,
                                ( ExtrFirstCandMask |      // get first cand
                                  ExtrSectorAddressMask ) ); // get sector address
            const unsigned int clearombMask = ( ExtrPassedMask |
                                                ExtrFirstCandMask |
                                                ExtrSectorAddressMask |
                                                ExtrBCIDMask );
            ( inputWord |= clearombMask ) ^= clearombMask;

            BitOp::sImposeNBits( &inputWord, Lvl2SecAddressStartBit, tmpSectAddFirst );

            if( candSign ) inputWord |= Lvl2CandidateSign;

            const unsigned int ptValue = BitOp::getValue( &inputWord, Lvl2PtValueMask );
            if( ( ptValue < 1 ) || ( ptValue > 6 ) ) {
               REPORT_ERROR_MSG( "Unknown pT value (" << ptValue << ") for candidate: 0x"
                                 << std::hex << inputWord );
               continue;
            }
            // Add the candidate to one of the sorter buckets:
            if( m_ptSorterBuckets[ ptValue - 1 ].size() >= m_maxCandPerPtvalue ) {
               candidateSorterOverflow = 1;
               // Overwrite the first candidate: (make sure it exisits - it might not if there was 
	       // a configurtion error
	       if ( m_ptSorterBuckets[ ptValue - 1 ].size() >= 1 ) {
		 m_ptSorterBuckets[ ptValue - 1 ][ 0 ] = inputWord;
	       } else {
		 REPORT_ERROR_MSG( "Sorter bucket size 0 - config error?" );
	       }
            } else {
               m_ptSorterBuckets[ ptValue - 1 ].push_back( inputWord );
            }
         }
      } // end of loop over Extractor data

      // retrieve information from last word in extractor data
      inputWord = 0 ;
      inputWord = extractorData->back() ;
//       thresholdFirst = BitOp::getValue( &inputWord, ExtrThresholdOneMask );
//       thresholdSecond = BitOp::getValue( &inputWord, ExtrThresholdSecondMask );
      suppressedFirst = BitOp::getValue( &inputWord, ExtrSuppressedFirstMask );
      suppressedSecond = BitOp::getValue( &inputWord, ExtrSuppressedSecondMask );
      suppressedEither = BitOp::getValue( &inputWord, ExtrSuppressedEitherMask );

      // Create a "master list" of all the candidates, with the right sorting:
      std::vector< std::vector< unsigned int > >::const_reverse_iterator pt_itr =
         m_ptSorterBuckets.rbegin();
      std::vector< std::vector< unsigned int > >::const_reverse_iterator pt_end =
         m_ptSorterBuckets.rend();
      for( ; pt_itr != pt_end; ++pt_itr ) {
         std::vector< unsigned int >::const_reverse_iterator itr =
            pt_itr->rbegin();
         std::vector< unsigned int >::const_reverse_iterator end =
            pt_itr->rend();
         for( ; itr != end; ++itr ) {
            m_ptCombinedFiFo.push_back( *itr );
         }
      }

      // add the selected and sorted candidates to the data stream
      // up to a maximum number of candidates
      unsigned int candidates = 0;
      std::list< unsigned int >::const_iterator itr = m_ptCombinedFiFo.begin();
      std::list< unsigned int >::const_iterator end = m_ptCombinedFiFo.end();
      for( ; itr != end; ++itr, ++candidates ) {
         // Make sure that we're not adding too many candidates to the output:
         if( candidates >= m_maxCandSendToRoib ) {
            candidateCounterOverflow = 1;
            break;
         }
         m_lvl2OutputData.push_back( *itr );
      }

      // build up error status word and add to data stream
      inputWord = 0 ;
      if ( BitOp::isSet( &extractorStatus, 0 ) ) {
         BitOp::setBit( &inputWord, 1 ); // EvIDMM
      }
      if ( BitOp::isSet( &extractorStatus, 1 ) ) {
         BitOp::setBit( &inputWord, 0 ); // BcIDMM
      }
      m_lvl2OutputData.push_back( inputWord );

      // build up status word and add to data stream
      inputWord = 0 ;
      if ( monitorFlag != 0 ) {BitOp::setBit( &inputWord, 20 );}
      if ( suppressedFirst != 0 ) {BitOp::setBit( &inputWord, 16 );}
      if ( suppressedSecond != 0 ) {BitOp::setBit( &inputWord, 17 );}
      if ( candidateCounterOverflow != 0 ) {BitOp::setBit( &inputWord, 18 );}
      if ( candidateSorterOverflow != 0 ) {BitOp::setBit( &inputWord, 19 );}
      if ( suppressedEither != 0 ||
           candidateCounterOverflow != 0 ||
           candidateSorterOverflow != 0 ) {BitOp::setBit( &inputWord, 1 );}
      m_lvl2OutputData.push_back( inputWord );

      // add number of status elements
      m_lvl2OutputData.push_back( 0x00000002 );

      // fill in number of data words
      inputWord = 0 ;
      inputWord = m_lvl2OutputData.size() - 12; // 12 comes from the 9 header words
      // and the 3 status words already added
      // to the list.
      m_lvl2OutputData.push_back( inputWord );

      // fill in status block position
      m_lvl2OutputData.push_back( 0x00000001 );

      return;
   }

   // method to print the event
   void MirodLvl2Processor::printEvent( std::string format ) const {

      REPORT_VERBOSE_MSG( "printing output of Lvl2Processor" );

      bool mioctWord = false;
      bool trailerWord = false;
      unsigned int wordCount = 0;
      unsigned int trailerWordCount = 0;
      std::list<unsigned int>::const_iterator it_Lvl2Pr = m_lvl2OutputData.begin();
      std::list<unsigned int>::const_iterator it_Lvl2PrMioctStart = m_lvl2OutputData.begin();
      std::list<unsigned int>::const_iterator it_Lvl2PrMioctEnd = m_lvl2OutputData.end();
      for( int i = 0; i < 9; ++i ) {
         ++it_Lvl2PrMioctStart;
      }
      for( int i = 0; i < 5; ++i ) {
         --it_Lvl2PrMioctEnd;
      }

      for( ; it_Lvl2Pr != m_lvl2OutputData.end(); ++it_Lvl2Pr ) {

         std::ostringstream message;

         if ( format == "hex" ) {
            message << std::hex << std::setw( 8 ) << ( *it_Lvl2Pr );
         } else if ( format == "bin" ) {
            std::string binaryString;
            BitOp::printBin( ( *it_Lvl2Pr ), binaryString );
            message << binaryString;
         } else if ( format == "human" ) {

            wordCount++;
            if ( wordCount == 1 ) {                                // Header 1
               message << " =================================================="
                       << "=========================" << std::endl;
               message << "  Lvl2Out: 0x" << std::hex << ( *it_Lvl2Pr );
            }
            if ( wordCount == 2 ) {                                // Header 2
               message << "  HeaderBytes: " << ( *it_Lvl2Pr );
            }
            if ( wordCount == 3 ) {                                // Header 3
               message << "  Format: 0x" << std::hex << ( *it_Lvl2Pr );
            }
            if ( wordCount == 4 ) {                                // Header 4
               message << "  DetID: 0x" << std::hex << ( *it_Lvl2Pr );
            }
            if ( wordCount == 5 ) {                                // Header 5
               message << "  Run Number: " << ( *it_Lvl2Pr );
            }
            if ( wordCount == 6 ) {                                // Header 6
               message << "  EvID: " << ( *it_Lvl2Pr );
            }
            if ( wordCount == 7 ) {                                // Header 7
               message << "  BcID: " << ( ( *it_Lvl2Pr ) & DaqHeaderBcIDMask );
            }
            if ( wordCount == 8 ) {                                // Header 8
               message << "  TrType: " << ( ( *it_Lvl2Pr ) & DaqHeaderTrTypeMask );
            }
            if ( wordCount == 9 ) {                                // Header 9
               message << "  EvType: " << ( *it_Lvl2Pr ) << std::endl;
               message << " --------------------------------------------------"
                       << "-------------------------";
            }
            if ( ( it_Lvl2Pr == it_Lvl2PrMioctStart ) || mioctWord ) { // MIOCT's
               mioctWord = true;
               if ( ( it_Lvl2Pr ) == it_Lvl2PrMioctEnd ) {
                  mioctWord = false;
                  trailerWord = true;
               } else {
                  message << printSector( *it_Lvl2Pr );
               }
            }
            if ( trailerWord ) {                                 //Trailer
               trailerWordCount++;
               if ( trailerWordCount == 1 ) {
                  message << " --------------------------------------------------"
                          << "-------------------------" << std::endl;
                  message << "  Errors: ";
                  std::string errorString;
                  BitOp::printBin( ( *it_Lvl2Pr ), errorString );
                  message << errorString;
               }
               if ( trailerWordCount == 2 ) {
                  message << "  Status: ";
                  std::string statusString;
                  BitOp::printBin( ( *it_Lvl2Pr ), statusString );
                  message << statusString;
               }
               if ( trailerWordCount == 3 ) {
                  message << "  Status Elements: " << ( *it_Lvl2Pr );
               }
               if ( trailerWordCount == 4 ) {
                  message << "  Data Elements: "
                          << ( ( *it_Lvl2Pr ) & DaqTrailerCandMask );
               }
               if ( trailerWordCount == 5 ) {
                  message << "  StatusBlockPos: " << ( *it_Lvl2Pr ) << std::endl;
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
   std::string MirodLvl2Processor::printSector( const unsigned int & sectorWord ) const {

      std::ostringstream outStream;
      std::string system;
      std::string hemisphere = "-";
      unsigned int sectorNumber = 0;
      unsigned int regionOfInterest = 0;
      unsigned int overlap = 0;

      // get the values out of the bit mask
      const unsigned int candFirst = BitOp::getValue( &sectorWord, Lvl2FirstCandMask );
      const unsigned int sectorAddress =
         BitOp::getValue( &sectorWord, Lvl2SectorAddressMask );
      const unsigned int ptValue = BitOp::getValue( &sectorWord, Lvl2PtValueMask );
      const unsigned int sectorRoIOvl = BitOp::getValue( &sectorWord, Lvl2RoIOvlMask );
      const unsigned int padOverflow =
         BitOp::getValue( &sectorWord, Lvl2PadOverflowMask );
      const unsigned int sectorOverflow =
         BitOp::getValue( &sectorWord, Lvl2SectorOverflowMask );
      const bool veto = sectorWord & 0x10000000;

      // sector address and RoI and Overlap must be taken apart agin
      // to extract the readable information
      // first find out if this is Barrel, endcap, forward, get sector
      // number, RoI and Overlap information
      if ( BitOp::getValue( &sectorAddress, EvReSecAddressSystemMask ) == 0 ) {
         system = "B"; // Barrel
         sectorNumber = BitOp::getValue( &sectorAddress, EvReSecNumberBarrelMask );
         regionOfInterest = BitOp::getValue( &sectorRoIOvl, Lvl2BarrelRoIMask );
         overlap = BitOp::getValue( &sectorRoIOvl, Lvl2BarrelOvlMask );
      } else if ( BitOp::getValue( &sectorAddress, EvReSecAddressSystemMask ) == 1 ) {
         system = "F"; // Forward
         sectorNumber = BitOp::getValue( &sectorAddress, EvReSecNumberForwardMask );
         regionOfInterest = BitOp::getValue( &sectorRoIOvl, Lvl2ForwardRoIMask );
         overlap = 0;
      } else if ( BitOp::getValue( &sectorAddress, EvReSecAddressSystemMask ) > 1 ) {
         system = "E"; // Endcap
         sectorNumber = BitOp::getValue( &sectorAddress, EvReSecNumberEndcapMask );
         regionOfInterest = BitOp::getValue( &sectorRoIOvl, Lvl2EndcapRoIMask );
         overlap = BitOp::getValue( &sectorRoIOvl, Lvl2EndcapOvlMask );
      }
      // get the Hemisphere
      if ( BitOp::getValue( &sectorAddress, EvReSecAddressSubSysMask ) == 1 ) {
         hemisphere = "+";
      }

      outStream << "  Addr: " << hemisphere << system << " "
                << std::setw( 2 ) << std::dec << sectorNumber
                << " Pt/RoI: " << ptValue
                << " " << std::setw( 2 ) << regionOfInterest
                << " Ovl: " << overlap
                << " pad/secOF: " << std::setw( 2 ) << padOverflow
                << std::setw( 2 ) << sectorOverflow
                << " First: " << candFirst
                << " veto: " << veto;

      return outStream.str();
   }

} // namespace LVL1MUCTPI
