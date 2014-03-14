/*
  TrigCaloD3PDMaker_entries.cxx
*/
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigCaloClusterFillerTool.h"
#include "../TrigEMClusterFillerTool.h"
#include "../TrigEMRingerFillerTool.h"
#include "../TriggerTowerFillerTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigCaloClusterFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigEMClusterFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigEMRingerFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TriggerTowerFillerTool)

DECLARE_FACTORY_ENTRIES(TrigCaloD3PDMkaer) {
  DECLARE_NAMESPACE_TOOL(D3PD, TrigCaloClusterFillerTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TrigEMClusterFillerTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TrigEMRingerFillerTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TriggerTowerFillerTool)
}


