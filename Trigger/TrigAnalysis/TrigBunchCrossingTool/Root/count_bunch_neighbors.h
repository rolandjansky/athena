// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBUNCHCROSSINGTOOL_COUNT_BUNCH_NEIGHBORS_H
#define TRIGBUNCHCROSSINGTOOL_COUNT_BUNCH_NEIGHBORS_H

// System include(s):
#include <vector>

namespace Trig {

   /// Functor counting how many neighbors a given bunch crossing has
   ///
   /// This is used as an unary predicate when counting how many bunches have
   /// neighbors closer than X. To determine the minimum bunch spacing of the
   /// configurations, before the code would start looking for single bunches
   /// and bunch trains.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class count_bunch_neighbors {

   public:
      /// Constructor with colliding bunches, and maximum bunch spacing
      count_bunch_neighbors( const std::vector< int >& bunches,
                             int maxBunchSpacing );

      /// Count how many neighbors can be found for the specified bunch crossing
      int operator()( int bunch ) const;

   private:
      /// Filled bunches in the current configuration
      const std::vector< int >& m_bunches;
      /// Maximum bunch spacing to consider
      const int m_maxBunchSpacing;

   }; // class count_bunch_neighbors

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_COUNT_BUNCH_NEIGHBORS_H
