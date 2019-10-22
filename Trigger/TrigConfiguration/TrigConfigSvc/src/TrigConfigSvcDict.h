/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTTriggerType.h"
#include "TrigConfHLTData/HLTStreamTag.h"

#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/L1DataBaseclass.h"
#include "TrigConfL1Data/TrigConfData.h"

#include "TrigConfigSvc/TrigConfigARA.h"
#include "TrigConfigSvc/TrigConfigSvcARA.h"

#include <vector>
namespace {
   std::vector<TrigConf::HLTChain*> vc;
   std::vector<TrigConf::HLTSequence*> vs;
}
