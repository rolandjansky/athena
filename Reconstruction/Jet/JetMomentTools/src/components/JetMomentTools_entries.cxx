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

DECLARE_COMPONENT( JetCaloEnergies )
DECLARE_COMPONENT( JetCaloQualityTool )
DECLARE_COMPONENT( JetWidthTool )
DECLARE_COMPONENT( JetVertexFractionTool )
DECLARE_COMPONENT( JetVertexTaggerTool )
DECLARE_COMPONENT( JetForwardJvtTool )
DECLARE_COMPONENT( JetTrackMomentsTool )
DECLARE_COMPONENT( JetTrackSumMomentsTool )
DECLARE_COMPONENT( JetClusterMomentsTool )
DECLARE_COMPONENT( JetVoronoiMomentsTool )
DECLARE_COMPONENT( JetMuonSegmentMomentsTool )
DECLARE_COMPONENT( JetPtAssociationTool )
DECLARE_COMPONENT( JetIsolationTool )
DECLARE_COMPONENT( JetLArHVTool )
DECLARE_COMPONENT( JetOriginCorrectionTool )
DECLARE_COMPONENT( JetECPSFractionTool )
DECLARE_COMPONENT( JetConstitFourMomTool )

#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( JetBadChanCorrTool )
DECLARE_COMPONENT( JetCaloCellQualityTool )
#endif

