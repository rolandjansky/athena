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


DECLARE_ALGORITHM_FACTORY( LArNoisyROAlg )
DECLARE_TOOL_FACTORY( LArG3Escale )
DECLARE_TOOL_FACTORY( LArG3Escale_TDR )
DECLARE_TOOL_FACTORY( LArNonLinearity )
DECLARE_TOOL_FACTORY( LArCellBuilderFromLArHitTool )
DECLARE_TOOL_FACTORY( LArCellBuilderFromLArRawChannelTool )
DECLARE_TOOL_FACTORY (LArCellEmMiscalib)
DECLARE_TOOL_FACTORY (LArCellRescaler)
DECLARE_TOOL_FACTORY (LArCellMaskingTool)
DECLARE_TOOL_FACTORY (LArCellHVCorr)
DECLARE_TOOL_FACTORY (LArCellRecalibration)
DECLARE_TOOL_FACTORY (LArCellNoiseMaskingTool)
DECLARE_TOOL_FACTORY (LArBadFebMaskingTool)
DECLARE_TOOL_FACTORY (LArCellGainPathology)
DECLARE_TOOL_FACTORY (LArCellMerger)
DECLARE_TOOL_FACTORY (LArCellDeadOTXCorr)
DECLARE_TOOL_FACTORY (LArNoisyROTool)
DECLARE_ALGORITHM_FACTORY ( LArCollisionTimeAlg )
DECLARE_ALGORITHM_FACTORY ( LArTimeVetoAlg )

