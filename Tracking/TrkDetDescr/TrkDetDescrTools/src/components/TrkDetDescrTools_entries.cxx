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

DECLARE_COMPONENT( CylinderVolumeCreator )
DECLARE_COMPONENT( BinnedLayerMaterialCreator )
DECLARE_COMPONENT( CompoundLayerMaterialCreator )
DECLARE_COMPONENT( CompressedLayerMaterialCreator )
DECLARE_COMPONENT( GeometryBuilder )
DECLARE_COMPONENT( GenericGeometryBuilder )
DECLARE_COMPONENT( LayerArrayCreator )
DECLARE_COMPONENT( LayerProvider )
DECLARE_COMPONENT( LayerMaterialProvider )
DECLARE_COMPONENT( LayerMaterialConverter )
DECLARE_COMPONENT( InputLayerMaterialProvider )
DECLARE_COMPONENT( TrackingVolumeArrayCreator )
DECLARE_COMPONENT( TrackingVolumeHelper )

