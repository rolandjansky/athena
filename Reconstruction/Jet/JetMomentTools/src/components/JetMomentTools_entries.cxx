#include "PFlowUtils/IWeightPFOTool.h"
#include "JetMomentTools/JetCaloEnergies.h"
#include "JetMomentTools/JetCaloQualityTool.h"
#include "JetMomentTools/JetWidthTool.h"
#include "JetMomentTools/JetVertexFractionTool.h"
#include "JetMomentTools/JetVertexTaggerTool.h"
#include "JetMomentTools/JetForwardJvtTool.h"
#include "JetMomentTools/JetTrackMomentsTool.h"
#include "JetMomentTools/JetTrackSumMomentsTool.h"
#include "JetMomentTools/JetClusterMomentsTool.h"
#include "JetMomentTools/JetVoronoiMomentsTool.h"
#include "JetMomentTools/JetMuonSegmentMomentsTool.h"
#include "JetMomentTools/JetPtAssociationTool.h"
#include "JetMomentTools/JetIsolationTool.h"
#include "JetMomentTools/JetLArHVTool.h"
#include "JetMomentTools/JetOriginCorrectionTool.h"
#include "JetMomentTools/JetECPSFractionTool.h"
#include "JetMomentTools/JetConstitFourMomTool.h"

#ifndef XAOD_ANALYSIS
#include "JetMomentTools/JetBadChanCorrTool.h"
#include "../JetCaloCellQualityTool.h"
#endif

DECLARE_TOOL_FACTORY(JetCaloEnergies)
DECLARE_TOOL_FACTORY(JetCaloQualityTool)
DECLARE_TOOL_FACTORY(JetWidthTool)
DECLARE_TOOL_FACTORY(JetVertexFractionTool)
DECLARE_TOOL_FACTORY(JetVertexTaggerTool)
DECLARE_TOOL_FACTORY(JetForwardJvtTool)
DECLARE_TOOL_FACTORY(JetTrackMomentsTool)
DECLARE_TOOL_FACTORY(JetTrackSumMomentsTool)
DECLARE_TOOL_FACTORY(JetClusterMomentsTool)
DECLARE_TOOL_FACTORY(JetVoronoiMomentsTool)
DECLARE_TOOL_FACTORY(JetMuonSegmentMomentsTool)
DECLARE_TOOL_FACTORY(JetPtAssociationTool)
DECLARE_TOOL_FACTORY(JetIsolationTool)
DECLARE_TOOL_FACTORY(JetLArHVTool)
DECLARE_TOOL_FACTORY(JetOriginCorrectionTool)
DECLARE_TOOL_FACTORY(JetECPSFractionTool)
DECLARE_TOOL_FACTORY(JetConstitFourMomTool)

#ifndef XAOD_ANALYSIS
DECLARE_TOOL_FACTORY(JetBadChanCorrTool)
DECLARE_TOOL_FACTORY(JetCaloCellQualityTool)
#endif

