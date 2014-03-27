#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator.h"
#include "../AdjustableT0Tool.h"

using namespace Muon;
using namespace AdjT0;

DECLARE_TOOL_FACTORY(MdtDriftCircleOnTrackCreator)
DECLARE_TOOL_FACTORY(AdjustableT0Tool)

DECLARE_FACTORY_ENTRIES(MdtDriftCircleOnTrackCreator) {
  DECLARE_TOOL(MdtDriftCircleOnTrackCreator)
  DECLARE_TOOL(AdjustableT0Tool)
}

