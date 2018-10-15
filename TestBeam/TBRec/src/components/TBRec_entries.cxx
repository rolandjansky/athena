#include "../TBAlgoSequencer.h"

#include "../TBXMLWriter.h"
#include "../TBXMLEventWriterTool.h"
#include "../TBXMLCaloCellWriterTool.h"
#include "../TBBPCRec.h"
#include "../TBMWPCRec.h"
#include "../TBScintillatorRec.h"
#include "../TBTailCatcherRec.h"
#include "../TBPlaneTrackingAlgo.h"
#include "../TBPhaseRec.h"
#include "../TBLArRawChannelBuilder.h"
#include "../TBECLArRawChannelBuilder.h"
#include "../TBEventStreamer.h"
#include "../TBPatternUnitStreamerTool.h"
#include "../TBPatternUnitStreamerToolH8.h"
#include "../TBEventInfoStreamerTool.h"
#include "../TBPreScalerStreamerTool.h"
#include "../CBNTAA_BeamInstrumentation.h"
#include "../CBNTAA_TBADCRaw.h"
#include "../CBNTAA_TBTDCRaw.h"
#include "../CBNTAA_TBInfo.h"
#include "../CBNTAA_TBTrack.h"
#include "../CBNTAA_TBScint.h"
#include "../CBNT_Timing.h"
#include "../CBNT_CaloH6.h"
#include "../CBNT_UpstreamMat.h"
#include "../TBPartIDCherenkovMuTag.h"
#include "../TBBeamQuality.h"
#include "../TBBeamQualityTool.h"
#include "../TBBeamQualityDoubleHitsTool.h"
#include "../TBBeamQualityMuonToolH6.h"
#include "../TBBeamQualityEMFractionTool.h"
#include "../TBBeamQualityTrackingTool.h"
#include "../TBPhaseStreamerTool.h"
#include "../TBEMECCellEtaCorrection.h"
#include "../TBClusterMaker.h"
#include "../TBTree_CaloClusterH6.h"
#include "../TBNoiseWrite.h"
#include "../TBCellNoiseCorrection.h"
#include "../TBCellContainerFillerTool.h"
#include "../CBNTAA_TBTPValidation.h"
#include "../TBTrackInfoFromTag.h"
#include "../TBBeamQualityMC.h"
#include "../TBXCryYTableRead.h"
#include "../TBDMContainerSplitter.h"
#include "../TBEMECXTalkToyModel.h"
#include "../TBExtrapolTrackToCaloTool.h"
#include "../TBTrackToCaloAlg.h"

DECLARE_COMPONENT( TBXMLWriter )
DECLARE_COMPONENT( TBAlgoSequencer )
DECLARE_COMPONENT( TBBPCRec )
DECLARE_COMPONENT( TBMWPCRec )
DECLARE_COMPONENT( TBScintillatorRec )
DECLARE_COMPONENT( TBTailCatcherRec )
DECLARE_COMPONENT( TBPlaneTrackingAlgo )
DECLARE_COMPONENT( TBPhaseRec )
DECLARE_COMPONENT( TBLArRawChannelBuilder )
DECLARE_COMPONENT( TBECLArRawChannelBuilder )
DECLARE_COMPONENT( TBEventStreamer )
DECLARE_COMPONENT( CBNTAA_BeamInstrumentation )
DECLARE_COMPONENT( CBNTAA_TBADCRaw )
DECLARE_COMPONENT( CBNTAA_TBTDCRaw )
DECLARE_COMPONENT( CBNTAA_TBInfo )
DECLARE_COMPONENT( CBNTAA_TBTrack )
DECLARE_COMPONENT( CBNTAA_TBScint )
DECLARE_COMPONENT( CBNT_Timing )
DECLARE_COMPONENT( CBNT_CaloH6 )
DECLARE_COMPONENT( CBNT_UpstreamMat )
DECLARE_COMPONENT( TBPartIDCherenkovMuTag )

DECLARE_COMPONENT( TBBeamQuality )
DECLARE_COMPONENT( TBNoiseWrite )
DECLARE_COMPONENT( TBTree_CaloClusterH6 )
DECLARE_COMPONENT( CBNTAA_TBTPValidation )
DECLARE_COMPONENT( TBTrackInfoFromTag )
DECLARE_COMPONENT( TBBeamQualityMC )
DECLARE_COMPONENT( TBXCryYTableRead )
DECLARE_COMPONENT( TBDMContainerSplitter )

DECLARE_COMPONENT( TBTrackToCaloAlg )

DECLARE_COMPONENT( TBXMLEventWriterTool )
DECLARE_COMPONENT( TBXMLCaloCellWriterTool )
DECLARE_COMPONENT( TBPatternUnitStreamerTool )
DECLARE_COMPONENT( TBPatternUnitStreamerToolH8 )
DECLARE_COMPONENT( TBEventInfoStreamerTool )
DECLARE_COMPONENT( TBPreScalerStreamerTool )

DECLARE_COMPONENT( TBBeamQualityDoubleHitsTool )
DECLARE_COMPONENT( TBBeamQualityMuonToolH6 )
DECLARE_COMPONENT( TBBeamQualityEMFractionTool )
DECLARE_COMPONENT( TBBeamQualityTrackingTool )

DECLARE_COMPONENT( TBPhaseStreamerTool )
DECLARE_COMPONENT( TBEMECCellEtaCorrection )
DECLARE_COMPONENT( TBClusterMaker )
DECLARE_COMPONENT( TBCellNoiseCorrection )
DECLARE_COMPONENT( TBCellContainerFillerTool )
DECLARE_COMPONENT( TBEMECXTalkToyModel )

DECLARE_COMPONENT( TBExtrapolTrackToCaloTool )

