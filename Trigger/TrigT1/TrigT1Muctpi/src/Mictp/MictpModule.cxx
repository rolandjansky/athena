/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MictpModule.cxx 700318 2015-10-13 14:13:15Z wengler $

// Local include(s):
#include "MictpModule.h"
#include "../Common/BitOp.h"
#include "../Common/PtMultiplicitySet.h"
#include "../Common/MuctpiBitMasks.h"

/*******************************************************************
 * $Date: 2015-10-13 16:13:15 +0200 (Tue, 13 Oct 2015) $ 
 *
 * Implementation of class MictpModule
 * @author   Author: Thorsten Wengler
 * @version $Revision: 700318 $
 ******************************************************************/

namespace LVL1MUCTPI {

   // constructor of class MictpModule
   MictpModule::MictpModule()
     :  m_CTPDataWord(0), m_logger("MictpModule")  {

      REPORT_VERBOSE_MSG( "Constructor called" );

      m_mictpData.setHighBits( &m_highBits );
      m_mictpData.setDataWords( &m_dataWords );
   }

   void MictpModule::setPtSums( PtMultiplicitySet* ptSums, unsigned int BCID, unsigned int ECRC ) {

      unsigned int inputWord = 0;

      // clear the data
      m_highBits.clear();
      m_dataWords.clear();

      // the first header word: MICTP ID and EventID and add to list
      BitOp::sImposeNBits( &inputWord, 28, MictpIDMask );
      BitOp::sImposeNBits( &inputWord, 0, m_eventID.getEvID() );
      m_highBits.push_back( MioctStartMask );
      m_dataWords.push_back( inputWord );
      inputWord = 0;

      // the second headerword; monitor bit, trigger type and BCID
      BitOp::sImposeNBits( &inputWord, 28, m_eventID.getMonitorEvent() );
      BitOp::sImposeNBits( &inputWord, 20, m_eventID.getTriggerType() );
      BitOp::sImposeNBits( &inputWord, 12, ( ECRC & 0xff ) );
      BitOp::sImposeNBits( &inputWord, 0, ( BCID & 0xfff ) );
      m_highBits.push_back( 0x0 );
      m_dataWords.push_back( inputWord );
      inputWord = 0;

      // the data word: BCID and Ptsums
      BitOp::sImposeNBits( &inputWord, 0, ptSums->getMultiplicity( 1 ) );
      BitOp::sImposeNBits( &inputWord, 3, ptSums->getMultiplicity( 2 ) );
      BitOp::sImposeNBits( &inputWord, 6, ptSums->getMultiplicity( 3 ) );
      BitOp::sImposeNBits( &inputWord, 9, ptSums->getMultiplicity( 4 ) );
      BitOp::sImposeNBits( &inputWord, 12, ptSums->getMultiplicity( 5 ) );
      BitOp::sImposeNBits( &inputWord, 15, ptSums->getMultiplicity( 6 ) );

      // this is the word sent to the CTP (no BCID)
      // It's important to observe that the first bit in the word is not used.
      // This is just how the hardware works...
      m_CTPDataWord = inputWord;
      m_CTPDataWord <<= 1;

      BitOp::sImposeNBits( &inputWord, 27, ( BCID & 0x7 ) ); // only 3 lowest bits
      m_highBits.push_back( 0x0 );
      m_dataWords.push_back( inputWord );
      inputWord = 0;

      // The trailer word: Ptsums, flags (dummy so far), word count
      BitOp::sImposeNBits( &inputWord, 10, ptSums->getMultiplicity( 1 ) );
      BitOp::sImposeNBits( &inputWord, 13, ptSums->getMultiplicity( 2 ) );
      BitOp::sImposeNBits( &inputWord, 16, ptSums->getMultiplicity( 3 ) );
      BitOp::sImposeNBits( &inputWord, 19, ptSums->getMultiplicity( 4 ) );
      BitOp::sImposeNBits( &inputWord, 22, ptSums->getMultiplicity( 5 ) );
      BitOp::sImposeNBits( &inputWord, 25, ptSums->getMultiplicity( 6 ) );
      unsigned int flags = 0;
      BitOp::sImposeNBits( &inputWord, 7, flags );
      unsigned int wordCount = m_dataWords.size() + 1;
      BitOp::sImposeNBits( &inputWord, 0, wordCount );
      m_highBits.push_back( MioctEndMask );
      m_dataWords.push_back( inputWord );
      inputWord = 0;

      return;
   }

} // namespace LVL1MUCTPI
