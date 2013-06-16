// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: InclusiveMultStrategy.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_MIOCT_INCLUSIVEMULTSTRATEGY_H
#define TRIGT1MUCTPI_MIOCT_INCLUSIVEMULTSTRATEGY_H

// Local include(s):
#include "MultiplicityStrategy.h"

namespace LVL1MUCTPI {

   /**
    *    @date $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
    *
    *   @short Inclusive multiplicity handling strategy
    *
    *          This strategy creates the multiplicity words by incrementing the
    *          multiplicity of the passed threshold, and those of the thresholds
    *          "below" the passed threshold. So for a candidate passing threshold
    *          "3", it would create a multiplicity word:
    *
    *          <verbatim>
    *          000 000 000 001 001 001
    *          </verbatim>
    *
    *  @author $Author: krasznaa $
    * @version $Revision: 362102 $
    */
   class InclusiveMultStrategy : public MultiplicityStrategy {

   public:
      /// Constructor
      InclusiveMultStrategy();
      /// Destructor
      ~InclusiveMultStrategy();

      /// Multiplicity handling function
      virtual PtMultiplicitySet calculateMultiplicity( unsigned int pt,
                                                       const SectorID& id,
                                                       bool firstCandidate ) const;

   }; // class InclusiveMultStrategy

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIOCT_INCLUSIVEMULTSTRATEGY_H
