#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MdtSegmentT0Fitter/MdtSegmentT0Fitter.h"

using namespace TrkDriftCircleMath;

DECLARE_TOOL_FACTORY(MdtSegmentT0Fitter)

DECLARE_FACTORY_ENTRIES(MdtSegmentT0Fitter) {
  DECLARE_TOOL(MdtSegmentT0Fitter)
}

