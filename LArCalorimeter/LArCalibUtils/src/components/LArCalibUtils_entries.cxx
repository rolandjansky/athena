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


DECLARE_COMPONENT( LArAutoCorrMaker )
DECLARE_COMPONENT( LArAutoCorrExtrapolate )
DECLARE_COMPONENT( LArAutoCorrBuilder )
DECLARE_COMPONENT( LArCalibDigitMaker )
DECLARE_COMPONENT( LArCaliWaveAverage )
DECLARE_COMPONENT( LArCaliWaveBuilder )
DECLARE_COMPONENT( LArCaliWaveBuilderXtalk )
DECLARE_COMPONENT( LArCaliWaveMerger )
DECLARE_COMPONENT( LArCaliWaveSelector )
DECLARE_COMPONENT( LArDeltaRespPredictor )
DECLARE_COMPONENT( LArFill )
DECLARE_COMPONENT( LArMasterWaveBuilder )
DECLARE_COMPONENT( LArOFCAlg )
DECLARE_COMPONENT( LArPedestalInPhysicsMaker )
DECLARE_COMPONENT( LArPedestalMaker )
DECLARE_COMPONENT( LArPedestalBuilder )
DECLARE_COMPONENT( LArPedestalAutoCorrBuilder )
DECLARE_COMPONENT( LArPhysWaveBuilder )
DECLARE_COMPONENT( LArPhysWaveShifter )
DECLARE_COMPONENT( LArRampBuilder )
DECLARE_COMPONENT( LArRTMParamExtractor )
DECLARE_COMPONENT( LArTCMPhysWavePredictor )
DECLARE_COMPONENT( LArStripsCrossTalkCorrector )
DECLARE_COMPONENT( LArPhysWavePredictor )
DECLARE_COMPONENT( LArTimeTuning )
DECLARE_COMPONENT( LArTimeTuningNtuple )
DECLARE_COMPONENT( LArDumpShapes )
DECLARE_COMPONENT( LArRampCorr )
DECLARE_COMPONENT( LArAccumulatedCalibDigitContSplitter )
DECLARE_COMPONENT( LArRampPatcher )
DECLARE_COMPONENT( LArCaliWavePatcher )
DECLARE_COMPONENT( LArOFPhaseFill )
DECLARE_COMPONENT( LArOFPhasePicker )
DECLARE_COMPONENT( LArHVCorrMaker )
DECLARE_COMPONENT( LArCalibShortCorrector )
DECLARE_COMPONENT( LArTimePhysPrediction )
DECLARE_COMPONENT( LArRampAdHocPatchingAlg )
//DECLARE_COMPONENT( LArPedestalCopyAlg )
DECLARE_COMPONENT( LArPhysWaveCopyAlg )
DECLARE_COMPONENT( LArDAC2uAMCCopyAlg )
DECLARE_COMPONENT( LArTdriftCompleteCopyAlg )
DECLARE_COMPONENT( LArShapeCorrector )
DECLARE_COMPONENT( LArAutoCorrToolToDB )
DECLARE_COMPONENT( LArDuplicateConstants )

  //DECLARE_COMPONENT( LArOFCPatcher )
DECLARE_COMPONENT( LArAutoCorrDecoderTool )
DECLARE_COMPONENT( LArDigitOscillationCorrTool )
DECLARE_COMPONENT( LArDeltaRespTool )
DECLARE_COMPONENT( LArPhysWaveTool )
DECLARE_COMPONENT( LArPhysWaveHECTool )
DECLARE_COMPONENT( LArTCMFitterTool )
DECLARE_COMPONENT( LArWFParamTool )
DECLARE_COMPONENT( LArPhaseToolConst )
DECLARE_COMPONENT( LArPhaseToolMC )
DECLARE_COMPONENT( LArPhaseToolTB )

