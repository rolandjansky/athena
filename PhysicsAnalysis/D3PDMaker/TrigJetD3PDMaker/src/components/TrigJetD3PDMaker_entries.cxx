/*
TrigJetD3PDMaker_entries.cxx
*/
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TrigT2JetFillerTool.h"
#include "../TrigT2JetObjectDecisionFillerTool.h"
#include "../JetObjectDecisionFillerTool.h"
#include "../JetObjectSourceFillerTool.h"
#include "../TrigT2JetObjectSourceFillerTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigT2JetFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigT2JetObjectDecisionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, JetObjectDecisionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, JetObjectSourceFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigT2JetObjectSourceFillerTool )

DECLARE_FACTORY_ENTRIES(TrigJetD3PDMaker) {

   DECLARE_NAMESPACE_TOOL( D3PD, TrigT2JetFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigT2JetObjectDecisionFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, JetObjectDecisionFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, JetObjectSourceFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigT2JetObjectSourceFillerTool )

}
