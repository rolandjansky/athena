/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  m_DEBUG( false ) {

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

