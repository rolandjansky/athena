#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TrackFastSimSDTool.h"
#include "../NeutronFastSimTool.h"
#include "../StoppedParticleFastSimTool.h"

DECLARE_TOOL_FACTORY( TrackFastSimSDTool )
DECLARE_TOOL_FACTORY( NeutronFastSimTool )
DECLARE_TOOL_FACTORY( StoppedParticleFastSimTool )

DECLARE_FACTORY_ENTRIES( TrackWriteFastSim ){
  DECLARE_TOOL( TrackFastSimSDTool )
  DECLARE_TOOL( NeutronFastSimTool )
  DECLARE_TOOL( StoppedParticleFastSimTool )
}
