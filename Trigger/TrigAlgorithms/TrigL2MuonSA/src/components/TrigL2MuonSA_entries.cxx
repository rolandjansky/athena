
#include "../MuFastSteering.h"
#include "../MuFastDataPreparator.h"
#include "../TgcDataPreparator.h"
#include "TrigL2MuonSA/RpcDataPreparator.h"
#include "../MdtDataPreparator.h"
#include "../CscDataPreparator.h"
#include "../TgcRoadDefiner.h"
#include "../TgcFit.h"
#include "../RpcRoadDefiner.h"
#include "../RpcPatFinder.h"
#include "../MdtRegionDefiner.h"
#include "../MuFastPatternFinder.h"
#include "../MuFastTrackFitter.h"
#include "../MuFastStationFitter.h"
#include "../MuFastTrackExtrapolator.h"
#include "../AlphaBetaEstimate.h"
#include "../SagittaRadiusEstimate.h"
#include "../PtFromAlphaBeta.h"
#include "../PtFromRadius.h"
#include "../PtBarrelLUT.h"
#include "../PtEndcapLUT.h"
#include "../AlignmentBarrelLUT.h"
#include "../PtBarrelLUTSvc.h"
#include "../PtEndcapLUTSvc.h"
#include "../AlignmentBarrelLUTSvc.h"
#include "../MuCalStreamerTool.h"
#include "../CscSegmentMaker.h"
#include "../CscRegUtils.h"
#include "../StgcDataPreparator.h"
#include "../MmDataPreparator.h"
#include "../NswPatternFinder.h"
#include "../NswStationFitter.h"

using namespace TrigL2MuonSA;

DECLARE_COMPONENT( MuFastSteering )
DECLARE_COMPONENT( MuCalStreamerTool )
DECLARE_COMPONENT( MuFastDataPreparator )
DECLARE_COMPONENT( TgcDataPreparator )
DECLARE_COMPONENT( RpcDataPreparator )
DECLARE_COMPONENT( MdtDataPreparator )
DECLARE_COMPONENT( CscDataPreparator )
DECLARE_COMPONENT( TgcRoadDefiner )
DECLARE_COMPONENT( TgcFit )
DECLARE_COMPONENT( RpcRoadDefiner )
DECLARE_COMPONENT( RpcPatFinder )
DECLARE_COMPONENT( MdtRegionDefiner )
DECLARE_COMPONENT( MuFastPatternFinder )
DECLARE_COMPONENT( MuFastStationFitter )
DECLARE_COMPONENT( MuFastTrackFitter )
DECLARE_COMPONENT( MuFastTrackExtrapolator )
DECLARE_COMPONENT( AlphaBetaEstimate )
DECLARE_COMPONENT( SagittaRadiusEstimate )
DECLARE_COMPONENT( PtFromAlphaBeta )
DECLARE_COMPONENT( PtFromRadius )
DECLARE_COMPONENT( PtBarrelLUT )
DECLARE_COMPONENT( PtEndcapLUT )
DECLARE_COMPONENT( AlignmentBarrelLUT )
DECLARE_COMPONENT( CscSegmentMaker )
DECLARE_COMPONENT( CscRegDict )
DECLARE_COMPONENT( StgcDataPreparator )
DECLARE_COMPONENT( MmDataPreparator )
DECLARE_COMPONENT( NswPatternFinder )
DECLARE_COMPONENT( NswStationFitter )

DECLARE_COMPONENT( PtBarrelLUTSvc )
DECLARE_COMPONENT( PtEndcapLUTSvc )
DECLARE_COMPONENT( AlignmentBarrelLUTSvc )

