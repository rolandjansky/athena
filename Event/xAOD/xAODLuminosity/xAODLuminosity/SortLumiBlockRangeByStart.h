// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SortLumiBlockRangeByStart.h 647501 2015-02-17 14:22:33Z krasznaa $
#ifndef XAODLUMINOSITY_SORTLUMIBLOCKRANGEBYSTART_H
#define XAODLUMINOSITY_SORTLUMIBLOCKRANGEBYSTART_H

// Local include(s):
#include "xAODLuminosity/LumiBlockRange.h"

namespace xAOD {

   /// Helper functor used to sort xAOD::LumiBlockRangeContainer-s
   ///
   /// As with all xAOD containers, one needs to be careful with using std::sort
   /// and friends. They should never be used on the containers that own their
   /// elements, only on view containers. Otherwise the performance of the code
   /// will degrade significantly.
   ///
   /// @author Marjorie Shapiro <mdshapiro@lbl.gov>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 647501 $
   /// $Date: 2015-02-17 15:22:33 +0100 (Tue, 17 Feb 2015) $
   ///
   class SortLumiBlockRangeByStart {

   public:
      /// Function ordering the xAOD::LumiBlockRange objects by starting time
      bool operator()( const LumiBlockRange& lbr1, const LumiBlockRange& lbr2 );
      /// Function ordering the xAOD::LumiBlockRange objects by starting time
      bool operator()( const LumiBlockRange* lbr1, const LumiBlockRange* lbr2 );

   }; // class SortLumiBlockRangeByStart

} // namespace xAOD

#endif // XAODLUMINOSITY_SORTLUMIBLOCKRANGEBYSTART_H
