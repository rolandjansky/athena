#include "TileRecUtils/TileRawChannelBuilderFlatFilter.h"
#include "TileRecUtils/TileRawChannelBuilderFitFilter.h"
#include "TileRecUtils/TileRawChannelBuilderFitFilterCool.h"
#include "TileRecUtils/TileRawChannelBuilderOptFilter.h"
#include "TileRecUtils/TileRawChannelBuilderOpt2Filter.h"
#include "TileRecUtils/TileRawChannelBuilderQIEFilter.h"
#include "TileRecUtils/TileRawChannelBuilderManyAmps.h"
#include "TileRecUtils/TileRawChannelBuilderMF.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileRecUtils/TileCellBuilder.h"
#include "TileRecUtils/TileCellBuilderFromHit.h"
#include "TileRecUtils/TileCellFakeProb.h"
#include "TileRecUtils/TileCellMaskingTool.h"
#include "TileRecUtils/TileRawChannelMaker.h"
#include "TileRecUtils/TileRawChannelVerify.h"
#include "TileRecUtils/TileRawCorrelatedNoise.h"
#include "TileRecUtils/TileTowerBuilderTool.h"
#include "TileRecUtils/TileCellNoiseFilter.h"
#include "TileRecUtils/TileRawChannelNoiseFilter.h"
#include "TileRecUtils/TileRawChannelOF1Corrector.h"

DECLARE_TOOL_FACTORY( TileRawChannelBuilderFlatFilter )
DECLARE_TOOL_FACTORY( TileRawChannelBuilderFitFilter )
DECLARE_TOOL_FACTORY( TileRawChannelBuilderFitFilterCool )
DECLARE_TOOL_FACTORY( TileRawChannelBuilderOptFilter )
DECLARE_TOOL_FACTORY( TileRawChannelBuilderOpt2Filter )
DECLARE_TOOL_FACTORY( TileRawChannelBuilderQIEFilter )
DECLARE_TOOL_FACTORY( TileRawChannelBuilderManyAmps )
DECLARE_TOOL_FACTORY( TileRawChannelBuilderMF )
DECLARE_TOOL_FACTORY( TileBeamInfoProvider )
DECLARE_TOOL_FACTORY( TileCellBuilder )
DECLARE_TOOL_FACTORY( TileCellBuilderFromHit )
DECLARE_TOOL_FACTORY( TileCellFakeProb )
DECLARE_TOOL_FACTORY( TileCellMaskingTool )
DECLARE_TOOL_FACTORY( TileTowerBuilderTool )
DECLARE_TOOL_FACTORY( TileCellNoiseFilter )
DECLARE_TOOL_FACTORY( TileRawChannelNoiseFilter )
DECLARE_TOOL_FACTORY( TileRawChannelOF1Corrector )
DECLARE_ALGORITHM_FACTORY( TileRawChannelMaker )
DECLARE_ALGORITHM_FACTORY( TileRawChannelVerify )
DECLARE_ALGORITHM_FACTORY( TileRawCorrelatedNoise )

