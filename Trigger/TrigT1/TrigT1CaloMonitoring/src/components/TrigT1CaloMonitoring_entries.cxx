#include "TrigT1CaloMonitoring/OverviewMon.h"
#include "TrigT1CaloMonitoring/CPMon.h"
#include "TrigT1CaloMonitoring/CPSimMon.h"
#include "TrigT1CaloMonitoring/JEPJEMMon.h"
#include "TrigT1CaloMonitoring/JEPCMXMon.h"
#include "TrigT1CaloMonitoring/JEPSimMon.h"
#include "TrigT1CaloMonitoring/PPrMon.h"
#include "TrigT1CaloMonitoring/PPrStabilityMon.h"
#include "TrigT1CaloMonitoring/PPrSpareMon.h"
#include "TrigT1CaloMonitoring/PPMSimBSMon.h"

//Run 1
#include "TrigT1CaloMonitoring/CMMMon.h"
#include "TrigT1CaloMonitoring/CPMSimBSMon.h"
#include "TrigT1CaloMonitoring/JEMMon.h"
#include "TrigT1CaloMonitoring/JEPSimBSMon.h"
#include "TrigT1CaloMonitoring/TrigT1CaloBSMon.h"
#include "TrigT1CaloMonitoring/TrigT1CaloCpmMonTool.h"
#include "TrigT1CaloMonitoring/TrigT1CaloRodMonTool.h"
#include "TrigT1CaloMonitoring/TrigT1CaloGlobalMonTool.h"
#include "TrigT1CaloMonitoring/EmEfficienciesMonTool.h"
#include "TrigT1CaloMonitoring/JetEfficienciesMonTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY(LVL1, OverviewMon)
DECLARE_NAMESPACE_TOOL_FACTORY(LVL1, CPMon)
DECLARE_NAMESPACE_TOOL_FACTORY(LVL1, CPSimMon)
DECLARE_NAMESPACE_TOOL_FACTORY(LVL1, JEPJEMMon)
DECLARE_NAMESPACE_TOOL_FACTORY(LVL1, JEPCMXMon)
DECLARE_NAMESPACE_TOOL_FACTORY(LVL1, JEPSimMon)
DECLARE_TOOL_FACTORY(PPrMon)
DECLARE_TOOL_FACTORY(PPrStabilityMon)
DECLARE_TOOL_FACTORY(PPrSpareMon)
DECLARE_TOOL_FACTORY(PPMSimBSMon)

//Run 1
DECLARE_TOOL_FACTORY(CMMMon)
DECLARE_TOOL_FACTORY(CPMSimBSMon)
DECLARE_TOOL_FACTORY(JEMMon)
DECLARE_TOOL_FACTORY(JEPSimBSMon)
DECLARE_TOOL_FACTORY(TrigT1CaloBSMon)
DECLARE_TOOL_FACTORY(TrigT1CaloCpmMonTool)
DECLARE_TOOL_FACTORY(TrigT1CaloRodMonTool)
DECLARE_TOOL_FACTORY(TrigT1CaloGlobalMonTool)
DECLARE_TOOL_FACTORY(EmEfficienciesMonTool)
DECLARE_TOOL_FACTORY(JetEfficienciesMonTool)


DECLARE_FACTORY_ENTRIES(TrigT1CaloMonitoring) {
  DECLARE_NAMESPACE_ALGTOOL(LVL1, OverviewMon)
  DECLARE_NAMESPACE_ALGTOOL(LVL1, CPMon)
  DECLARE_NAMESPACE_ALGTOOL(LVL1, CPSimMon)
  DECLARE_NAMESPACE_ALGTOOL(LVL1, JEPJEMMon)
  DECLARE_NAMESPACE_ALGTOOL(LVL1, JEPCMXMon)
  DECLARE_NAMESPACE_ALGTOOL(LVL1, JEPSimMon)
  DECLARE_ALGTOOL(PPrMon)
  DECLARE_ALGTOOL(PPrStabilityMon)  
  DECLARE_ALGTOOL(PPrSpareMon)
  DECLARE_ALGTOOL(PPMSimBSMon)
  
  //Run 1
  DECLARE_ALGTOOL(CMMMon)
  DECLARE_ALGTOOL(CPMSimBSMon)
  DECLARE_ALGTOOL(JEMMon)
  DECLARE_ALGTOOL(JEPSimBSMon)
  DECLARE_ALGTOOL(TrigT1CaloBSMon)
  DECLARE_ALGTOOL(TrigT1CaloCpmMonTool)
  DECLARE_ALGTOOL(TrigT1CaloRodMonTool)
  DECLARE_ALGTOOL(TrigT1CaloGlobalMonTool)
  DECLARE_ALGTOOL(EmEfficienciesMonTool)
  DECLARE_ALGTOOL(JetEfficienciesMonTool)
}

