/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERING_DefaultStreamingStrategy_h
#define TRIGSTEERING_DefaultStreamingStrategy_h
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigSteering/IStreamingStrategy.h"

namespace HLT {
  class DefaultStreamingStrategy: public AthAlgTool, public virtual IStreamingStrategy {
  public:
    DefaultStreamingStrategy(const std::string& name, const std::string& type,
			     const IInterface* parent) : AthAlgTool(name, type, parent) {
      declareInterface<HLT::IStreamingStrategy>( this ); 
    }
    virtual ~DefaultStreamingStrategy() {} 
    
    // the empty implementation (by default no modifications to the streaming decision are made!)
    virtual void apply(std::vector<TriggerInfo::StreamTag>&) const {}
  };
}


#endif
