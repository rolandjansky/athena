/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfHLTDataDict
#define TrigConfHLTDataDict

#include "TrigConfHLTData/HLTPrescale.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTPrescaleSetCollection.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTTriggerType.h"
#include "TrigConfHLTData/HLTStreamTag.h"

// ROOT 6's dictionay generator can't seem to be able to deal with
// multi_index_container...
#ifndef __CLING__
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#endif // __CLING__

#ifdef __GCCXML__
#include <vector>
namespace {
   std::vector<TrigConf::HLTChain*> vec_of_chains;
   std::vector<TrigConf::HLTSequence*> vec_of_sequencese;
}
#endif

#endif
