
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

DECLARE_ALGORITHM_FACTORY(MuFastSteering)
DECLARE_TOOL_FACTORY(MuCalStreamerTool)
DECLARE_TOOL_FACTORY(MuFastDataPreparator)
DECLARE_TOOL_FACTORY(TgcDataPreparator)
DECLARE_TOOL_FACTORY(RpcDataPreparator)
DECLARE_TOOL_FACTORY(MdtDataPreparator)
DECLARE_TOOL_FACTORY(CscDataPreparator)
DECLARE_TOOL_FACTORY(TgcRoadDefiner)
DECLARE_TOOL_FACTORY(TgcFit)
DECLARE_TOOL_FACTORY(RpcRoadDefiner)
DECLARE_TOOL_FACTORY(RpcPatFinder)
DECLARE_TOOL_FACTORY(MdtRegionDefiner)
DECLARE_TOOL_FACTORY(MuFastPatternFinder)
DECLARE_TOOL_FACTORY(MuFastStationFitter)
DECLARE_TOOL_FACTORY(MuFastTrackFitter)
DECLARE_TOOL_FACTORY(MuFastTrackExtrapolator)
DECLARE_TOOL_FACTORY(AlphaBetaEstimate)
DECLARE_TOOL_FACTORY(SagittaRadiusEstimate)
DECLARE_TOOL_FACTORY(PtFromAlphaBeta)
DECLARE_TOOL_FACTORY(PtFromRadius)
DECLARE_TOOL_FACTORY(PtBarrelLUT)
DECLARE_TOOL_FACTORY(PtEndcapLUT)
DECLARE_TOOL_FACTORY(AlignmentBarrelLUT)
DECLARE_TOOL_FACTORY(CscSegmentMaker)
DECLARE_TOOL_FACTORY(CscRegDict)

DECLARE_SERVICE_FACTORY(PtBarrelLUTSvc)
DECLARE_SERVICE_FACTORY(PtEndcapLUTSvc)
DECLARE_SERVICE_FACTORY(AlignmentBarrelLUTSvc)

