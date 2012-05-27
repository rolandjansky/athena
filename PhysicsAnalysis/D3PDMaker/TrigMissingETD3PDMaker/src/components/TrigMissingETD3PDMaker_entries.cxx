// $Id: TrigMissingETD3PDMaker_entries.cxx 278740 2010-02-11 14:32:52Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../TrigMissingETBaseFillerTool.h"
#include "../TrigMissingETComponentFillerTool.h"

#include "../TrigMissingETGetterTool.h"

//
// Declare the components of the package:
//
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMissingETBaseFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMissingETComponentFillerTool )

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMissingETGetterTool )

DECLARE_FACTORY_ENTRIES( TrigMissingETD3PDMaker ) {

   DECLARE_NAMESPACE_TOOL( D3PD, TrigMissingETBaseFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMissingETComponentFillerTool )

   DECLARE_NAMESPACE_TOOL( D3PD, TrigMissingETGetterTool )

}
