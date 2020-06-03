/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// $Id: RoIDecoder.cxx 187728 2009-05-27 16:18:06Z krasznaa $
/***************************************************************************
                          RoIDecoder.cxx  -  description
                             -------------------
    begin                : Fri Apr 19 2002
    email                : moyse@ph.qmw.ac.uk
 ***************************************************************************/


#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1Interfaces/RoIDecoder.h"
#else
// running in TRIGGERSPACE
#include "RoIDecoder.h"
#endif

#include <math.h>
#include <iostream>

LVL1::RoIDecoder::RoIDecoder():
   m_crate( 0 ),  m_thresholdsPassed( 0 ), m_DEBUG( false ) {

}

LVL1::RoIDecoder::~RoIDecoder() {

}

/** returns the value of bits in word between bit "start" and bit "start" + "length" */
unsigned int LVL1::RoIDecoder::extractBits( unsigned int word, const unsigned int start,
                                            const unsigned int length ) const {

   // this works as follows. Imagine start=3, bits=4 and word=11011100
   // so this routine should return 0111
   // now:
   // word>>(first-1)  = 110111
   // (1<<length)      = 010000
   // (1<<length)-1    = 001111
   word = ( word >> ( start - 1 ) );

   unsigned int bitmask = ( 1 << length ) - 1;
   // and finally:
   // 110111&001111    = 0111
   return ( word & bitmask ); // return only bits in bitmask
}

/** print thresholds passed */
void LVL1::RoIDecoder::dumpThresholdsPassed() const{
 
   for( unsigned int thresh = 0; thresh < 16; ++thresh ) {
      if( ( 1 << thresh ) & m_thresholdsPassed ) {
         std::cout << "Threshold " << ( thresh + 1 ) << " : passed" << std::endl;
      }
   }
   return;
}

/** returns the crate associated with the RoI word */
unsigned int LVL1::RoIDecoder::crate( const unsigned int word ){
  decodeWord(word);
  return m_crate;
}

/** returns a vector containing the numbers of threshold passed i.e. if the vector
    contains 1,3,5 it means that this RoI passed thresholds 1,3 and 5.*/
const std::vector< unsigned int >& LVL1::RoIDecoder::thresholdsPassed( const unsigned int word ) {

   decodeWord( word );
   m_threshPassedVec.clear();
   for( unsigned int thresh = 0; thresh < 16; ++thresh ){
      if( ( 1 << thresh ) & m_thresholdsPassed ) {
         m_threshPassedVec.push_back( thresh + 1 );
      }
   }
   return m_threshPassedVec;
}

/** examines an 32bit RoI word, extracts the RoI type and returns it*/
LVL1::TrigT1CaloDefs::RoIType LVL1::RoIDecoder::roiType( unsigned int word ) const{
   unsigned int temp=extractBits(word, 31, 2);
   if (temp == TrigT1CaloDefs::cpRoIType) {
      return TrigT1CaloDefs::CpRoIWordType;
   }
   else if (temp == TrigT1CaloDefs::jetRoIType) {
      unsigned int temp2 = extractBits(word, 30, 1);
      if (temp2 == TrigT1CaloDefs::jetRoI) {
         return TrigT1CaloDefs::JetRoIWordType;
      }
      else if (temp2 == TrigT1CaloDefs::jetEtRoI) {
         return TrigT1CaloDefs::JetEtRoIWordType;
      }
      else {
         return TrigT1CaloDefs::RoIWordTypeError;
      }
   }
   else if (temp == TrigT1CaloDefs::energyRoIType) {
      unsigned int temp2 = extractBits(word, 29, 2);
      if (temp2 == TrigT1CaloDefs::energyRoI0) {
         return TrigT1CaloDefs::EnergyRoIWordType0;
      }
      else if (temp2 == TrigT1CaloDefs::energyRoI1) {
         return TrigT1CaloDefs::EnergyRoIWordType1;
      }
      else if (temp2 == TrigT1CaloDefs::energyRoI2) {
         return TrigT1CaloDefs::EnergyRoIWordType2;
      }
      else {
         return TrigT1CaloDefs::RoIWordTypeError;
      }
   }
   return TrigT1CaloDefs::RoIWordTypeError;
}
