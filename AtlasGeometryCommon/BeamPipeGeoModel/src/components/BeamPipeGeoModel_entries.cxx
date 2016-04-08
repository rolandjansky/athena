#include "BeamPipeGeoModel/BeamPipeDetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(BeamPipeDetectorTool)

DECLARE_FACTORY_ENTRIES(BeamPipeGeoModel) {
    DECLARE_ALGTOOL  ( BeamPipeDetectorTool )
}
