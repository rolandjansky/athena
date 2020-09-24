/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <functional>
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "HeavyIonStreamingStrategy.h"
using namespace HLT;
HeavyIonStreamingStrategy::HeavyIonStreamingStrategy(const std::string& name, const std::string& type,
						     const IInterface* parent)
  : AthAlgTool(name, type, parent)  
{
  declareProperty("LowPriority", m_lowPriority="bulk", "The stream which should be removed if presence of the 'HighPriority'.");
  declareProperty("Override",    m_override="prompt",  "The stream which should be used to override the LowPriority one or removed if LowPriority is absent.");
  declareInterface<HLT::IStreamingStrategy>( this ); 
}
struct nameEquals {
  nameEquals(const std::string& x) { m_name = x; }
  std::string m_name;
  bool operator() (const TriggerInfo::StreamTag& x) { return m_name == x.name(); }
  
};


void HeavyIonStreamingStrategy::apply(std::vector<TriggerInfo::StreamTag>& streams) const {

  bool lowPrio_present  = find_if(streams.begin(), streams.end(), nameEquals(m_lowPriority) ) != streams.end();
  bool override_present = find_if(streams.begin(), streams.end(), nameEquals(m_override) ) != streams.end();

  // if override is present then it shoudl replace low priority one 
  // or be removed
  if ( override_present ) {
    if (lowPrio_present) {
      streams.erase(remove_if(streams.begin(), streams.end(), nameEquals(m_lowPriority)));
      ATH_MSG_DEBUG("The low priority stream was removed" );
    } else {
      streams.erase(remove_if(streams.begin(), streams.end(), nameEquals(m_override)));
      ATH_MSG_DEBUG("The override stream is removed because low priority is absent" );
    }
  }

}

