#include "../LArNonLinearity.h"
#include "../LArCellBuilderFromLArRawChannelTool.h"
#include "../LArCellEmMiscalib.h"
#include "../LArCellRescaler.h"
#include "../LArCellMaskingTool.h"
#include "../LArCellNoiseMaskingTool.h"
#include "../LArBadFebMaskingTool.h"
#include "../LArCellGainPathology.h"
#include "../LArCellMerger.h"
#include "../LArCellDeadOTXCorr.h"
#include "../LArNoisyROAlg.h"
#include "../LArNoisyROTool.h"
#include "../LArCollisionTimeAlg.h"
#include "../LArTimeVetoAlg.h"
#include "../LArCellContHVCorrTool.h"
#include "../LArHVFraction.h"
#include "../LArRAWtoSuperCell.h"

DECLARE_COMPONENT( LArNoisyROAlg )
DECLARE_COMPONENT( LArNonLinearity )
DECLARE_COMPONENT( LArCellBuilderFromLArRawChannelTool )
DECLARE_COMPONENT( LArCellEmMiscalib )
DECLARE_COMPONENT( LArCellRescaler )
DECLARE_COMPONENT( LArCellMaskingTool )
DECLARE_COMPONENT( LArCellNoiseMaskingTool )
DECLARE_COMPONENT( LArBadFebMaskingTool )
DECLARE_COMPONENT( LArCellGainPathology )
DECLARE_COMPONENT( LArCellMerger )
DECLARE_COMPONENT( LArCellDeadOTXCorr )
DECLARE_COMPONENT( LArNoisyROTool )
DECLARE_COMPONENT( LArCollisionTimeAlg )
DECLARE_COMPONENT( LArTimeVetoAlg )
DECLARE_COMPONENT( LArCellContHVCorrTool )
DECLARE_COMPONENT( LArHVFraction )
DECLARE_COMPONENT( LArRAWtoSuperCell )
