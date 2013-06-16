// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTBBOverlapCalculator.h 439423 2011-05-25 21:22:40Z krasznaa $
#ifndef TRIGT1MUCTPI_LUTBBOVERLAPCALCULATOR_H
#define TRIGT1MUCTPI_LUTBBOVERLAPCALCULATOR_H

// Local include(s):
#include "LUTOverlapCalculatorBase.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class BarrelSector;

   /**
    *  @short Class flagging barrel candidates for barrel-barrel overlaps
    *
    *         This is one of the end-classes of the new LUT overlap handling. It can be
    *         initialized by passing the correct DOMNode to it in it's initialize method.
    *         When two barrel candidates are given to it, it can flag one of them to be
    *         supressed in the multiplicity count.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 439423 $
    * $Date: 2011-05-25 23:22:40 +0200 (Wed, 25 May 2011) $
    */
   class LUTBBOverlapCalculator : public LUTOverlapCalculatorBase {

   public:
      /// Default constructor
      LUTBBOverlapCalculator();
      /// Copy constructor
      LUTBBOverlapCalculator( const LUTBBOverlapCalculator& calc );

      /// Function initializing the calculator object
      virtual StatusCode initialize( const xercesc::DOMNode* bbnode, bool dumpLut );

      /// Calculate the overlap flags for two barrel sectors
      void calculate( const BarrelSector& sector1, const BarrelSector& sector2 ) const;

   private:
      //
      // The LUT words take the form:
      //
      // variable  |Ovl2|          RoI2          |Ovl1|          RoI1          |
      //      bit  | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |

   }; // class LUTBBOverlapCalculator

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_LUTBBOVERLAPCALCULATOR_H
