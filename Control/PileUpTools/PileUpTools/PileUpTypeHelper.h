#ifndef PILEUPTOOLS_PILEUPTYPEHELPER_H
#define PILEUPTOOLS_PILEUPTYPEHELPER_H

#include "EventInfo/PileUpEventInfo.h"
#include "xAODEventInfo/EventInfo.h"
#include <map>

namespace {
   PileUpEventInfo::SubEvent::pileup_type pileupTypeMapper(xAOD::EventInfo::PileUpType inType) {

    // A map translating between the AOD and xAOD pileup event types:
    static std::map< xAOD::EventInfo::PileUpType,
                     PileUpEventInfo::SubEvent::pileup_type > pileupTypeMap;
    if( ! pileupTypeMap.size() ) {
#define DECLARE_SE_TYPE( TYPE )                                         \
      pileupTypeMap[ xAOD::EventInfo::TYPE ] = PileUpEventInfo::SubEvent::pileup_type::TYPE

      DECLARE_SE_TYPE( Unknown );
      DECLARE_SE_TYPE( Signal );
      DECLARE_SE_TYPE( MinimumBias );
      DECLARE_SE_TYPE( Cavern );
      DECLARE_SE_TYPE( HaloGas );
      DECLARE_SE_TYPE( HighPtMinimumBias );
      DECLARE_SE_TYPE( ZeroBias );
#undef DECLARE_SE_TYPE
    }
    // And now add a sub-event to the temporary list:
    auto typeItr = pileupTypeMap.find( inType );
    PileUpEventInfo::SubEvent::pileup_type type = PileUpEventInfo::SubEvent::pileup_type::Unknown;
    if( typeItr != pileupTypeMap.end() ) {
      type = typeItr->second;
    }
    return type;
  }
}
#endif //PILEUPTOOLS_PILEUPTYPEHELPER_H
