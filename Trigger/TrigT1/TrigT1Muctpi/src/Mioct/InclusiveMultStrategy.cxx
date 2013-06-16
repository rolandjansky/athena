/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: InclusiveMultStrategy.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// Local include(s):
#include "InclusiveMultStrategy.h"
#include "../Common/SectorConstants.h"

namespace LVL1MUCTPI {

   InclusiveMultStrategy::InclusiveMultStrategy()
      : MultiplicityStrategy( "Inclusive-Multiplicity-Strategy" ) {

   }

   InclusiveMultStrategy::~InclusiveMultStrategy() {

   }

   /**
    * This function takes care of creating the multiplicity word from a
    * muon candidate in an inclusive way.
    *
    * @param pt The p<sub>T</sub> threshold of the candidate
    * @param id The sector that produced the muon candidate (ignored)
    * @returns The multiplicity word from one muon candidate
    */
   PtMultiplicitySet
   InclusiveMultStrategy::calculateMultiplicity( unsigned int pt,
                                                 const SectorID&,
                                                 bool ) const {

      PtMultiplicitySet result;

      //
      // If a candidate is not created, that's signalled by giving it the pt threshold 7.
      // So the multiplicity should only be changed if the given threshold is in the
      // correct range.
      //
      if( ( pt >= 1 ) && ( pt <= MAX_NUMBER_OF_THRESHOLDS ) ) {
         for( unsigned int i = 1; i <= pt; ++i ) {
            result.addMuon( i );
         }
      }

      return result;

   }

} // namespace LVL1MUCTPI
