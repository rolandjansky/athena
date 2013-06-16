// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ExclusiveMultStrategy.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_MIOCT_EXCLUSIVEMULTSTRATEGY_H
#define TRIGT1MUCTPI_MIOCT_EXCLUSIVEMULTSTRATEGY_H

// Local include(s):
#include "MultiplicityStrategy.h"

namespace LVL1MUCTPI {

   /**
    *    @date $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
    *
    *   @short Exclusive multiplicity handling strategy
    *
    *          This strategy creates the multiplicity words by only incrementing
    *          the multiplicity of the highest passed threshold. So for a candidate
    *          passing threshold "3", it would create the multiplicity word:
    *
    *          <verbatim>
    *          000 000 000 001 000 000
    *          </verbatim>
    *
    *          It's mostly used when the muon thresholds are in no direct relation
    *          with each other. For instance threshold "1" could be an RPC-only
    *          threshold, while threshold "2" is TGC-only.
    *
    *  @author $Author: krasznaa $
    * @version $Revision: 362102 $
    */
   class ExclusiveMultStrategy : public MultiplicityStrategy {

   public:
      /// Constructor
      ExclusiveMultStrategy();
      /// Destructor
      ~ExclusiveMultStrategy();

      /// Multiplicity handling function
      virtual PtMultiplicitySet calculateMultiplicity( unsigned int pt,
                                                       const SectorID& id,
                                                       bool firstCandidate ) const;

   }; // class ExclusiveMultStrategy

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIOCT_EXCLUSIVEMULTSTRATEGY_H
