// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTOverlapCalculatorBase.h 707664 2015-11-13 05:36:00Z ssnyder $
#ifndef TRIGT1MUCTPI_LUTOVERLAPCALCULATORBASE_H
#define TRIGT1MUCTPI_LUTOVERLAPCALCULATORBASE_H

// STL include(s):
#include <map>
#include <string>

// XercesC include(s):
#include <xercesc/dom/DOMNode.hpp>

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"

// Local include(s):
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   class LUTOverlapCalculatorBase;
   bool operator== ( const LUTOverlapCalculatorBase& calc1,
                     const LUTOverlapCalculatorBase& calc2 );

   /**
    *  @short Base class for the LUT overlap calculators
    *
    *         All overlap calculators share some common properties. This class
    *         tries to group them, to minimize code duplication.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 707664 $
    * $Date: 2015-11-13 06:36:00 +0100 (Fri, 13 Nov 2015) $
    */
   class LUTOverlapCalculatorBase {

   public:
      /// "Regular" constructor
      LUTOverlapCalculatorBase( const std::string& name );
      /// Copy constructor
      LUTOverlapCalculatorBase( const LUTOverlapCalculatorBase& calc );
      LUTOverlapCalculatorBase  & operator = ( const LUTOverlapCalculatorBase  & ) = delete;
 
      /// Virtual destructor to make vtable happy
      virtual ~LUTOverlapCalculatorBase() {}

      /// Common initialization function for all kinds of calculator types
      /**
       * Calculators are always initialized from XML files. The owner of the
       * calculator object is responsible for giving the calculator the
       * XML node that describes the configuration for the object.
       *
       * @param node The XML node that holds the configuration for the object
       * @param dumpLut When set to <code>true</code> the calculator will print
       *                the contents of the read LUT. For debugging only.
       */
     virtual StatusCode initialize( const xercesc::DOMNode* node, bool dumpLut, const std::string& runPeriod ) = 0;

      /// Set the maximal threshold for overlap removal
      /**
       * The hardware uses a small LUT that defines which threshold combinations
       * can be taken into account when evaluating a fake double count. However in
       * practice we always only use a very simple configuration. We take the lowest
       * N thresholds into account and ignore the higher threshold candidates. This
       * function defined the maximal threshold that should still be taken into
       * account.
       */
      void setMaximumThreshold( int thr );

   protected:
      /// For printing messages
      mutable MsgLogger m_logger;

      /// Each overlap calculator should work with such a "look up table"
      std::map< unsigned int, bool > m_lut;

      /// Maximal threshold to be taken into account in overlap removal
      int m_thr;

      /// This operator helps to compare all "simple" overlap calculators
      friend bool LVL1MUCTPI::operator== ( const LUTOverlapCalculatorBase& calc1,
                                           const LUTOverlapCalculatorBase& calc2 );

   }; // class LUTOverlapCalculatorBase

   /// Operator comparing overlap calculator objects
   bool operator== ( const LUTOverlapCalculatorBase& calc1,
                     const LUTOverlapCalculatorBase& calc2 );

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_LUTOVERLAPCALCULATORBASE_H
