/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ExclusiveMultStrategy.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// Local include(s):
#include "ExclusiveMultStrategy.h"

namespace LVL1MUCTPI {

   ExclusiveMultStrategy::ExclusiveMultStrategy()
      : MultiplicityStrategy( "Exclusive-Multiplicity-Strategy" ) {

   }

   ExclusiveMultStrategy::~ExclusiveMultStrategy() {

   }

   /**
    * This function takes care of creating the multiplicity word from a
    * muon candidate in an exclusive way.
    *
    * @param pt The p<sub>T</sub> threshold of the candidate
    * @param id The sector that produced the muon candidate (ignored)
    * @returns The multiplicity word from one muon candidate
    */
   PtMultiplicitySet
   ExclusiveMultStrategy::calculateMultiplicity( unsigned int pt,
                                                 const SectorID&,
                                                 bool ) const {

      PtMultiplicitySet result;

      //
      // Increment only this one threshold:
      //
      result.addMuon( pt );

      return result;

   }

} // namespace LVL1MUCTPI
