/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */


#ifndef TrigHTTSGInput_TRIGHTTINPUTUTILS_H
#define TrigHTTSGInput_TRIGHTTINPUTUTILS_H

#include "AtlasHepMC/GenParticle_fwd.h"
#include <bitset>


namespace TrigHTTInputUtils {
  
  typedef enum { TAU_PARENT_BIT , B_PARENT_BIT , PION_PARENT_BIT , PION_IMMEDIATE_PARENT_BIT , NBITS } Bits;
  typedef std::bitset<NBITS> ParentBitmask;
  const ParentBitmask construct_truth_bitmap( HepMC::ConstGenParticlePtr p ) ;
}

#endif
