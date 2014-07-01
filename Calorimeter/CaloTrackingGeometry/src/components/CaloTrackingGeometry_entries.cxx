#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CaloTrackingGeometry/CaloTrackingGeometryBuilder.h"
#include "CaloTrackingGeometry/CaloSurfaceBuilder.h"
#include "CaloTrackingGeometry/CaloSurfaceHelper.h"
//
using namespace Calo;
//
DECLARE_TOOL_FACTORY( CaloTrackingGeometryBuilder )
DECLARE_TOOL_FACTORY( CaloSurfaceBuilder )
DECLARE_TOOL_FACTORY( CaloSurfaceHelper )
//
DECLARE_FACTORY_ENTRIES( CaloTrackingGeometry )
{
    DECLARE_TOOL( CaloTrackingGeometryBuilder );
    DECLARE_TOOL( CaloSurfaceBuilder );
    DECLARE_TOOL( CaloSurfaceHelper );
}


