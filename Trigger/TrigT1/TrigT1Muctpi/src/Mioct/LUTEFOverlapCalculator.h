// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTEFOverlapCalculator.h 701446 2015-10-19 15:19:09Z wengler $
#ifndef TRIGT1MUCTPI_LUTEFOVERLAPCALCULATOR_H
#define TRIGT1MUCTPI_LUTEFOVERLAPCALCULATOR_H

// Local include(s):
#include "LUTOverlapCalculatorBase.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class EndcapSector;
   class ForwardSector;

   /**
    *  @short Class flagging candidates for endcap-forward overlaps
    *
    *         This is one of the end-classes of the new LUT overlap handling. It can be
    *         initialized by passing the correct DOMNode to it in it's initialize method.
    *         When two candidates are given to it, it can flag one of them to be
    *         supressed in the multiplicity count.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 701446 $
    * $Date: 2015-10-19 17:19:09 +0200 (Mon, 19 Oct 2015) $
    */
   class LUTEFOverlapCalculator : public LUTOverlapCalculatorBase {

   public:
      /// Default constructor
      LUTEFOverlapCalculator();
      /// Copy constructor
      LUTEFOverlapCalculator( const LUTEFOverlapCalculator& calc );
      LUTEFOverlapCalculator  & operator = ( const LUTEFOverlapCalculator  & ) = delete;

      /// Function initializing the calculator object
     virtual StatusCode initialize( const xercesc::DOMNode* bbnode, bool dumpLut,const std::string& runPeriod );

      /// Calculate the overlap flags for two barrel sectors
      void calculate( const EndcapSector& esec, const ForwardSector& fsec ) const;

   private:
      //
      // The LUT words take the form:
      //
      // variable  |            FRoI             |                  ERoI                 |
      //      bit  | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |

   }; // class LUTEFOverlapCalculator

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_LUTEFOVERLAPCALCULATOR_H
