#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkDetDescrUnitTests/TrkDetDescrTPCnvTest.h"
#include "TrkDetDescrUnitTests/BinUtilityTest.h"
#include "TrkDetDescrUnitTests/TrackingGeometryTest.h"
#include "TrkDetDescrUnitTests/SurfaceIntersectionTest.h"
#include "TrkDetDescrUnitTests/MappingTest.h"

using namespace Trk;

DECLARE_ALGORITHM_FACTORY( BinUtilityTest )
DECLARE_ALGORITHM_FACTORY( TrkDetDescrTPCnvTest )
DECLARE_ALGORITHM_FACTORY( TrackingGeometryTest )
DECLARE_ALGORITHM_FACTORY( SurfaceIntersectionTest )
DECLARE_ALGORITHM_FACTORY( MappingTest )


/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkDetDescrUnitTests )
{
    DECLARE_ALGORITHM( BinUtilityTest )
    DECLARE_ALGORITHM( TrkDetDescrTPCnvTest )
    DECLARE_ALGORITHM( TrackingGeometryTest )
    DECLARE_ALGORITHM( SurfaceIntersectionTest )
    DECLARE_ALGORITHM( MappingTest )
    
}
