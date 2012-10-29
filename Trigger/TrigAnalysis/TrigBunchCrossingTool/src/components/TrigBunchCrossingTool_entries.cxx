// $Id: TrigBunchCrossingTool_entries.cxx 502485 2012-05-25 13:02:53Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../TrigConfBunchCrossingTool.h"
#include "../StaticBunchCrossingTool.h"
#include "../MCBunchCrossingTool.h"
#include "../LHCBunchCrossingTool.h"
#include "../WebBunchCrossingTool.h"
#include "../D3PDBunchCrossingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigConfBunchCrossingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, StaticBunchCrossingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, MCBunchCrossingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, LHCBunchCrossingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, WebBunchCrossingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, D3PDBunchCrossingTool )

DECLARE_FACTORY_ENTRIES( TrigBunchCrossingTool ) {

   DECLARE_NAMESPACE_TOOL( Trig, TrigConfBunchCrossingTool )
   DECLARE_NAMESPACE_TOOL( Trig, StaticBunchCrossingTool )
   DECLARE_NAMESPACE_TOOL( Trig, MCBunchCrossingTool )
   DECLARE_NAMESPACE_TOOL( Trig, LHCBunchCrossingTool )
   DECLARE_NAMESPACE_TOOL( Trig, WebBunchCrossingTool )
   DECLARE_NAMESPACE_TOOL( Trig, D3PDBunchCrossingTool )

}
