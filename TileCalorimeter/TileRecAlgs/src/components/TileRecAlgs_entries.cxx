#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileRecUtils/TileCellMaskingTool.h"

#include "../TileCellSelector.h"
#include "../TileCellIDCToCell.h"
#include "../TileCellVerify.h"
#include "../TileBeamElemToCell.h"
#include "../TileRawChannelToTTL1.h"
#include "../TileDigitsToTTL1.h"
#include "../TileCellToTTL1.h"
#include "../TileRawChannelToHit.h"
#include "../TileDigitsFilter.h"
#include "../TileDigitsThresholdFilter.h"
#include "../MBTSTimeDiffEventInfoAlg.h"
#include "../TileMuonReceiverReadCnt.h"

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

