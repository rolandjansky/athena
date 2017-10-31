
#include "TrigL2MuonSA/MuFastSteering.h"
#include "TrigL2MuonSA/MuFastDataPreparator.h"
#include "TrigL2MuonSA/TgcDataPreparator.h"
#include "TrigL2MuonSA/RpcDataPreparator.h"
#include "TrigL2MuonSA/MdtDataPreparator.h"
#include "TrigL2MuonSA/CscDataPreparator.h"
#include "TrigL2MuonSA/TgcRoadDefiner.h"
#include "TrigL2MuonSA/TgcFit.h"
#include "TrigL2MuonSA/RpcRoadDefiner.h"
#include "TrigL2MuonSA/RpcPatFinder.h"
#include "TrigL2MuonSA/MdtRegionDefiner.h"
#include "TrigL2MuonSA/MuFastPatternFinder.h"
#include "TrigL2MuonSA/MuFastTrackFitter.h"
#include "TrigL2MuonSA/MuFastStationFitter.h"
#include "TrigL2MuonSA/MuFastTrackExtrapolator.h"
#include "TrigL2MuonSA/AlphaBetaEstimate.h"
#include "TrigL2MuonSA/SagittaRadiusEstimate.h"
#include "TrigL2MuonSA/PtFromAlphaBeta.h"
#include "TrigL2MuonSA/PtFromRadius.h"
#include "TrigL2MuonSA/PtBarrelLUT.h"
#include "TrigL2MuonSA/PtEndcapLUT.h"
#include "TrigL2MuonSA/AlignmentBarrelLUT.h"
#include "TrigL2MuonSA/PtBarrelLUTSvc.h"
#include "TrigL2MuonSA/PtEndcapLUTSvc.h"
#include "TrigL2MuonSA/AlignmentBarrelLUTSvc.h"
#include "TrigL2MuonSA/MuCalStreamerTool.h"
#include "TrigL2MuonSA/CscSegmentMaker.h"
#include "TrigL2MuonSA/CscRegUtils.h"

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

DECLARE_COMPONENT( PtBarrelLUTSvc )
DECLARE_COMPONENT( PtEndcapLUTSvc )
DECLARE_COMPONENT( AlignmentBarrelLUTSvc )

