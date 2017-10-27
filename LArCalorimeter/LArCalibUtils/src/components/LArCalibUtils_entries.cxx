#include "LArCalibUtils/LArAutoCorrMaker.h"
#include "LArCalibUtils/LArAutoCorrExtrapolate.h"
#include "LArCalibUtils/LArAutoCorrBuilder.h"
#include "LArCalibUtils/LArAutoCorrDecoderTool.h"
#include "LArCalibUtils/LArCalibDigitMaker.h"
#include "LArCalibUtils/LArCaliWaveAverage.h"
#include "LArCalibUtils/LArCaliWaveBuilder.h"
#include "LArCalibUtils/LArCaliWaveBuilderXtalk.h"
#include "LArCalibUtils/LArCaliWaveMerger.h"
#include "LArCalibUtils/LArCaliWaveSelector.h"
#include "LArCalibUtils/LArDeltaRespPredictor.h"
#include "LArCalibUtils/LArDeltaRespTool.h"
#include "LArCalibUtils/LArDigitOscillationCorrTool.h"
#include "LArCalibUtils/LArFill.h"
#include "LArCalibUtils/LArMasterWaveBuilder.h"
#include "LArCalibUtils/LArOFCAlg.h"
#include "LArCalibUtils/LArPedestalInPhysicsMaker.h"
#include "LArCalibUtils/LArPedestalMaker.h"
#include "LArCalibUtils/LArPedestalBuilder.h"
#include "LArCalibUtils/LArPhysWaveBuilder.h"
#include "LArCalibUtils/LArPhysWaveShifter.h"
#include "LArCalibUtils/LArPhysWaveTool.h"
#include "LArCalibUtils/LArPhysWaveHECTool.h"
#include "LArCalibUtils/LArRampBuilder.h"
#include "LArCalibUtils/LArRTMParamExtractor.h"
#include "LArCalibUtils/LArStripsCrossTalkCorrector.h"
#include "LArCalibUtils/LArTCMFitterTool.h"
#include "LArCalibUtils/LArTCMPhysWavePredictor.h"
#include "LArCalibUtils/LArPhysWavePredictor.h"
#include "LArCalibUtils/LArTimeTuning.h"
#include "LArCalibUtils/LArTimeTuningNtuple.h"
#include "LArCalibUtils/LArWFParamTool.h"
#include "LArCalibUtils/LArPhaseToolConst.h"
#include "LArCalibUtils/LArPhaseToolMC.h"
#include "LArCalibUtils/LArPhaseToolTB.h"
#include "LArCalibUtils/LArDumpShapes.h"
#include "LArCalibUtils/LArRampCorr.h"
#include "LArCalibUtils/LArAccumulatedCalibDigitContSplitter.h"
#include "LArCalibUtils/LArOFPhaseFill.h"
#include "LArCalibUtils/LArOFPhasePicker.h"
#include "LArCalibUtils/LArHVCorrMaker.h"
#include "LArCalibUtils/LArCalibShortCorrector.h"
#include "LArCalibUtils/LArTimePhysPrediction.h"
#include "LArCalibUtils/LArPedestalAutoCorrBuilder.h"
#include "LArCalibUtils/LArRampAdHocPatchingAlg.h"
#include "LArCalibUtils/LArShapeCorrector.h"
#include "LArCalibUtils/LArAutoCorrToolToDB.h"
#include "LArCalibUtils/LArDuplicateConstants.h"
#include "LArCalibUtils/LArCalibPatchingAlg.h"
#include "LArCalibUtils/LArCalibCopyAlg.h"



typedef LArCalibPatchingAlg<LArRampComplete> LArRampPatcher;
//typedef LArCalibPatchingAlg<LArOFCComplete> LArOFCPatcher;
typedef LArCalibPatchingAlg<LArCaliWaveContainer> LArCaliWavePatcher;
//#include "LArRawConditions/LArMphysOverMcalComplete.h"
typedef LArCalibPatchingAlg<LArMphysOverMcalComplete> LArMphysOverMcalPatcher;

//typedef LArCalibCopyAlg<LArPedestalComplete> LArPedestalCopyAlg;
typedef LArCalibCopyAlg<LArPhysWaveContainer> LArPhysWaveCopyAlg;
typedef LArCalibCopyAlg<LArDAC2uAMC> LArDAC2uAMCCopyAlg;
typedef LArCalibCopyAlg<LArTdriftComplete> LArTdriftCompleteCopyAlg;


DECLARE_ALGORITHM_FACTORY( LArAutoCorrMaker )
DECLARE_ALGORITHM_FACTORY( LArAutoCorrExtrapolate )
DECLARE_ALGORITHM_FACTORY( LArAutoCorrBuilder )
DECLARE_ALGORITHM_FACTORY( LArCalibDigitMaker )
DECLARE_ALGORITHM_FACTORY( LArCaliWaveAverage )
DECLARE_ALGORITHM_FACTORY( LArCaliWaveBuilder )
DECLARE_ALGORITHM_FACTORY( LArCaliWaveBuilderXtalk )
DECLARE_ALGORITHM_FACTORY( LArCaliWaveMerger )
DECLARE_ALGORITHM_FACTORY( LArCaliWaveSelector )
DECLARE_ALGORITHM_FACTORY( LArDeltaRespPredictor )
DECLARE_ALGORITHM_FACTORY( LArFill )
DECLARE_ALGORITHM_FACTORY( LArMasterWaveBuilder )
DECLARE_ALGORITHM_FACTORY( LArOFCAlg )
DECLARE_ALGORITHM_FACTORY( LArPedestalInPhysicsMaker )
DECLARE_ALGORITHM_FACTORY( LArPedestalMaker )
DECLARE_ALGORITHM_FACTORY( LArPedestalBuilder )
DECLARE_ALGORITHM_FACTORY( LArPedestalAutoCorrBuilder )
DECLARE_ALGORITHM_FACTORY( LArPhysWaveBuilder )
DECLARE_ALGORITHM_FACTORY( LArPhysWaveShifter )
DECLARE_ALGORITHM_FACTORY( LArRampBuilder )
DECLARE_ALGORITHM_FACTORY( LArRTMParamExtractor )
DECLARE_ALGORITHM_FACTORY( LArTCMPhysWavePredictor )
DECLARE_ALGORITHM_FACTORY( LArStripsCrossTalkCorrector )
DECLARE_ALGORITHM_FACTORY( LArPhysWavePredictor )
DECLARE_ALGORITHM_FACTORY( LArTimeTuning )
DECLARE_ALGORITHM_FACTORY( LArTimeTuningNtuple )
DECLARE_ALGORITHM_FACTORY( LArDumpShapes )
DECLARE_ALGORITHM_FACTORY( LArRampCorr )
DECLARE_ALGORITHM_FACTORY( LArAccumulatedCalibDigitContSplitter )
DECLARE_ALGORITHM_FACTORY( LArRampPatcher )
DECLARE_ALGORITHM_FACTORY( LArCaliWavePatcher )
DECLARE_ALGORITHM_FACTORY( LArOFPhaseFill )
DECLARE_ALGORITHM_FACTORY( LArOFPhasePicker )
DECLARE_ALGORITHM_FACTORY( LArHVCorrMaker )
DECLARE_ALGORITHM_FACTORY( LArCalibShortCorrector )
DECLARE_ALGORITHM_FACTORY( LArTimePhysPrediction )
DECLARE_ALGORITHM_FACTORY( LArRampAdHocPatchingAlg )
//DECLARE_ALGORITHM_FACTORY( LArPedestalCopyAlg )
DECLARE_ALGORITHM_FACTORY( LArPhysWaveCopyAlg )
DECLARE_ALGORITHM_FACTORY( LArDAC2uAMCCopyAlg )
DECLARE_ALGORITHM_FACTORY( LArTdriftCompleteCopyAlg )
DECLARE_ALGORITHM_FACTORY( LArShapeCorrector )
DECLARE_ALGORITHM_FACTORY( LArAutoCorrToolToDB )
DECLARE_ALGORITHM_FACTORY( LArDuplicateConstants )

  //DECLARE_ALGORITHM_FACTORY (LArOFCPatcher)
DECLARE_TOOL_FACTORY( LArAutoCorrDecoderTool )
DECLARE_TOOL_FACTORY( LArDigitOscillationCorrTool )
DECLARE_TOOL_FACTORY( LArDeltaRespTool )
DECLARE_TOOL_FACTORY( LArPhysWaveTool )
DECLARE_TOOL_FACTORY( LArPhysWaveHECTool )
DECLARE_TOOL_FACTORY( LArTCMFitterTool )
DECLARE_TOOL_FACTORY( LArWFParamTool )
DECLARE_TOOL_FACTORY( LArPhaseToolConst )
DECLARE_TOOL_FACTORY( LArPhaseToolMC )
DECLARE_TOOL_FACTORY( LArPhaseToolTB )

