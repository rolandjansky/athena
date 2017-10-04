#include "TriggerMenuAnalysis/TrigAccessTool.h"
#include "TriggerMenuAnalysis/RoILinksCnvTool.h"
#include "TriggerMenuAnalysis/ReadTriggerInfo.h"
#include "TriggerMenuAnalysis/TrigMenuHists.h"
#include "TriggerMenuAnalysis/TestNewTDT.h"
#include "TriggerMenuAnalysis/JustInitTDTAlgo.hxx"
#include "TriggerMenuAnalysis/TrigMenuNtupleAlg.h"
#include "TriggerMenuAnalysis/DumpAPTInput.h"
#include "TriggerMenuAnalysis/HltNaviChecker.h"

#include "TriggerMenuAnalysis/FlatHltNaviMaker.h"

DECLARE_ALGORITHM_FACTORY(JustInitTDTAlgo)
DECLARE_ALGORITHM_FACTORY(TrigMenuNtupleAlg)
DECLARE_ALGORITHM_FACTORY(DumpAPTInput)
DECLARE_ALGORITHM_FACTORY(FlatHltNaviMaker)
DECLARE_ALGORITHM_FACTORY(HltNaviChecker)

DECLARE_TOOL_FACTORY(TrigAccessTool)
DECLARE_TOOL_FACTORY(TrigMenuHists)
DECLARE_TOOL_FACTORY(RoILinksCnvTool)
DECLARE_ALGORITHM_FACTORY(ReadTriggerInfo)
DECLARE_ALGORITHM_FACTORY(TestNewTDT)

