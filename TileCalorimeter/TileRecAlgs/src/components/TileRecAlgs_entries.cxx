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

DECLARE_COMPONENT( TileCellCorrection )
DECLARE_COMPONENT( TileCellSelector )
DECLARE_COMPONENT( TileCellIDCToCell )
DECLARE_COMPONENT( TileCellVerify )
DECLARE_COMPONENT( TileBeamElemToCell )
DECLARE_COMPONENT( TileRawChannelToTTL1 )
DECLARE_COMPONENT( TileDigitsToTTL1 )
DECLARE_COMPONENT( TileRawChannelToHit )
DECLARE_COMPONENT( TileDigitsFilter )
DECLARE_COMPONENT( TileDigitsThresholdFilter )
DECLARE_COMPONENT( TileCellToTTL1 )
DECLARE_COMPONENT( MBTSTimeDiffEventInfoAlg )
DECLARE_COMPONENT( TileMuonReceiverReadCnt )

