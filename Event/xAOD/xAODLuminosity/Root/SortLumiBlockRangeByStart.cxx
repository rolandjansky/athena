/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SortLumiBlockRangeByStart.cxx 647501 2015-02-17 14:22:33Z krasznaa $

// Local include(s):
#include "xAODLuminosity/SortLumiBlockRangeByStart.h"

namespace xAOD {

   bool SortLumiBlockRangeByStart::operator()( const LumiBlockRange& lbr1,
                                               const LumiBlockRange& lbr2 ) {

      if( lbr1.startRunNumber() != lbr2.startRunNumber() ) {
         return lbr1.startRunNumber() < lbr2.startRunNumber();
      } else {
         return lbr1.startLumiBlockNumber() < lbr2.startLumiBlockNumber();
      }
   }

   bool SortLumiBlockRangeByStart::operator()( const LumiBlockRange* lbr1,
                                               const LumiBlockRange* lbr2 ) {

      return this->operator()( *lbr1, *lbr2 );
   }

} // namespace xAOD
