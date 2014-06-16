#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_TrackSegmentsTool_xk/TRT_TrackSegmentsMaker_ATLxk.h"
#include "TRT_TrackSegmentsTool_xk/TRT_TrackSegmentsMaker_ECcosmics.h"
#include "TRT_TrackSegmentsTool_xk/TRT_TrackSegmentsMaker_BarrelCosmics.h"

using namespace InDet;

DECLARE_TOOL_FACTORY( TRT_TrackSegmentsMaker_ATLxk )
DECLARE_TOOL_FACTORY( TRT_TrackSegmentsMaker_ECcosmics )
DECLARE_TOOL_FACTORY( TRT_TrackSegmentsMaker_BarrelCosmics )

DECLARE_FACTORY_ENTRIES( TRT_TrackSegmentsTool_xk )
{
	DECLARE_TOOL( TRT_TrackSegmentsMaker_ATLxk )
	DECLARE_TOOL( TRT_TrackSegmentsMaker_ECcosmics )
        DECLARE_TOOL( TRT_TrackSegmentsMaker_BarrelCosmics )
}





