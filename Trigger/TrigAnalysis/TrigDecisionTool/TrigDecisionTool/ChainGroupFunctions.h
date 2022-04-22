/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDecisionTool_ChainGroupFunctions_h
#define TrigDecisionTool_ChainGroupFunctions_h
/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @class  : ChainGroupFunctions
 *
 * @brief Chain Group management
 *
 * @author Michael Begel  <michael.begel@cern.ch>  - Brookhaven National Laboratory
 * @author Tomasz Bold    <Tomasz.Bold@cern.ch>    - UC Irvine - AGH Krakow
 * @author Joerg Stelzer  <Joerg.Stelzer@cern.ch>  - DESY
 *
 ***********************************************************************************/
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "TrigDecisionInterface/GroupProperties.h"

namespace Trig {

  class ChainGroup;
  class CacheGlobalMemory;

  class ChainGroupFunctions
    {
    public:
      // constructors, destructor
      ChainGroupFunctions() = default;
      virtual ~ChainGroupFunctions() = default;

      /// Create/get chain group (@see CacheGlobalMemory)
      const Trig::ChainGroup* getChainGroup(const std::vector< std::string >& patterns,
                                            TrigDefs::Group props = TrigDefs::Group::Default) const;

      /// Create/get chain group (@see CacheGlobalMemory)
      const Trig::ChainGroup* getChainGroup(const std::string& pattern,
                                            TrigDefs::Group props = TrigDefs::Group::Default) const;

    protected:

      void ChainGroupInitialize();

    private:

      virtual Trig::CacheGlobalMemory* cgm() = 0;
      virtual const Trig::CacheGlobalMemory* cgm() const = 0;
      
    };
}
#endif
