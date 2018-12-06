/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigConfHLTDataDict
#define TrigConfHLTDataDict

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescale.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTPrescaleSetCollection.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTTriggerType.h"
#include "TrigConfHLTData/HLTStreamTag.h"

#ifdef __GCCXML__
#include <vector>
namespace {
   std::vector<TrigConf::HLTChain*> vec_of_chains;
   std::vector<TrigConf::HLTSequence*> vec_of_sequencese;
}
#endif

#endif
