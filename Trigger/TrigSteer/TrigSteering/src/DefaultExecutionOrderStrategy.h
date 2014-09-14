/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERING_DefaultExecutionOrderStrategy_h
#define TRIGSTEERING_DefaultExecutionOrderStrategy_h
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigSteering/IExecutionOrderStrategy.h"

namespace HLT {
  class DefaultExecutionOrderStrategy: public AthAlgTool, public virtual IExecutionOrderStrategy {
  public:
    DefaultExecutionOrderStrategy(const std::string& name, const std::string& type,
			     const IInterface* parent) : AthAlgTool(name, type, parent) {
      declareInterface<HLT::IExecutionOrderStrategy>( this ); 
    }
    virtual ~DefaultExecutionOrderStrategy() {} 
    
    // the empty implementation (by default no modifications to the execution ordere is applied!)
    virtual StatusCode prepare(std::vector<HLT::SteeringChain*>&)  { return StatusCode::SUCCESS; }
    virtual void apply(std::vector<HLT::SteeringChain*>&) const {}
  };
}


#endif
