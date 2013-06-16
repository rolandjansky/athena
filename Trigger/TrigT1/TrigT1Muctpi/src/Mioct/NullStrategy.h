// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: NullStrategy.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_NULLSTRATEGY_H
#define TRIGT1MUCTPI_NULLSTRATEGY_H

// Local include(s):
#include "../Logging/MsgLogger.h"
#include "../Common/PtMultiplicitySet.h"
#include "OverlapStrategy.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class MultiplicityLogic;

   /**************************************************************************
    *
    *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
    *
    *    @short No Overlap is considered while forming muon multiplicities for octanat boards.
    *
    *           When forming pt-multiplicies for octant boards with this
    *           strategy active, all muon candidates of all sectors are
    *           considered irrespectively wether or not they are in
    *           overlapping regions.
    *
    *      @see OverlapStrategy
    *      @see PtMultiplicitySet
    *      @see MioctSectorSet
    *   @author $Author: krasznaa $
    *  @version $Revision: 362102 $
    *
    *
    ************************************************************************
    */
   class NullStrategy : public OverlapStrategy {

   public:
      NullStrategy( const MultiplicityLogic* logic );
      ~NullStrategy();
      /**
       * The implentation of the function to calculate the muon
       * multiplicities without consideration of overlapping muon
       * candidates.
       * @return PtMultipliciySet is the result of the calculation.
       * @param sectorData references to the Sectors of one MioctModule.
       * @param id         Not used in this implementation of OverlapStrategy
       */
      virtual PtMultiplicitySet calculateMultiplicity ( const MioctSectorSet& sectorData,
                                                        const MioctID& id ) const;

   private:
      MsgLogger m_errorLogger;
      const MultiplicityLogic* m_multiplicityLogic;

   }; // class NullStrategy

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_NULLSTRATEGY_H
