/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERING_HeavyIonStreamingStrategy_h
#define TRIGSTEERING_HeavyIonStreamingStrategy_h
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigSteering/IStreamingStrategy.h"


namespace HLT {
  class HeavyIonStreamingStrategy: public AthAlgTool, public virtual IStreamingStrategy {
  public:
    HeavyIonStreamingStrategy(const std::string& name, const std::string& type,
			      const IInterface* parent);
    virtual ~HeavyIonStreamingStrategy() {}

    // the empty implementation (by default no modifications to the streaming decision are made!)
    virtual void apply(std::vector<TriggerInfo::StreamTag>&) const;

  private:
    std::string m_lowPriority;
    std::string m_override;
  };
}


#endif
