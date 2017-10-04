#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaL1SelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaL2CaloSelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaL2CaloRingerSelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaL2ElectronSelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaL2SelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEFCaloSelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEFElectronSelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEFPhotonSelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaIsolationSelectorTool.h"
#include "../TrigEgammaEmulationToolTest.h"
#include "../TrigEgammaEmulationToolAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaEmulationTool)   
DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaSelectorBaseTool)   
DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaL1SelectorTool)   
DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaL2SelectorTool) 
DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaL2ElectronSelectorTool) 
DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaL2CaloSelectorTool) 
DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaL2CaloRingerSelectorTool) 
DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaEFCaloSelectorTool)   
DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaEFElectronSelectorTool)   
DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaEFPhotonSelectorTool)   
DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaIsolationSelectorTool)   
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trig,TrigEgammaEmulationToolTest) 
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trig,TrigEgammaEmulationToolAlg) 

DECLARE_FACTORY_ENTRIES(TrigEgammaEmulationTool) {
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaEmulationTool)
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaSelectorBaseTool)
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaL1SelectorTool)
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaL2SelectorTool)
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaL2ElectronSelectorTool)
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaL2CaloSelectorTool)
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaL2CaloRingerSelectorTool)
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaEFCaloSelectorTool)
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaEFElectronSelectorTool)
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaEFPhotonSelectorTool)
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaIsolationSelectorTool)
  DECLARE_NAMESPACE_ALGORITHM(Trig,TrigEgammaMatchingToolTest)
}

