// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTFFOverlapCalculator.h 650693 2015-03-01 16:53:48Z masato $
#ifndef TRIGT1MUCTPI_LUTFFOVERLAPCALCULATOR_H
#define TRIGT1MUCTPI_LUTFFOVERLAPCALCULATOR_H

// Local include(s):
#include "LUTOverlapCalculatorBase.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class ForwardSector;

   /**
    *  @short Class flagging forward muon candidates for fake double-counts
    *
    *         This class can be used to flag the one of the muon candidates in
    *         an overlapping forward sector pair if they are believed to be coming
    *         from the same muon. The muon with the higher p<sub>T</sub> is left
    *         un-flagged.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 650693 $
    * $Date: 2015-03-01 17:53:48 +0100 (Sun, 01 Mar 2015) $
    */
   class LUTFFOverlapCalculator : public LUTOverlapCalculatorBase {

   public:
      /// Default constructor
      LUTFFOverlapCalculator();
      /// Copy constructor
      LUTFFOverlapCalculator( const LUTFFOverlapCalculator& calc );

      /// Function initializing the calculator object
     virtual StatusCode initialize( const xercesc::DOMNode* ffnode, bool dumpLut,const std::string& runPeriod );

      /// Calculate the overlap flags for two barrel sectors
      void calculate( const ForwardSector& sector1, const ForwardSector& sector2 ) const;

   private:
      //
      // The LUT words take the form:
      //
      // variable  |             RoI2            |             RoI1            |
      //      bit  | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |

   }; // class LUTFFOverlapCalculator

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_LUTFFOVERLAPCALCULATOR_H
