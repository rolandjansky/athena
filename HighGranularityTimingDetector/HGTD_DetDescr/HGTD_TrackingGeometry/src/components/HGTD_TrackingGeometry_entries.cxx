#include "GaudiKernel/DeclareFactoryEntries.h"
// Atlas AlgTools
#include "HGTD_TrackingGeometry/HGTD_LayerBuilder.h"
#include "HGTD_TrackingGeometry/HGTD_TrackingGeometryBuilder.h"

using namespace HGTDet;

DECLARE_TOOL_FACTORY( HGTD_LayerBuilder )
DECLARE_TOOL_FACTORY( HGTD_TrackingGeometryBuilder )

DECLARE_FACTORY_ENTRIES( HGTD_TrackingGeometry )
{
    DECLARE_TOOL( HGTD_LayerBuilder )
    DECLARE_TOOL( HGTD_TrackingGeometryBuilder )
}
