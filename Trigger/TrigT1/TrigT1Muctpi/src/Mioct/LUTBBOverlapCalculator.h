// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTBBOverlapCalculator.h 650693 2015-03-01 16:53:48Z masato $
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
    * $Revision: 650693 $
    * $Date: 2015-03-01 17:53:48 +0100 (Sun, 01 Mar 2015) $
    */
   class LUTBBOverlapCalculator : public LUTOverlapCalculatorBase {

   public:
      /// Default constructor
      LUTBBOverlapCalculator();
      /// Copy constructor
      LUTBBOverlapCalculator( const LUTBBOverlapCalculator& calc );

      /// Function initializing the calculator object
     virtual StatusCode initialize( const xercesc::DOMNode* bbnode, bool dumpLut, const std::string& runPeriod );

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
