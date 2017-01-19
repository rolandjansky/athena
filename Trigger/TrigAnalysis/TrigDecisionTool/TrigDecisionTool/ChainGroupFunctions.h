/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDecisionTool_ChainGroupFunctions_h
#define TrigDecisionTool_ChainGroupFunctions_h
/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : ChainGroupFunctions
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
#include<boost/algorithm/string.hpp>

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"

namespace Trig {

  class ChainGroupFunctions
    {
    public:
      // constructors, destructor
      ChainGroupFunctions();
      virtual ~ChainGroupFunctions();
      
      const Trig::ChainGroup* getChainGroup(const std::vector< std::string >& patterns) const; //!< @see CacheGlobalMemory
      const Trig::ChainGroup* getChainGroup(const std::string& pattern) const;                 //!< @see CacheGlobalMemory

    protected:

      void ChainGroupInitialize();

    private:

      virtual Trig::CacheGlobalMemory* cgm() const =0; 
      
    };
}
#endif
