/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         JetEtRoIDecoder.cxx  -  description
                            -------------------
   begin                : Fri Apr 19 2002
   email                : moyse@ph.qmw.ac.uk
***************************************************************************/


#include <cmath>
#include <iostream>

#include "TrigT1Interfaces/JetEtRoIDecoder.h"

namespace LVL1 {

  JetEtRoIDecoder::JetEtRoIDecoder() {

  }

  JetEtRoIDecoder::~JetEtRoIDecoder() {

  }

  /** returns a 4b number which represents the thresholds passed */
  unsigned int JetEtRoIDecoder::energy( unsigned int roiWord ) const {
    return roiWord & 0xF;
  }

  /** returns the value of bits in word between bit "start" and bit "start" + "length" */
  unsigned int JetEtRoIDecoder::extractBits( unsigned int word, const unsigned int start, const unsigned int length ) const {

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

} // namespace LVL1
