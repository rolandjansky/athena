// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MultiplicityStrategy.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_MIOCT_MULTIPLICITYSTRATEGY_H
#define TRIGT1MUCTPI_MIOCT_MULTIPLICITYSTRATEGY_H

// STL include(s):
#include <string>

// Local include(s):
#include "../Common/PtMultiplicitySet.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class SectorID;

   /**
    *    @date $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
    *
    *   @short Base class for handling different kinds of multiplicity treatments
    *
    *          The MIOCT boards calculate the multiplicities of muons in an
    *          octant of the detector. The resulting multiplicities are represented
    *          by a 6x3 bit multiplicity word. This multiplicity word can be
    *          incremented in a number of ways. If the MIOCT sees a muon candidate
    *          passing threshold "3", it can either just increment the corresponding
    *          multiplicity in the multiplicity word, it can increment the
    *          multiplicities corresponding to threshold "1", "2" and "3", or it can
    *          basically increment any of the multiplicities.
    *
    *          This interface is used in the simulation to determine how the
    *          multiplicity words should be created.
    *
    *  @author $Author: krasznaa $
    * @version $Revision: 362102 $
    */
   class MultiplicityStrategy {

   public:
      /// Constructor specifying a strategy name
      /**
       * @param name The name of the derived strategy. Printed by the simulation code.
       */
      MultiplicityStrategy( const std::string& name ) : m_name( name ) {}
      /// Destructor
      virtual ~MultiplicityStrategy() {}

      /// Multiplicity handling function
      /**
       * This function creates a PtMultiplicitySet object from one muon candidate
       * coming from the specified sector. The MIOCTs are able to calculate the
       * multiplicities from each sector differently. (For instance they can mask
       * noisy sectors.)
       *
       * @param pt The p<sub>T</sub> threshold of the candidate
       * @param id The sector that produced the muon candidate
       * @param firstCandidate <code>true</code> if the candidate is the first one in
       *                       the sector, <code>false</code> otherwise
       * @returns The multiplicity word from one muon candidate
       */
      virtual PtMultiplicitySet calculateMultiplicity( unsigned int pt,
                                                       const SectorID& id,
                                                       bool firstCandidate ) const = 0;

      /// Get the name of the strategy
      /**
       * The simulation uses this function to print what kind of strategy
       * is being used to treat the muon multiplicities.
       *
       * @returns The name of the multiplicity strategy
       */
      const std::string& getName() const { return m_name; }

   private:
      /// Name of the multiplicity strategy
      std::string m_name;

   }; // class MultiplicityStrategy

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIOCT_MULTIPLICITYSTRATEGY_H
