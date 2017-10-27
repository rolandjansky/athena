#include "JetRecTools/JetTrackSelectionTool.h"
#include "JetRecTools/SimpleJetTrackSelectionTool.h"
#include "JetRecTools/TrackVertexAssociationTool.h"
#include "JetRecTools/TrackPseudoJetGetter.h"
#include "JetRecTools/PFlowPseudoJetGetter.h"
#include "JetRecTools/JetConstituentModSequence.h"
#include "JetRecTools/JetConstituentModifierBase.h"
#include "JetRecTools/CaloClusterConstituentsOrigin.h"
#include "JetRecTools/SoftKillerWeightTool.h"
#include "JetRecTools/VoronoiWeightTool.h"
#include "JetRecTools/ClusterAtEMScaleTool.h"
#include "JetRecTools/ConstitTimeCutTool.h"
#include "JetRecTools/ConstituentSubtractorTool.h"
#include "JetRecTools/JetInputElRemovalTool.h"
#include "JetRecTools/CorrectPFOTool.h"
#include "JetRecTools/ChargedHadronSubtractionTool.h"
#include "JetRecTools/PuppiWeightTool.h"

DECLARE_COMPONENT( JetTrackSelectionTool )
DECLARE_COMPONENT( SimpleJetTrackSelectionTool )
DECLARE_COMPONENT( TrackVertexAssociationTool )
DECLARE_COMPONENT( TrackPseudoJetGetter )
DECLARE_COMPONENT( PFlowPseudoJetGetter )
DECLARE_COMPONENT( JetConstituentModSequence )
DECLARE_COMPONENT( JetConstituentModifierBase )
DECLARE_COMPONENT( CaloClusterConstituentsOrigin )
DECLARE_COMPONENT( SoftKillerWeightTool )
DECLARE_COMPONENT( VoronoiWeightTool )
DECLARE_COMPONENT( ClusterAtEMScaleTool )
DECLARE_COMPONENT( ConstitTimeCutTool )
DECLARE_COMPONENT( ConstituentSubtractorTool )
DECLARE_COMPONENT( JetInputElRemovalTool )
DECLARE_COMPONENT( CorrectPFOTool )
DECLARE_COMPONENT( ChargedHadronSubtractionTool )
DECLARE_COMPONENT( PuppiWeightTool )

