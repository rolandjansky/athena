/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_RATEABC_H
#define ANP_RATEABC_H


/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : RateABC
 * @Author : Rustem Ospanov
 *
 * @Brief  : Interface class for accessing trigger rates results
 * 
 **********************************************************************************/

// Local
#include "TrigCostBase/Handle.h"

// Local
#include "TrigCostRate/RateChain.h"
#include "TrigCostRate/RateGroup.h"
#include "TrigCostRate/RateBand.h"

namespace Anp
{
  class Registry;

  class RateABC: public virtual Base {
  public:

    RateABC() {}
    virtual ~RateABC() {}
    
    virtual std::vector<Anp::Handle<Anp::RateChain> >& GetRateChains() = 0;
    virtual std::vector<Anp::Handle<Anp::RateChain> >& GetPTOnlyRate() = 0;
    virtual std::vector<Anp::Handle<Anp::RateChain> >& GetMultiSeeds() = 0;
    virtual std::vector<Anp::Handle<Anp::RateGroup> >& GetRateGroups() = 0;
    virtual std::vector<Anp::Handle<Anp::RateGroup> >& GetRateUnions() = 0;
    virtual std::vector<Anp::Handle<Anp::RateGroup> >& GetRateCumuls() = 0;
    virtual std::vector<Anp::Handle<Anp::RateGroup> >& GetRateLevels() = 0;
    virtual std::vector<Anp::Handle<Anp::RateGroup> >& GetRateStream() = 0;

    virtual const Registry& GetIndexRegistry() const = 0;
    
    virtual Handle<RateBand> GetRateBand() const { return Handle<RateBand>(); }

    virtual TrigMonL1Item::Decision GetDecisionBitLV1() const = 0;
    virtual TrigMonChain::Decision  GetDecisionBitHLT() const = 0;

    virtual double GetScale() const = 0;
    virtual double GetTotal() const = 0;
  };
}

#endif
