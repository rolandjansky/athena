// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTMultiplicityCalculator.h 701446 2015-10-19 15:19:09Z wengler $
#ifndef TRIGT1MUCTPI_LUTMULTIPLICITYCALCULATOR_H
#define TRIGT1MUCTPI_LUTMULTIPLICITYCALCULATOR_H

// XercesC include(s):
#include <xercesc/dom/DOMNode.hpp>

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"

// Local include(s):
#include "LUTBBOverlapCalculator.h"
#include "LUTEEOverlapCalculator.h"
#include "LUTFFOverlapCalculator.h"
#include "LUTBEOverlapCalculator.h"
#include "LUTEFOverlapCalculator.h"
#include "../Common/PtMultiplicitySet.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class MioctSectorSet;
   class LUTMultiplicityCalculator;
   class MultiplicityLogic;

   bool operator== ( const LUTMultiplicityCalculator& calc1,
                     const LUTMultiplicityCalculator& calc2 );

   /**
    *  @short Class capable of calculating the multiplicity in one MIOCT
    *
    *         Each MIOCT can calculate its multiplicity using such an object.
    *         It is handled separately from the MIOCTs, as multiple MIOCTs can
    *         possibly share the same configuration. (Although in real life they
    *         don't really because of the RPC pad numbering scheme...)
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 701446 $
    * $Date: 2015-10-19 17:19:09 +0200 (Mon, 19 Oct 2015) $
    */
   class LUTMultiplicityCalculator {

   public:
      /// Constructor only taking a multiplicity logic
      LUTMultiplicityCalculator( const MultiplicityLogic* logic );
      /// Copy constructor
      LUTMultiplicityCalculator( const LUTMultiplicityCalculator& calculator );
      LUTMultiplicityCalculator  & operator = ( const LUTMultiplicityCalculator  & ) = delete;

      /// Function initializing the object from an XML file
      StatusCode initialize( const xercesc::DOMNode* node, bool flagMode,
                             bool dumpLut, const std::string& runPeriod );

      /// Function calculating the multiplicity for one MIOCT
      PtMultiplicitySet calculateMultiplicity( const MioctSectorSet& sectorData ) const;

  private:
      /// The message logger object
      mutable MsgLogger m_logger;

      /// Pointer to the multiplicity logic of this MIOCT
      const MultiplicityLogic* m_multiplicityLogic;

      /// When set to true, the multiplicity calculation is in flagging mode
      bool m_flagMode;

      //
      // The different overlap calculator/flagger objects:
      //
      LUTBBOverlapCalculator m_bb_calc_1, m_bb_calc_2;
      LUTEEOverlapCalculator m_ee_calc_1, m_ee_calc_2, m_ee_calc_3, m_ee_calc_4, m_ee_calc_5;
      LUTFFOverlapCalculator m_ff_calc_1, m_ff_calc_2;
      LUTBEOverlapCalculator m_be_calc_11, m_be_calc_12, m_be_calc_22, m_be_calc_23;
      LUTBEOverlapCalculator m_be_calc_34, m_be_calc_35, m_be_calc_45, m_be_calc_46;
      LUTEFOverlapCalculator m_ef_calc_21, m_ef_calc_31, m_ef_calc_42;
      LUTEFOverlapCalculator m_ef_calc_52, m_ef_calc_63;

      friend bool LVL1MUCTPI::operator== ( const LUTMultiplicityCalculator& calc1,
                                           const LUTMultiplicityCalculator& calc2 );

   }; // class LUTMultiplicityCalculator

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_LUTMULTIPLICITYCALCULATOR_H
