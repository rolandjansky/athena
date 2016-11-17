// $Id: TrigBunchCrossingTool_entries.cxx 762070 2016-07-15 08:39:37Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "TrigBunchCrossingTool/StaticBunchCrossingTool.h"
#include "TrigBunchCrossingTool/WebBunchCrossingTool.h"
#include "TrigBunchCrossingTool/D3PDBunchCrossingTool.h"
#include "TrigBunchCrossingTool/xAODBunchCrossingTool.h"
#include "TrigBunchCrossingTool/TrigConfBunchCrossingTool.h"

#include "../MCBunchCrossingTool.h"
#include "../LHCBunchCrossingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trig, StaticBunchCrossingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, WebBunchCrossingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, D3PDBunchCrossingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, xAODBunchCrossingTool )

DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigConfBunchCrossingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, MCBunchCrossingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, LHCBunchCrossingTool )

DECLARE_FACTORY_ENTRIES( TrigBunchCrossingTool ) {

   DECLARE_NAMESPACE_TOOL( Trig, StaticBunchCrossingTool )
   DECLARE_NAMESPACE_TOOL( Trig, WebBunchCrossingTool )
   DECLARE_NAMESPACE_TOOL( Trig, D3PDBunchCrossingTool )
   DECLARE_NAMESPACE_TOOL( Trig, xAODBunchCrossingTool )

   DECLARE_NAMESPACE_TOOL( Trig, TrigConfBunchCrossingTool )
   DECLARE_NAMESPACE_TOOL( Trig, MCBunchCrossingTool )
   DECLARE_NAMESPACE_TOOL( Trig, LHCBunchCrossingTool )

}
