// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OverlapLogic.h 650693 2015-03-01 16:53:48Z masato $
#ifndef TRIGT1MUCTPI_OVERLAPLOGIC_H
#define TRIGT1MUCTPI_OVERLAPLOGIC_H

// STL include(s):
#include <map>
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"

// Local include(s):
#include "../Logging/MsgLogger.h"
#include "StrategyName.h"
#include "OverlapStrategy.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class MioctSectorSet;
   class PtMultiplicitySet;
   class MultiplicityLogic;

   /**
    *  @short Contains the logic to resolve overlapping muon candidates during multiplicity formation.
    *
    *         The  OverlapLogic  is  responsible for installing  various
    *         strategies to  resolve the  overlap of different  sectors.
    *         It contains a map which associates all possible strategies
    *         to a StrategyName like it is defined in StrategyName.h.
    *
    *    @see OverlapStrategy
    *    @see NullStrategy
    *    @see MioctSectorSet
    *    @see PtMultiplicitySet
    *
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 650693 $
    * $Date: 2015-03-01 17:53:48 +0100 (Sun, 01 Mar 2015) $
    */
   class OverlapLogic {

   public:
      /**
       * The construcor builds up a map containing an instance of all
       * possible overlap strategies mapped to their corresponding names.
       * The constructor by defaults sets the current strategy to the
       * NullStrategy.
       * @see OverlapStrategy
       * @see NullStrategy
       * @see StrategyName.h
       */
      OverlapLogic( MultiplicityLogic* logic );
      /**
       * Simple destructor.
       */
      ~OverlapLogic();
      /**
       * This function lets the user change the Overlap Strategy used to
       * resolve overlapping muon candidates.
       * @param newStrategy is a StrategyName as defined in StrategyName.h
       * @see StrategyName.h
       */
      void changeStrategy( StrategyName newStrategy );
      /**
       * This functions returns the name of the currently selected
       * strategy to resolve overlap.
       */
      std::string getCurrentStrategyName() const { return m_currentStrategy->getName(); }
      /**
       * This member function calculates the pt multiplicity of an octant board
       * under consideration of the current OverlapStrategy.
       * @param mioctData is a reference to a set of Sectors belonging
       *                  to one MIOCT board.
       * @param mioct     Defines which MIOCT the data is coming from
       * @see MioctSectorSet
       */
      PtMultiplicitySet calculateMultiplicity( const MioctSectorSet& mioctData,
                                               const MioctID& mioct ) const;
      /**
       * This function breaks the model a bit in that it provides a direct connection
       * between one of the OverlapStrategy implementations and the user.
       *
       * @param lutXMLFile Full path name to the XML file to load
       * @param flagMode Controls the flagging mode of the LUT strategy
       * @param dumpLut When set to true, detailed information is printed about the
       *                loaded LUT
       */
      StatusCode initializeLUT( const std::string& lutXMLFile, bool flagMode,
                                bool dumpLut, const std::string& runPeriod );

   private:
      mutable MsgLogger                          m_logger;
      MultiplicityLogic*                         m_multiplicityLogic;
      OverlapStrategy*                           m_currentStrategy;
      std::map< StrategyName, OverlapStrategy* > m_overlapStrategyMap;

   }; // class OverlapLogic

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_OVERLAPLOGIC_H
