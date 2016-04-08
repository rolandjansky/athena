#include "GeoModelExamples/ToyDetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(ToyDetectorTool)

DECLARE_FACTORY_ENTRIES(GeoModelExamples) {
    DECLARE_ALGTOOL  ( ToyDetectorTool )
}
