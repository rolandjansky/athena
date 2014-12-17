
// Tools and algos
#include "TrigCostMonitor/TrigCostAlg.h"
#include "TrigCostMonitor/TrigCostRun.h"
#include "TrigCostMonitor/TrigCostTool.h"

// Subtools
#include "TrigCostMonitor/TrigNtConfTool.h"
#include "TrigCostMonitor/TrigNtElemTool.h"
#include "TrigCostMonitor/TrigNtExecTool.h"
#include "TrigCostMonitor/TrigNtLvl1Tool.h"
#include "TrigCostMonitor/TrigNtHltRTool.h"
#include "TrigCostMonitor/TrigNtEBWeightTool.h"
#include "TrigCostMonitor/TrigNtPostTool.h"
#include "TrigCostMonitor/TrigNtRobsTool.h"
#include "TrigCostMonitor/TrigNtSaveTool.h"
#include "TrigCostMonitor/TrigNtVarsTool.h"

// Framework
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigCostAlg )
DECLARE_ALGORITHM_FACTORY( TrigCostRun )
DECLARE_TOOL_FACTORY( TrigCostTool )

DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtConfTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtElemTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtExecTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtLvl1Tool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtHltRTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtEBWeightTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtPostTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtRobsTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtSaveTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtVarsTool )

DECLARE_FACTORY_ENTRIES( TrigCostMonitor )
{
  DECLARE_ALGORITHM( TrigCostAlg )
  DECLARE_ALGORITHM( TrigCostRun )
  DECLARE_TOOL( TrigCostTool )

  DECLARE_NAMESPACE_TOOL( Trig, TrigNtConfTool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtElemTool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtExecTool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtLvl1Tool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtHltRTool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtEBWeightTool )  
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtPostTool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtRobsTool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtSaveTool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtVarsTool )
}
