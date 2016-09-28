/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDECISION_TRIGDECISIONDICT_H
#define TRIGDECISION_TRIGDECISIONDICT_H

#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigDecisionTool/ChainGroupFunctions.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/ConfigurationAccess.h"
#include "TrigDecisionTool/DecisionAccess.h"
#include "TrigDecisionTool/ExpertMethods.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/TrigDecisionToolCore.h"
#include "TrigDecisionTool/TDTUtilities.h"


namespace TDTStruct {
   ToolHandle<Trig::TrigDecisionTool> tdt;

}

#endif
