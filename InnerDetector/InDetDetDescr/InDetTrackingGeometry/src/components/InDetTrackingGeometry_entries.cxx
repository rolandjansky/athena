#include "GaudiKernel/DeclareFactoryEntries.h"
// Atlas AlgTools
#include "InDetTrackingGeometry/BeamPipeBuilder.h"
#include "InDetTrackingGeometry/SiLayerBuilder.h"
#include "InDetTrackingGeometry/TRT_LayerBuilder.h"
#include "InDetTrackingGeometry/RobustTrackingGeometryBuilder.h"
#include "InDetTrackingGeometry/StagedTrackingGeometryBuilder.h"

using namespace InDet;

DECLARE_TOOL_FACTORY( BeamPipeBuilder )
DECLARE_TOOL_FACTORY( SiLayerBuilder )
DECLARE_TOOL_FACTORY( TRT_LayerBuilder )
DECLARE_TOOL_FACTORY( RobustTrackingGeometryBuilder )
DECLARE_TOOL_FACTORY( StagedTrackingGeometryBuilder )

DECLARE_FACTORY_ENTRIES( InDetTrackingGeometry )
{
    DECLARE_TOOL( BeamPipeBuilder )
    DECLARE_TOOL( SiLayerBuilder )
    DECLARE_TOOL( TRT_LayerBuilder )
    DECLARE_TOOL( RobustTrackingGeometryBuilder )  
    DECLARE_TOOL( StagedTrackingGeometryBuilder )       
}


