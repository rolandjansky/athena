// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ChainGroupHandling.h 502443 2012-05-25 10:43:25Z krasznaa $
#ifndef TRIGROOTANALYSIS_CHAINGROUPHANDLING_H
#define TRIGROOTANALYSIS_CHAINGROUPHANDLING_H

// STL include(s):
#include <vector>
#include <string>

// Local include(s):
#include "IDataAccess.h"
#include "IConfigAccess.h"
#include "IITrigDecisionToolD3PD.h"
#include "ChainGroup.h"

namespace D3PD {

   namespace Trig {

      /**
       *  @short Class implementing the functions that handle ChainGroup-s
       *
       *         The functionality of the TDT is split up in many pieces. This
       *         piece takes care of creating ChainGroup objects in the correct
       *         way.
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 502443 $
       * $Date: 2012-05-25 12:43:25 +0200 (Fri, 25 May 2012) $
       */
      class ChainGroupHandling : public virtual D3PD::IITrigDecisionToolD3PD,
                                 public virtual D3PD::Trig::IDataAccess,
                                 public virtual D3PD::Trig::IConfigAccess {

      public:
         /// Create a chain group from a list of patterns
         virtual D3PD::ChainGroup
         GetChainGroup( const std::vector< std::string >& patterns );
         /// Create a chain group from a single pattern
         virtual D3PD::ChainGroup GetChainGroup( const std::string& pattern );

      }; // class ChainGroupHandling

   } // namespace Trig

} // namespace D3PD

#endif // TRIGROOTANALYSIS_CHAINGROUPHANDLING_H
