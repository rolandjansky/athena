/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <algorithm>
#include <functional>

// Local include(s):
#include "count_bunch_neighbors.h"
#include "TrigBunchCrossingTool/BunchCrossing.h"
#include "unary_compose.h"

namespace Trig {

   count_bunch_neighbors::
   count_bunch_neighbors( const std::vector< int >& bunches,
                          int maxBunchSpacing )
      : m_bunches( bunches ), m_maxBunchSpacing( maxBunchSpacing ) {

   }

   int count_bunch_neighbors::operator()( int bunch ) const {

      // Count how many neighbors the bunch has:
      int neighbors =
         std::count_if( m_bunches.begin(), m_bunches.end(),
                        compose1( std::bind2nd( std::less_equal< int >(),
                                                m_maxBunchSpacing ),
                                  std::bind1st( std::ptr_fun( Trig::distance ),
                                                bunch ) ) );

      // Remember that the above expression always counts the bunch itself
      // as its own neighbor. So the real value we're looking for is 1 less.
      return ( neighbors - 1 );
   }

} // namespace Trig
