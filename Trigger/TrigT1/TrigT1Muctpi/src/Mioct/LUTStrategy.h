// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTStrategy.h 701446 2015-10-19 15:19:09Z wengler $
#ifndef TRIGT1MUCTPI_LUTSTRATEGY_H
#define TRIGT1MUCTPI_LUTSTRATEGY_H

// STL include(s):
#include <map>
#include <list>
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"

// Local include(s):
#include "OverlapStrategy.h"
#include "MioctID.h"
#include "LUTMultiplicityCalculator.h"
#include "../Common/PtMultiplicitySet.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class MultiplicityLogic;

   /**
    *  @short Overlap strategy for the hardware's LUT based overlap removal
    *
    *         This overlap strategy simulates the final hardware's overlap
    *         removal logic. It is initialized with the same XML files as the
    *         hardware is.
    *
    * @authos Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 701446 $
    * $Date: 2015-10-19 17:19:09 +0200 (Mon, 19 Oct 2015) $
    */
   class LUTStrategy : public OverlapStrategy {

   public:
      /// Standard constructor, specifying a multiplicity logic
      LUTStrategy( const MultiplicityLogic* logic );
      /// Copy constructor
      LUTStrategy( const LUTStrategy& strategy );
      LUTStrategy  & operator = ( const LUTStrategy  & ) = delete;

      /// Function initializing the strategy object
      StatusCode initialize( const std::string& lutXMLFile, bool flagMode,
                             bool dumpLut, const std::string& runPeriod );

      /// Function calculating the multiplicity for one MIOCT
      virtual PtMultiplicitySet calculateMultiplicity( const MioctSectorSet& data,
                                                       const MioctID& mioct ) const;

   private:
      mutable MsgLogger m_logger;
      const MultiplicityLogic* m_multiplicityLogic;
      std::map< MioctID, const LUTMultiplicityCalculator* > m_calculator_map;
      std::list< LUTMultiplicityCalculator > m_calculator_list;

   }; // LUTStrategy

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_LUTSTRATEGY_H
