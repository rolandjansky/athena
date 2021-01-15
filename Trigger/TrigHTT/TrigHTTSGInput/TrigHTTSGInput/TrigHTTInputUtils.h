/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */


#ifndef TRIGHTTINPUTUTILS_H
#define TRIGHTTINPUTUTILS_H

#include "HepMC/GenParticle.h"
#include <bitset>


namespace TrigHTTInputUtils {
  
  typedef enum { TAU_PARENT_BIT , B_PARENT_BIT , PION_PARENT_BIT , PION_IMMEDIATE_PARENT_BIT , NBITS } Bits;
  typedef std::bitset<NBITS> ParentBitmask;
  const ParentBitmask construct_truth_bitmap( const HepMC::GenParticle* p ) ;

 


}

#endif
