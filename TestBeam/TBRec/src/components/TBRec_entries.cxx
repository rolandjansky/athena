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

