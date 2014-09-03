#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "InDetConversionFinderTools/TrackPairsSelector.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "InDetConversionFinderTools/ConversionPostSelector.h"
#include "InDetConversionFinderTools/SingleTrackConversionTool.h"
using namespace InDet;

DECLARE_TOOL_FACTORY( InDetConversionFinderTools )
DECLARE_TOOL_FACTORY( TrackPairsSelector )
DECLARE_TOOL_FACTORY( ConversionFinderUtils )
DECLARE_TOOL_FACTORY( VertexPointEstimator )
DECLARE_TOOL_FACTORY( ConversionPostSelector )
DECLARE_TOOL_FACTORY( SingleTrackConversionTool )

DECLARE_FACTORY_ENTRIES( InDetConversionFinderTools )
{
    DECLARE_TOOL( InDetConversionFinderTools )
    DECLARE_TOOL( TrackPairsSelector )
    DECLARE_TOOL( ConversionFinderUtils )
    DECLARE_TOOL( VertexPointEstimator )
    DECLARE_TOOL( ConversionPostSelector )
    DECLARE_TOOL( SingleTrackConversionTool )

}
