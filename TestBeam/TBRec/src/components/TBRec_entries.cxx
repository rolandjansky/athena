#include "TBRec/TBAlgoSequencer.h"

#include "TBRec/TBXMLWriter.h"
#include "TBRec/TBXMLEventWriterTool.h"
#include "TBRec/TBXMLCaloCellWriterTool.h"
#include "TBRec/TBBPCRec.h"
#include "TBRec/TBMWPCRec.h"
#include "TBRec/TBScintillatorRec.h"
#include "TBRec/TBTailCatcherRec.h"
#include "TBRec/TBPlaneTrackingAlgo.h"
#include "TBRec/TBPhaseRec.h"
#include "TBRec/TBLArRawChannelBuilder.h"
#include "TBRec/TBECLArRawChannelBuilder.h"
#include "TBRec/TBEventStreamer.h"
#include "TBRec/TBPatternUnitStreamerTool.h"
#include "TBRec/TBPatternUnitStreamerToolH8.h"
#include "TBRec/TBEventInfoStreamerTool.h"
#include "TBRec/TBPreScalerStreamerTool.h"
#include "TBRec/CBNTAA_BeamInstrumentation.h"
#include "TBRec/CBNTAA_TBADCRaw.h"
#include "TBRec/CBNTAA_TBTDCRaw.h"
#include "TBRec/CBNTAA_TBInfo.h"
#include "TBRec/CBNTAA_TBTrack.h"
#include "TBRec/CBNTAA_TBScint.h"
#include "TBRec/CBNT_Timing.h"
#include "TBRec/CBNT_CaloH6.h"
#include "TBRec/CBNT_UpstreamMat.h"
#include "TBRec/TBPartIDCherenkovMuTag.h"
#include "TBRec/TBBeamQuality.h"
#include "TBRec/TBBeamQualityTool.h"
#include "TBRec/TBBeamQualityDoubleHitsTool.h"
#include "TBRec/TBBeamQualityMuonToolH6.h"
#include "TBRec/TBBeamQualityEMFractionTool.h"
#include "TBRec/TBBeamQualityTrackingTool.h"
#include "TBRec/TBPhaseStreamerTool.h"
#include "TBRec/TBEMECCellEtaCorrection.h"
#include "TBRec/TBClusterMaker.h"
#include "TBRec/TBTree_CaloClusterH6.h"
#include "TBRec/TBNoiseWrite.h"
#include "TBRec/TBCellNoiseCorrection.h"
#include "TBRec/TBCellContainerFillerTool.h"
#include "TBRec/CBNTAA_TBTPValidation.h"
#include "TBRec/TBTrackInfoFromTag.h"
#include "TBRec/TBBeamQualityMC.h"
#include "TBRec/TBXCryYTableRead.h"
#include "TBRec/TBDMContainerSplitter.h"
#include "TBRec/TBEMECXTalkToyModel.h"
#include "TBRec/TBExtrapolTrackToCaloTool.h"
#include "TBRec/TBTrackToCaloAlg.h"

DECLARE_ALGORITHM_FACTORY( TBXMLWriter )
DECLARE_ALGORITHM_FACTORY( TBAlgoSequencer )
DECLARE_ALGORITHM_FACTORY( TBBPCRec )
DECLARE_ALGORITHM_FACTORY( TBMWPCRec )
DECLARE_ALGORITHM_FACTORY( TBScintillatorRec )
DECLARE_ALGORITHM_FACTORY( TBTailCatcherRec )
DECLARE_ALGORITHM_FACTORY( TBPlaneTrackingAlgo )
DECLARE_ALGORITHM_FACTORY( TBPhaseRec )
DECLARE_ALGORITHM_FACTORY( TBLArRawChannelBuilder )
DECLARE_ALGORITHM_FACTORY( TBECLArRawChannelBuilder )
DECLARE_ALGORITHM_FACTORY( TBEventStreamer )
DECLARE_ALGORITHM_FACTORY( CBNTAA_BeamInstrumentation )
DECLARE_ALGORITHM_FACTORY( CBNTAA_TBADCRaw )
DECLARE_ALGORITHM_FACTORY( CBNTAA_TBTDCRaw )
DECLARE_ALGORITHM_FACTORY( CBNTAA_TBInfo )
DECLARE_ALGORITHM_FACTORY( CBNTAA_TBTrack )
DECLARE_ALGORITHM_FACTORY( CBNTAA_TBScint )
DECLARE_ALGORITHM_FACTORY( CBNT_Timing )
DECLARE_ALGORITHM_FACTORY( CBNT_CaloH6 )
DECLARE_ALGORITHM_FACTORY( CBNT_UpstreamMat )
DECLARE_ALGORITHM_FACTORY( TBPartIDCherenkovMuTag )

DECLARE_ALGORITHM_FACTORY(  TBBeamQuality )
DECLARE_ALGORITHM_FACTORY( TBNoiseWrite )
DECLARE_ALGORITHM_FACTORY( TBTree_CaloClusterH6 )
DECLARE_ALGORITHM_FACTORY( CBNTAA_TBTPValidation )
DECLARE_ALGORITHM_FACTORY( TBTrackInfoFromTag )
DECLARE_ALGORITHM_FACTORY( TBBeamQualityMC )
DECLARE_ALGORITHM_FACTORY( TBXCryYTableRead )
DECLARE_ALGORITHM_FACTORY( TBDMContainerSplitter )

DECLARE_ALGORITHM_FACTORY( TBTrackToCaloAlg )

DECLARE_TOOL_FACTORY( TBXMLEventWriterTool )
DECLARE_TOOL_FACTORY( TBXMLCaloCellWriterTool )
DECLARE_TOOL_FACTORY( TBPatternUnitStreamerTool )
DECLARE_TOOL_FACTORY( TBPatternUnitStreamerToolH8 )
DECLARE_TOOL_FACTORY( TBEventInfoStreamerTool )
DECLARE_TOOL_FACTORY( TBPreScalerStreamerTool )

DECLARE_TOOL_FACTORY(  TBBeamQualityDoubleHitsTool )
DECLARE_TOOL_FACTORY(  TBBeamQualityMuonToolH6 )
DECLARE_TOOL_FACTORY(  TBBeamQualityEMFractionTool )
DECLARE_TOOL_FACTORY(  TBBeamQualityTrackingTool )

DECLARE_TOOL_FACTORY( TBPhaseStreamerTool )
DECLARE_TOOL_FACTORY( TBEMECCellEtaCorrection )
DECLARE_TOOL_FACTORY( TBClusterMaker )
DECLARE_TOOL_FACTORY( TBCellNoiseCorrection )
DECLARE_TOOL_FACTORY( TBCellContainerFillerTool )
DECLARE_TOOL_FACTORY( TBEMECXTalkToyModel )

DECLARE_TOOL_FACTORY( TBExtrapolTrackToCaloTool )

