#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkDetDescrTools/CylinderVolumeCreator.h"
#include "TrkDetDescrTools/BinnedLayerMaterialCreator.h"
#include "TrkDetDescrTools/CompoundLayerMaterialCreator.h"
#include "TrkDetDescrTools/CompressedLayerMaterialCreator.h"
#include "TrkDetDescrTools/GeometryBuilder.h"
#include "TrkDetDescrTools/GenericGeometryBuilder.h"
#include "TrkDetDescrTools/LayerArrayCreator.h"
#include "TrkDetDescrTools/LayerProvider.h"
#include "TrkDetDescrTools/LayerMaterialProvider.h"
#include "TrkDetDescrTools/LayerMaterialConverter.h"
#include "TrkDetDescrTools/InputLayerMaterialProvider.h"
#include "TrkDetDescrTools/TrackingVolumeArrayCreator.h"
#include "TrkDetDescrTools/TrackingVolumeHelper.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( CylinderVolumeCreator )
DECLARE_TOOL_FACTORY( BinnedLayerMaterialCreator )
DECLARE_TOOL_FACTORY( CompoundLayerMaterialCreator )
DECLARE_TOOL_FACTORY( CompressedLayerMaterialCreator )
DECLARE_TOOL_FACTORY( GeometryBuilder )
DECLARE_TOOL_FACTORY( GenericGeometryBuilder )
DECLARE_TOOL_FACTORY( LayerArrayCreator )
DECLARE_TOOL_FACTORY( LayerProvider )
DECLARE_TOOL_FACTORY( LayerMaterialProvider )
DECLARE_TOOL_FACTORY( LayerMaterialConverter )
DECLARE_TOOL_FACTORY( InputLayerMaterialProvider )
DECLARE_TOOL_FACTORY( TrackingVolumeArrayCreator )
DECLARE_TOOL_FACTORY( TrackingVolumeHelper )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkDetDescrTools )
{
    DECLARE_TOOL( CylinderVolumeCreator )
    DECLARE_TOOL( BinnedLayerMaterialCreator )
    DECLARE_TOOL( CompoundLayerMaterialCreator )
    DECLARE_TOOL( CompressedLayerMaterialCreator )
    DECLARE_TOOL( GeometryBuilder )
    DECLARE_TOOL( GenericGeometryBuilder )
    DECLARE_TOOL( LayerArrayCreator )
    DECLARE_TOOL( LayerProvider )
    DECLARE_TOOL( LayerMaterialProvider )
    DECLARE_TOOL( LayerMaterialConverter )
    DECLARE_TOOL( InputLayerMaterialProvider )    
    DECLARE_TOOL( TrackingVolumeArrayCreator )
    DECLARE_TOOL( TrackingVolumeHelper )
}
