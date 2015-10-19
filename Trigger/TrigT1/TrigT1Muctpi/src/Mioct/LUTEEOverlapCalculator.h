// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTEEOverlapCalculator.h 701446 2015-10-19 15:19:09Z wengler $
#ifndef TRIGT1MUCTPI_LUTEEOVERLAPCALCULATOR_H
#define TRIGT1MUCTPI_LUTEEOVERLAPCALCULATOR_H

// Local include(s):
#include "LUTOverlapCalculatorBase.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class EndcapSector;

   /**
    *  @short Class flagging endcap muon candidates for fake double-counts
    *
    *         This class can be used to flag the one of the muon candidates in
    *         an overlapping endcap sector pair if they are believed to be coming
    *         from the same muon. The muon with the higher p<sub>T</sub> is left
    *         un-flagged.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 701446 $
    * $Date: 2015-10-19 17:19:09 +0200 (Mon, 19 Oct 2015) $
    */
   class LUTEEOverlapCalculator : public LUTOverlapCalculatorBase {

   public:
      /// Default constructor
      LUTEEOverlapCalculator();
      /// Copy constructor
      LUTEEOverlapCalculator( const LUTEEOverlapCalculator& calc );
      LUTEEOverlapCalculator  & operator = ( const LUTEEOverlapCalculator  & ) = delete;

      /// Function initializing the calculator object
     virtual StatusCode initialize( const xercesc::DOMNode* eenode, bool dumpLut,const std::string& runPeriod );

      /// Calculate the overlap flags for two endcap sectors
      void calculate( const EndcapSector& sector1, const EndcapSector& sector2 ) const;

   private:
      //
      // The LUT words take the form:
      //
      // variable  |                  RoI2                 |                  RoI1                 |
      //      bit  | 16 | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |

   }; // class LUTEEOverlapCalculator

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_LUTEEOVERLAPCALCULATOR_H
