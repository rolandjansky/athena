/*
  TrigEgammaD3PDMaker_entries.cxx
*/
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigElectronFillerTool.h"
#include "../TrigPhotonFillerTool.h"
#include "../TrigElectronObjectDecisionFillerTool.h"
#include "../TrigPhotonObjectDecisionFillerTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigElectronFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigPhotonFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigElectronObjectDecisionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigPhotonObjectDecisionFillerTool)

DECLARE_FACTORY_ENTRIES(TrigEgammaD3PDMkaer) {
  DECLARE_NAMESPACE_TOOL(D3PD, TrigElectronFillerTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TrigPhotonFillerTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TrigElectronObjectDecisionFillerTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TrigPhotonObjectDecisionFillerTool)
}


