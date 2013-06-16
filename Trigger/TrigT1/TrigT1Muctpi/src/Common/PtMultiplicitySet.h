// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PtMultiplicitySet.h 364083 2011-05-06 09:09:55Z krasznaa $
#ifndef TRIGT1MUCTPI_PTMULTIPLICITYSET_H
#define TRIGT1MUCTPI_PTMULTIPLICITYSET_H

// STL include(s):
#include <string>

// Local include(s):
#include "SectorConstants.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class InclusiveMultStrategy;
   class ExclusiveMultStrategy;
   class XMLMultCalculator;

   /**
    *  @short A set of pt-multiplicities corresponding to the six different pt-thresholds
    *
    *         This   class contains a    set of  6 muon   multiplicities
    *         corresponding  to the  six  different pt-thresholds in the
    *         system.  The multiplicities  saturate at a  count of seven
    *         like it is implemented in the hardware (3 bits). Operators
    *         for adding multiplicities are defined.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 364083 $
    * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
    */
   class PtMultiplicitySet {

      // All the concrete multiplicity strategies have to be listed as friends,
      // because only they should be allowed to freely modify such objects.
      // I really don't like having to do it like this, but unfortunately
      // friendship is not inherited. Even though it would be elegant to just
      // write "friend class MultiplicityStrategy;"...
      friend class InclusiveMultStrategy;
      friend class ExclusiveMultStrategy;
      friend class XMLMultCalculator;

   public:
      /**
       * The constructor without any argument creates an empty multiplicty
       * set (all multiplicities set to 0). It might take an array with
       * initial values as an argument. Currently no check is performed if
       * the inital values do not exceed the maximal number of seven!!)
       */
      PtMultiplicitySet( unsigned int ptarr[ MAX_NUMBER_OF_THRESHOLDS ] = 0 );
      /**
       * Method to access the multiplicity of a particular threshold
       * @param threshold for which multiplicity is requested
       * @return multiplicity for requested threshold
       */
      int getMultiplicity( const unsigned int threshold ) const;

      /// Assignment operator
      PtMultiplicitySet& operator=( const PtMultiplicitySet& );
      /// This operator clippes the multiplicities at a count of sevenn
      PtMultiplicitySet& operator+=( const PtMultiplicitySet& );
      /// This operator clippes the multiplicities at a count of seven.
      PtMultiplicitySet operator+( const PtMultiplicitySet& );

      /// Reset the object to 0 multiplicity
      void clear();

      /**
       * For debugging and to look at results.
       * @param result is a reference on a string which after the call
       *               to the function contains the formatted result (six multiplicities for
       *               the six different pt-thresholds).
       */
      void print( std::string& result ) const;

   private:
      /**
       * With this member function a multiplicity of a specified threshold
       * can be incremented by one.
       * @param pt This is the threshold which is incremented. It must be
       *           a number between one and six. If this condition is not met, nothing
       *           will be done.
       */
      void addMuon( const unsigned int pt );

      unsigned int m_multiplicity_pt[ MAX_NUMBER_OF_THRESHOLDS ];

  }; // class PtMultiplicitySet

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_PTMULTIPLICITYSET_H
