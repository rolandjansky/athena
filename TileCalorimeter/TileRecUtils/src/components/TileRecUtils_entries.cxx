#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TileRecUtils/TileRawChannelBuilderFlatFilter.h"
#include "TileRecUtils/TileRawChannelBuilderFitFilter.h"
#include "TileRecUtils/TileRawChannelBuilderFitFilterCool.h"
#include "TileRecUtils/TileRawChannelBuilderOptFilter.h"
#include "TileRecUtils/TileRawChannelBuilderOpt2Filter.h"
#include "TileRecUtils/TileRawChannelBuilderQIEFilter.h"
#include "TileRecUtils/TileRawChannelBuilderManyAmps.h"
#include "TileRecUtils/TileRawChannelBuilderMF.h"
#include "TileRecUtils/TileRawChannelBuilderWienerFilter.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileRecUtils/TileCellBuilder.h"
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
DECLARE_TOOL_FACTORY( TileRawChannelBuilderWienerFilter )
DECLARE_TOOL_FACTORY( TileBeamInfoProvider )
DECLARE_TOOL_FACTORY( TileCellBuilder )
DECLARE_TOOL_FACTORY( TileCellFakeProb )
DECLARE_TOOL_FACTORY( TileCellMaskingTool )
DECLARE_TOOL_FACTORY( TileTowerBuilderTool )
DECLARE_TOOL_FACTORY( TileCellNoiseFilter )
DECLARE_TOOL_FACTORY( TileRawChannelNoiseFilter )
DECLARE_TOOL_FACTORY( TileRawChannelOF1Corrector )
DECLARE_ALGORITHM_FACTORY( TileRawChannelMaker )
DECLARE_ALGORITHM_FACTORY( TileRawChannelVerify )
DECLARE_ALGORITHM_FACTORY( TileRawCorrelatedNoise )

DECLARE_FACTORY_ENTRIES(TileRecUtils) {
  DECLARE_TOOL( TileRawChannelBuilderFlatFilter )
  DECLARE_TOOL( TileRawChannelBuilderFitFilter )
  DECLARE_TOOL( TileRawChannelBuilderFitFilterCool )
  DECLARE_TOOL( TileRawChannelBuilderOptFilter )
  DECLARE_TOOL( TileRawChannelBuilderOpt2Filter )
  DECLARE_TOOL( TileRawChannelBuilderQIEFilter )
  DECLARE_TOOL( TileRawChannelBuilderManyAmps )
  DECLARE_TOOL( TileRawChannelBuilderMF )
  DECLARE_TOOL( TileRawChannelBuilderWienerFilter )
  DECLARE_TOOL( TileBeamInfoProvider )
  DECLARE_TOOL( TileCellBuilder )
  DECLARE_TOOL( TileCellFakeProb )
  DECLARE_TOOL( TileCellMaskingTool )
  DECLARE_TOOL( TileTowerBuilderTool )
  DECLARE_TOOL( TileCellNoiseFilter )
  DECLARE_TOOL( TileRawChannelNoiseFilter )
  DECLARE_ALGORITHM( TileRawChannelMaker )
  DECLARE_ALGORITHM( TileRawChannelVerify )
  DECLARE_ALGORITHM( TileRawCorrelatedNoise )
}
