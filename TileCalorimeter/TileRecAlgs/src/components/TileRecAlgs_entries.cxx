#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileRecUtils/TileCellMaskingTool.h"

#include "TileRecAlgs/TileCellCorrection.h"
#include "TileRecAlgs/TileCellSelector.h"
#include "TileRecAlgs/TileCellIDCToCell.h"
#include "TileRecAlgs/TileCellVerify.h"
#include "TileRecAlgs/TileBeamElemToCell.h"
#include "TileRecAlgs/TileRawChannelToTTL1.h"
#include "TileRecAlgs/TileDigitsToTTL1.h"
#include "TileRecAlgs/TileCellToTTL1.h"
#include "TileRecAlgs/TileRawChannelToHit.h"
#include "TileRecAlgs/TileDigitsFilter.h"
#include "TileRecAlgs/TileDigitsThresholdFilter.h"
#include "TileRecAlgs/MBTSTimeDiffEventInfoAlg.h"
#include "TileRecAlgs/TileMuonReceiverReadCnt.h"

DECLARE_SERVICE_FACTORY( TileCellCorrection )
DECLARE_ALGORITHM_FACTORY( TileCellSelector )
DECLARE_ALGORITHM_FACTORY( TileCellIDCToCell )
DECLARE_ALGORITHM_FACTORY( TileCellVerify )
DECLARE_ALGORITHM_FACTORY( TileBeamElemToCell )
DECLARE_ALGORITHM_FACTORY( TileRawChannelToTTL1 )
DECLARE_ALGORITHM_FACTORY( TileDigitsToTTL1 )
DECLARE_ALGORITHM_FACTORY( TileRawChannelToHit )
DECLARE_ALGORITHM_FACTORY( TileDigitsFilter )
DECLARE_ALGORITHM_FACTORY( TileDigitsThresholdFilter )
DECLARE_ALGORITHM_FACTORY( TileCellToTTL1 )
DECLARE_ALGORITHM_FACTORY( MBTSTimeDiffEventInfoAlg )
DECLARE_ALGORITHM_FACTORY( TileMuonReceiverReadCnt )

