#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkRIO_OnTrackCreator/RIO_OnTrackCreator.h"
#include "TrkRIO_OnTrackCreator/RIO_OnTrackErrorScalingTool.h"
// using namespace Trk;

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, RIO_OnTrackCreator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, RIO_OnTrackErrorScalingTool )

DECLARE_FACTORY_ENTRIES(TrkRIO_OnTrackCreator)
{ // in this package name the following methods are registered 
  DECLARE_NAMESPACE_TOOL(Trk, RIO_OnTrackCreator)
  DECLARE_NAMESPACE_TOOL(Trk, RIO_OnTrackErrorScalingTool)
}

