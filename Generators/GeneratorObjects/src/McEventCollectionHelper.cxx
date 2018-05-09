/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorObjects/McEventCollectionHelper.h"
#include "EventInfo/PileUpTimeEventIndex.h"

/** @class McEventCollectionHelper McEventCollectionHelper.cxx "McEventCollectionHelper.cxx"
 *
 *  Abstract interface to service for mapping pileUpTypes onto McEventCollection names
 */

EBC_EVCOLL McEventCollectionHelper::getMcEventCollectionHMPLEnumFromPileUpType(const int pileupType) {
    switch(pileupType)      {
      case PileUpTimeEventIndex::Signal: return EBC_MAINEVCOLL; break;
      case PileUpTimeEventIndex::MinimumBias: return EBC_FIRSTPUEVCOLL; break;
      case PileUpTimeEventIndex::HighPtMinimumBias: return EBC_SECONDPUEVCOLL; break;
      case PileUpTimeEventIndex::Cavern: return EBC_THIRDPUEVCOLL; break;
    }
    std::cout << "Warning: McEventCollectionHelper::getMcEventCollectionHMPLEnumFromPileUpType(" << pileupType << "): Unexpected pileupType!!" << std::endl;
    return EBC_MAINEVCOLL;
}

std::string McEventCollectionHelper::getMcEventCollectionNameFromPileUpType(const int pileupType) {
    switch(pileupType)      {
      case PileUpTimeEventIndex::Signal:            return "TruthEvent";           break;
      case PileUpTimeEventIndex::MinimumBias:       return "TruthEvent_PU";        break;
      case PileUpTimeEventIndex::HighPtMinimumBias: return "TruthEvent_HighPtPU";  break;
      case PileUpTimeEventIndex::Cavern:            return "TruthEvent_Cavern";    break;
    }
    std::cout << "Warning: McEventCollectionHelper::getMcEventCollectionNameFromPileUpType(" << pileupType << "): Unexpected pileupType!!" << std::endl;
    return "";
}

