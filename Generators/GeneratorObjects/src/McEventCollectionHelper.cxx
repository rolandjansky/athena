/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GeneratorObjects/McEventCollectionHelper.h"
#include "EventInfo/PileUpTimeEventIndex.h"

/** @class McEventCollectionHelper McEventCollectionHelper.cxx "McEventCollectionHelper.cxx"
 *
 *  Abstract interface to service for mapping pileUpTypes onto McEventCollection names
 */
EBC_EVCOLL McEventCollectionHelper::getMcEventCollectionHMPLEnumFromPileUpType(const int pileupType, MsgStream* log) {
    switch(pileupType) {
      case PileUpTimeEventIndex::Signal: return EBC_MAINEVCOLL; break;
      case PileUpTimeEventIndex::MinimumBias: return EBC_FIRSTPUEVCOLL; break;
      case PileUpTimeEventIndex::HighPtMinimumBias: return EBC_SECONDPUEVCOLL; break;
      case PileUpTimeEventIndex::Cavern: return EBC_THIRDPUEVCOLL; break;
    }
    if (log) {
      *log << MSG::WARNING
           << "McEventCollectionHelper::getMcEventCollectionHMPLEnumFromPileUpType Unexpected pileupType: "
           << pileupType << endmsg;
    }
    return EBC_MAINEVCOLL;
}

std::string McEventCollectionHelper::getMcEventCollectionNameFromPileUpType(const int pileupType, MsgStream* log) {
    switch(pileupType) {
      case PileUpTimeEventIndex::Signal:            return "TruthEvent";           break;
      case PileUpTimeEventIndex::MinimumBias:       return "TruthEvent_PU";        break;
      case PileUpTimeEventIndex::HighPtMinimumBias: return "TruthEvent_HighPtPU";  break;
      case PileUpTimeEventIndex::Cavern:            return "TruthEvent_Cavern";    break;
    }
    if (log) {
      *log << MSG::WARNING
           << "McEventCollectionHelper::getMcEventCollectionNameFromPileUpType Unexpected pileupType: "
           << pileupType << endmsg;
    }
    return "";
}

