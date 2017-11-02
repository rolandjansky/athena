#include "LArCellRec/LArG3Escale.h"
#include "LArCellRec/LArG3Escale_TDR.h"
#include "LArCellRec/LArNonLinearity.h"
#include "LArCellRec/LArCellBuilderFromLArHitTool.h"
#include "LArCellRec/LArCellBuilderFromLArRawChannelTool.h"
#include "LArCellRec/LArCellEmMiscalib.h"
#include "LArCellRec/LArCellRescaler.h"
#include "LArCellRec/LArCellMaskingTool.h"
#include "LArCellRec/LArCellHVCorr.h"
#include "LArCellRec/LArCellRecalibration.h"
#include "LArCellRec/LArCellNoiseMaskingTool.h"
#include "LArCellRec/LArBadFebMaskingTool.h"
#include "LArCellRec/LArCellGainPathology.h"
#include "LArCellRec/LArCellMerger.h"
#include "LArCellRec/LArCellDeadOTXCorr.h"
#include "LArCellRec/LArNoisyROAlg.h"
#include "LArCellRec/LArNoisyROTool.h"
#include "LArCellRec/LArCollisionTimeAlg.h"
#include "LArCellRec/LArTimeVetoAlg.h"


DECLARE_COMPONENT( LArNoisyROAlg )
DECLARE_COMPONENT( LArG3Escale )
DECLARE_COMPONENT( LArG3Escale_TDR )
DECLARE_COMPONENT( LArNonLinearity )
DECLARE_COMPONENT( LArCellBuilderFromLArHitTool )
DECLARE_COMPONENT( LArCellBuilderFromLArRawChannelTool )
DECLARE_COMPONENT( LArCellEmMiscalib )
DECLARE_COMPONENT( LArCellRescaler )
DECLARE_COMPONENT( LArCellMaskingTool )
DECLARE_COMPONENT( LArCellHVCorr )
DECLARE_COMPONENT( LArCellRecalibration )
DECLARE_COMPONENT( LArCellNoiseMaskingTool )
DECLARE_COMPONENT( LArBadFebMaskingTool )
DECLARE_COMPONENT( LArCellGainPathology )
DECLARE_COMPONENT( LArCellMerger )
DECLARE_COMPONENT( LArCellDeadOTXCorr )
DECLARE_COMPONENT( LArNoisyROTool )
DECLARE_COMPONENT( LArCollisionTimeAlg )
DECLARE_COMPONENT( LArTimeVetoAlg )

