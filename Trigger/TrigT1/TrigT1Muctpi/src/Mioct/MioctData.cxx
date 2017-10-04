/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MioctData.cxx 439423 2011-05-25 21:22:40Z krasznaa $

// Local include(s):
#include "MioctData.h"
#include "MioctSectorSet.h"
#include "OverlapLogic.h"
#include "MioctID.h"
#include "../Common/BitOp.h"
#include "../Common/PtMultiplicitySet.h"
#include "../Common/MuctpiBitMasks.h"
#include "../Logging/MsgLogger.h"


/*******************************************************************
 * $Date: 2011-05-25 23:22:40 +0200 (Wed, 25 May 2011) $
 *
 * Implementation of class MioctData
 * @author   Author: Thorsten Wengler
 * @version $Revision: 439423 $
 ******************************************************************/

namespace LVL1MUCTPI {

   const unsigned int MioctData::m_startMioct = MioctStartMask;
   const unsigned int MioctData::m_endMioct   = MioctEndMask;
   const unsigned int MioctData::m_sectorNumberMap[ SECTORS_IN_MIOCT ] = {
      0xa, 0xb, 0xc, 0x7, 0x8, 0x9, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6
   };

   // constructor
   MioctData::MioctData()
      : m_headerWord( 0 ), m_trailerWord( 0 ), m_flags( 0 ),
        m_sectorWords(), m_sectorHighBits(), m_eventID() {

   }

   void MioctData::assignData( const MioctID& ID, const MioctSectorSet& mioctSectors,
                               const OverlapLogic* lvl1OverlapLogic ) {

      // Start with calculating the multiplicity. This flags the candidates that
      // were suppressed in the multiplicity calculation.
      PtMultiplicitySet ptSums = lvl1OverlapLogic->calculateMultiplicity( mioctSectors, ID );

      // get the information needed for the Header word and fill it
      m_headerWord = 0;
      unsigned int MNBR = ID.getNumber();             // module number
      Hemisphere theHem = ID.getRapidityRegion();     // Rapidity region
      if( theHem == POSITIVE ) MNBR += 8;
      unsigned int EVID = m_eventID.getEvID();        // Event ID
      unsigned int BCID = m_eventID.BCID();           // Bunch ID
      BitOp::sImposeNBits( &m_headerWord, 28, MNBR ); // impose Module Number
      BitOp::sImposeNBits( &m_headerWord, 12, EVID ); // impose EVID
      BitOp::sImposeNBits( &m_headerWord,  0, BCID ); // impose BCID

      // get the list of sector words from the set of sectors belonging to this MIOCT
      m_sectorWords.clear();
      m_sectorHighBits.clear();
      for( unsigned int i = 0; i < mioctSectors.getBitFieldSet()->size(); ++i ) {
         m_sectorWords.push_back( *mioctSectors.getBitFieldSet()->at( i ) );
         // Bit 36 is the suppression bit for the first candidate, and bit 37 is the
         // suppression bit for the second candidate.
         m_sectorHighBits.push_back( m_sectorNumberMap[ i ] );
         switch( i ) {
         case 0:
            m_sectorHighBits[ i ] |= ( mioctSectors.Forward1.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Forward1.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 1:
            m_sectorHighBits[ i ] |= ( mioctSectors.Forward2.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Forward2.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 2:
            m_sectorHighBits[ i ] |= ( mioctSectors.Forward3.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Forward3.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 3:
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap4.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap4.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 4:
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap5.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap5.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 5:
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap6.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap6.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 6:
            m_sectorHighBits[ i ] |= ( mioctSectors.Barrel1.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Barrel1.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 7:
            m_sectorHighBits[ i ] |= ( mioctSectors.Barrel2.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Barrel2.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 8:
            m_sectorHighBits[ i ] |= ( mioctSectors.Barrel3.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Barrel3.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 9:
            m_sectorHighBits[ i ] |= ( mioctSectors.Barrel4.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Barrel4.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 10:
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap1.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap1.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 11:
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap2.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap2.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         case 12:
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap3.getCand1Supressed() ?
                                       0x10 : 0x0 );
            m_sectorHighBits[ i ] |= ( mioctSectors.Endcap3.getCand2Supressed() ?
                                       0x20 : 0x0 );
            break;
         default:
            MsgLogger logger( "MioctData" );
            REPORT_ERROR_MSG2( "There's a coding mistake in the MIOCT simulation!", logger );
            break;
         }
      }

      // Fill the trailer word:
      m_trailerWord = 0;
      // Pt sums
      BitOp::sImposeNBits( &m_trailerWord, 10, ptSums.getMultiplicity( 1 ) );
      BitOp::sImposeNBits( &m_trailerWord, 13, ptSums.getMultiplicity( 2 ) );
      BitOp::sImposeNBits( &m_trailerWord, 16, ptSums.getMultiplicity( 3 ) );
      BitOp::sImposeNBits( &m_trailerWord, 19, ptSums.getMultiplicity( 4 ) );
      BitOp::sImposeNBits( &m_trailerWord, 22, ptSums.getMultiplicity( 5 ) );
      BitOp::sImposeNBits( &m_trailerWord, 25, ptSums.getMultiplicity( 6 ) );
      // flags: nothing set for now
      m_flags = 0;
      BitOp::sImposeNBits( &m_trailerWord, 7, m_flags );
      // word count: number of sector words plus header and trailer
      // overwritten later by method setTrailorWordCount in case
      // zero suppression is switched on
      BitOp::sImposeNBits( &m_trailerWord, 0,
                           static_cast< unsigned int >( m_sectorWords.size() + 2 ) );

      return;
   }

   void MioctData::setTrailerWordCount( unsigned int theWordCount ){

      for( int i = 0; i < 7; ++i ) { BitOp::clearBit( &m_trailerWord, i ); }
      BitOp::sImposeNBits( &m_trailerWord, 0, theWordCount );

      return;
   }

} // namespace LVL1MUCTPI
