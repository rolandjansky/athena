#include "TileRecUtils/TileRawChannelBuilderFlatFilter.h"
#include "TileRecUtils/TileRawChannelBuilderFitFilter.h"
#include "TileRecUtils/TileRawChannelBuilderFitFilterCool.h"
#include "TileRecUtils/TileRawChannelBuilderOptFilter.h"
#include "TileRecUtils/TileRawChannelBuilderOpt2Filter.h"
#include "TileRecUtils/TileRawChannelBuilderQIEFilter.h"
#include "TileRecUtils/TileRawChannelBuilderManyAmps.h"
#include "TileRecUtils/TileRawChannelBuilderMF.h"
#include "TileRecUtils/TileCellBuilder.h"
#include "TileRecUtils/TileCellBuilderFromHit.h"
#include "TileRecUtils/TileCellFakeProb.h"
#include "TileRecUtils/TileCellMaskingTool.h"
#include "TileRecUtils/TileRawChannelMaker.h"
#include "TileRecUtils/TileRawChannelVerify.h"
#include "TileRecUtils/TileRawCorrelatedNoise.h"
#include "TileRecUtils/TileCellNoiseFilter.h"
#include "TileRecUtils/TileRawChannelNoiseFilter.h"
#include "TileRecUtils/TileRawChannelOF1Corrector.h"
#include "../TileTowerBuilderTool.h"
#include "../TileDigitsDumper.h"
#include "../TileRawChannelDumper.h"
#include "../TileBeamElemDumper.h"
#include "../TileL2Dumper.h"
#include "../TileLaserObjectDumper.h"
#include "../TileMuonReceiverDumper.h"
#include "../TileRawChannelBuilderTest.h"
#include "../TileDQstatusTool.h"
#include "../TileDQstatusAlg.h"

DECLARE_COMPONENT( TileRawChannelBuilderFlatFilter )
DECLARE_COMPONENT( TileRawChannelBuilderFitFilter )
DECLARE_COMPONENT( TileRawChannelBuilderFitFilterCool )
DECLARE_COMPONENT( TileRawChannelBuilderOptFilter )
DECLARE_COMPONENT( TileRawChannelBuilderOpt2Filter )
DECLARE_COMPONENT( TileRawChannelBuilderQIEFilter )
DECLARE_COMPONENT( TileRawChannelBuilderManyAmps )
DECLARE_COMPONENT( TileRawChannelBuilderMF )
DECLARE_COMPONENT( TileCellBuilder )
DECLARE_COMPONENT( TileCellBuilderFromHit )
DECLARE_COMPONENT( TileCellFakeProb )
DECLARE_COMPONENT( TileCellMaskingTool )
DECLARE_COMPONENT( TileTowerBuilderTool )
DECLARE_COMPONENT( TileCellNoiseFilter )
DECLARE_COMPONENT( TileRawChannelNoiseFilter )
DECLARE_COMPONENT( TileRawChannelOF1Corrector )
DECLARE_COMPONENT( TileRawChannelMaker )
DECLARE_COMPONENT( TileRawChannelVerify )
DECLARE_COMPONENT( TileRawCorrelatedNoise )
DECLARE_COMPONENT( TileDigitsDumper )
DECLARE_COMPONENT( TileRawChannelDumper )
DECLARE_COMPONENT( TileBeamElemDumper )
DECLARE_COMPONENT( TileL2Dumper )
DECLARE_COMPONENT( TileLaserObjectDumper )
DECLARE_COMPONENT( TileMuonReceiverDumper )
DECLARE_COMPONENT( TileRawChannelBuilderTest )
DECLARE_COMPONENT( TileDQstatusTool )
DECLARE_COMPONENT( TileDQstatusAlg )
