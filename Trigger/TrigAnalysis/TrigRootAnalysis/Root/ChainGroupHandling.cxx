/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ChainGroupHandling.cxx 502443 2012-05-25 10:43:25Z krasznaa $

// Local include(s):
#include "TrigRootAnalysis/ChainGroupHandling.h"

namespace D3PD {

   namespace Trig {

      /**
       * Create a ChainGroup object from a list of trigger patterns. It's possible to use
       * regular expressions. For instance the following creates a chain group of all the
       * EF muon chains: ["EF_mu.*"].
       *
       * @param patterns A list of trigger name patterns
       * @returns A ChainGroup object describing the requested triggers
       */
      ChainGroup
      ChainGroupHandling::GetChainGroup( const std::vector< std::string >& patterns ) {

         return ChainGroup( patterns, *this, *this );
      }

      /**
       * This overloaded function can be helpful when you only want to use one trigger
       * name pattern. Notice that it's possible to give a comma separated list of trigger
       * names to the function. So the following is valid for instance:
       * "L1_MU6,L1_EM5,L1_J.*".
       *
       * @param patterns A list of trigger name patterns
       * @returns A ChainGroup object describing the requested triggers
       */
      ChainGroup ChainGroupHandling::GetChainGroup( const std::string& pattern ) {

         std::vector< std::string > patterns; patterns.push_back( pattern );
         return GetChainGroup( patterns );
      }

   } // namespace Trig

} // namespace D3PD
