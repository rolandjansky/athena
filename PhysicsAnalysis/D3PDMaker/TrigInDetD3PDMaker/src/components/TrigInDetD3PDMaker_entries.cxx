/*
  $Id: TrigInDetD3PDMaker_entries.cxx 340544 2011-01-18 12:51:04Z krasznaa $
  TrigInDetD3PDMaker_entries.cxx
*/

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Filler tool(s):
#include "../TrigInDetTrackFillerTool.h"
#include "../TrigVertexFillerTool.h"
#include "../TrigInDetTrackFitParFillerTool.h"

// Association tool(s):
#include "../TrigInDetTrackFitParAssociationTool.h"
#include "../TrigInDetTrackEndFitParAssociationTool.h"
#include "../TrigInDetTrackFitParAtVertexAssociationTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigInDetTrackFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigVertexFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigInDetTrackFitParFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigInDetTrackFitParAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigInDetTrackEndFitParAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigInDetTrackFitParAtVertexAssociationTool)

DECLARE_FACTORY_ENTRIES(TrigInDetD3PDMaker) {
  DECLARE_NAMESPACE_TOOL(D3PD, TrigInDetTrackFillerTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TrigVertexFillerTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TrigInDetTrackFitParFillerTool)

  DECLARE_NAMESPACE_TOOL(D3PD, TrigInDetTrackFitParAssociationTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TrigInDetTrackEndFitParAssociationTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TrigInDetTrackFitParAtVertexAssociationTool)
}


