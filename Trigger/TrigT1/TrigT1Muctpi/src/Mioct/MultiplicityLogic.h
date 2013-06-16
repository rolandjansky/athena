// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MultiplicityLogic.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_MIOCT_MULTIPLICITYLOGIC_H
#define TRIGT1MUCTPI_MIOCT_MULTIPLICITYLOGIC_H

// STL include(s):
#include <map>

// Local include(s):
#include "../Common/MultiplicityCalcType.h"
#include "../Common/PtMultiplicitySet.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class SectorID;
   class MultiplicityStrategy;

   /**
    *    @date $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
    *
    *   @short Top level class handling the multiplicity summation
    *
    *          All the parts of the simulation that need to create
    *          PtMultiplicitySet objects, should create them using a
    *          correctly configured instance of such an object. It can
    *          be configured to use a variety of multiplicity handling
    *          strategies, so all the other parts of the code don't need
    *          to know how the multiplicities are handled at any given
    *          time.
    *
    *  @author $Author: krasznaa $
    * @version $Revision: 362102 $
    */
   class MultiplicityLogic {

   public:
      /// Constructor
      MultiplicityLogic();
      /// Destructor
      ~MultiplicityLogic();

      /// Change the multiplicity strategy
      void changeStrategy( MultiplicityCalcType type );
      /// Initialise the XML multiplicity strategy
      void initializeXML( const std::string& xmlFile );

      /// Create the multiplicity with the active strategy
      PtMultiplicitySet calculateMultiplicity( unsigned int pt,
                                               const SectorID& id,
                                               bool firstCandidate ) const;

   private:
      /// Message logging object
      MsgLogger                                               m_logger;
      /// Pointer to the currently active multiplicity strategy
      MultiplicityStrategy*                                   m_currentStrategy;
      /// A map holding all the available multiplicity strategies
      std::map< MultiplicityCalcType, MultiplicityStrategy* > m_strategyMap;

   }; // class MultiplicityLogic

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIOCT_MULTIPLICITYLOGIC_H
