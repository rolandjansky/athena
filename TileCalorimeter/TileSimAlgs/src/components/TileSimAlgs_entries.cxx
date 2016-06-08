#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TileSimAlgs/TileHitVecToCnt.h"
#include "TileSimAlgs/TileHitVecToCntTool.h"
#include "TileSimAlgs/TileHitToRawChannel.h"
#include "TileSimAlgs/TileHitToCell.h"
#include "TileSimAlgs/TileHitToTTL1.h"
#include "TileSimAlgs/TileTBHitToBeamElem.h"
#include "TileSimAlgs/TileDigitsMaker.h"
#include "TileSimAlgs/TileDigitsFromPulse.h"
#include "TileSimAlgs/TilePulseForTileMuonReceiver.h"
#include "TileSimAlgs/TileMuonReceiverDecision.h"

DECLARE_ALGORITHM_FACTORY( TileHitVecToCnt )
DECLARE_TOOL_FACTORY( TileHitVecToCntTool )
DECLARE_ALGORITHM_FACTORY( TileHitToRawChannel )
DECLARE_ALGORITHM_FACTORY( TileHitToCell )
DECLARE_ALGORITHM_FACTORY( TileHitToTTL1 )
DECLARE_ALGORITHM_FACTORY( TileTBHitToBeamElem )
DECLARE_ALGORITHM_FACTORY( TileDigitsMaker )
DECLARE_ALGORITHM_FACTORY( TileDigitsFromPulse )
DECLARE_ALGORITHM_FACTORY( TilePulseForTileMuonReceiver )
DECLARE_ALGORITHM_FACTORY( TileMuonReceiverDecision )

DECLARE_FACTORY_ENTRIES(TileSimAlgs) {
  DECLARE_ALGORITHM( TileHitVecToCnt )
  DECLARE_TOOL( TileHitVecToCntTool )
  DECLARE_ALGORITHM( TileHitToRawChannel )
  DECLARE_ALGORITHM( TileHitToCell )
  DECLARE_ALGORITHM( TileHitToTTL1 )
  DECLARE_ALGORITHM( TileTBHitToBeamElem )
  DECLARE_ALGORITHM( TileDigitsMaker )
  DECLARE_ALGORITHM( TileDigitsFromPulse )
  DECLARE_ALGORITHM( TilePulseForTileMuonReceiver )
  DECLARE_ALGORITHM( TileMuonReceiverDecision )
}
