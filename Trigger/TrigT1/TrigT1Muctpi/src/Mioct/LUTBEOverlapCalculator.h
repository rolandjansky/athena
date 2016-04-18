// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTBEOverlapCalculator.h 701446 2015-10-19 15:19:09Z wengler $
#ifndef TRIGT1MUCTPI_LUTBEOVERLAPCALCULATOR_H
#define TRIGT1MUCTPI_LUTBEOVERLAPCALCULATOR_H

// Local include(s):
#include "LUTOverlapCalculatorBase.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class BarrelSector;
   class EndcapSector;

   /**
    *  @short Class flagging barrel or endcap muon candidates for fake double-counts
    *
    *         This class can be used to flag the one of the muon candidates in
    *         an overlapping barrel-endcap sector pair if they are believed
    *         to be coming from the same muon. The muon with the higher
    *         p<sub>T</sub> is left un-flagged.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 701446 $
    * $Date: 2015-10-19 17:19:09 +0200 (Mon, 19 Oct 2015) $
    */
   class LUTBEOverlapCalculator : public LUTOverlapCalculatorBase {

   public:
      /// Default constructor
      LUTBEOverlapCalculator();
      /// Copy constructor
      LUTBEOverlapCalculator( const LUTBEOverlapCalculator& calc );
      LUTBEOverlapCalculator  & operator = ( const LUTBEOverlapCalculator  & ) = delete;

      /// Function initializing the calculator object
     virtual StatusCode initialize( const xercesc::DOMNode* benode, bool dumpLut,const std::string& runPeriod );

      /// Calculate the overlap flags for two barrel sectors
      void calculate( const BarrelSector& bsector, const EndcapSector& esector ) const;

   private:
      //
      // The LUT words take the form:
      //
      // variable  | EQ |                 ERoI                  |      BPt     |          BRoI          |
      //      bit  | 17 | 16 | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |

   }; // class LUTBEOverlapCalculator

} // namespac LVL1MUCTPI

#endif // TRIGT1MUCTPI_LUTBEOVERLAPCALCULATOR_H
