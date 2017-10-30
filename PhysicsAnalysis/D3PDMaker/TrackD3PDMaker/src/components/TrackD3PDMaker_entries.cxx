// generic fillers
#include "../TrackFitQualityFillerTool.h"
#include "../CovarianceFillerTool.h"
#include "../ImpactSigmaFillerTool.h"
#include "../PerigeeFillerTool.h"
//#include "../PerigeeUnbiasedIPAtPVFillerTool.h"
#include "../PerigeeBLPredictionFillerTool.h"
#include "../TrackStateOnSurfaceFillerTool.h"
#include "../TrackTrackSummaryFillerTool.h"
#include "../TrackInfoFillerTool.h"
#include "../V0CandidateFillerTool.h"
#include "../PixelClusterFillerTool.h"
#include "../TrackPIDFillerTool.h"
#include "../TrackIsolationFillerTool.h"
#include "../TrackParametersFillerTool.h"
#include "../ErrorMatrixErrorFillerTool.h"
#include "../ErrorMatrixCovarianceFillerTool.h"
#include "../TrackParticleChargeFillerTool.h"

// Vertex fillers
#include "../VertexPositionFillerTool.h"
#include "../VertexTypeFillerTool.h"
#include "../VertexKineFillerTool.h"
#include "../VertexTrackAssociationFillerTool.h"
#include "../VertexPurityFillerTool.h"
#include "../BeamSpotFillerTool.h"

// Truth fillers
#include "../TruthTrackFillerTool.h"
#include "../TruthVertexFillerTool.h"
//#include "../TrackParticleMCClassifierFillerTool.h"

// generic associators
#include "../PerigeeCovarianceAssociationTool.h"
#include "../TSOFitQualityAssociationTool.h"
#include "../VxTrackAtVertexTrackParticleAssociationTool.h"
#include "../VxTrackAtVertexTrkTrackAssociationTool.h"
#include "../V0CandidateVxCandidateAssociationTool.h"
#include "../TrkPrepRawDataGenParticleAssociationTool.h"
#include "../TSOSPixelClusterAssociationTool.h"
#include "../TrackParametersErrorMatrixAssociationTool.h"

// Rec::TrackParticle associators
#include "../TrackParticleFitQualityAssociationTool.h"
#include "../TrackParticlePerigeeAtOOAssociationTool.h"
#include "../TrackParticlePerigeePairAtOOAssociationTool.h"
#include "../TrackParticlePerigeeAtPVAssociationTool.h"
#include "../TrackParticlePerigeeAtBSAssociationTool.h"
#include "../TrackParticleTrackSummaryAssociationTool.h"
#include "../TrackParticleInfoAssociationTool.h"
#include "../TrackParticleTSOAssociationTool.h"
#include "../TrackParticleVertexAssociationTool.h"
#include "../TrackParticleTruthAssociationTool.h"
#include "../TrackParticleDetailedTruthAssociationTool.h"
#include "../TrackParticleLastParametersAssociationTool.h"

// Rec::TrackParticle fillers
#include "../TrackParticleParametersAtBLFillerTool.h"

// Trk::Track associators
#include "../TrkTrackFitQualityAssociationTool.h"
#include "../TrkTrackPerigeeAssociationTool.h"
#include "../TrkTrackPerigeeAtPVAssociationTool.h"
#include "../TrkTrackPerigeeAtBSAssociationTool.h"
#include "../TrkTrackTrackSummaryAssociationTool.h"
#include "../TrkTrackInfoAssociationTool.h"
#include "../TrkTrackTSOAssociationTool.h"
#include "../TrkTrackVertexAssociationTool.h"
#include "../TrkTrackTruthAssociationTool.h"
#include "../TrkTrackDetailedTruthAssociationTool.h"

// Trk::Track fillers
#include "../TrkTrackParametersAtBLFillerTool.h"

// Vertex associators
#include "../VertexFitQualityAssociationTool.h"
#include "../VertexCovarianceAssociationTool.h"
//#include "../VertexTrackAssociationTool.h"
#include "../VertexTrackParticleAssociationTool.h"

// Other associators
#include "../PerigeePairOldPerigeeAssociationTool.h"
#include "../TrackBeamlineParametersAssociationTool.h"

// getters
#include "../TrkTrackGetterTool.h"
#include "../TrackParticleGetterTool.h"
#include "../TruthTrackGetterTool.h"
#include "../TruthVertexGetterTool.h"
#include "../PixelClusterCollectionGetterTool.h"
#include "../BeamSpotGetterTool.h"

// SCT tools
#include "../TRTPhaseFillerTool.h"
#include "../SCTByteStreamErrorFillerTool.h"
#include "../SCTClusterCollectionGetterTool.h"
#include "../SCTClusterFillerTool.h"
#include "../SCTClusterToTrackParticleAssociationTool.h"
#include "../SCTClusterSCTRDOAssociationTool.h"
#include "../SCTRDOCollectionGetterTool.h"
#include "../SCTRDOFillerTool.h"
#include "../SCTRDOClusterAssociationTool.h"
#include "../TrackParticleToSCTClusterAssociationTool.h"
#include "../TrackParticleToSCTClusterAssociationToolLight.h"
#include "../ClusterAssocFillerTool.h"
#include "../SCTTrackParticleToMuonAssociationTool.h"

//Pixel tools
#include "../PixelRDOCollectionGetterTool.h"
#include "../PixelRDOFillerTool.h"
#include "../PixelRDOClusterAssociationTool.h"
#include "../PixelClusterPixelRDOAssociationTool.h"
#include "../PixelClusterToTrackParticleAssociationTool.h"
#include "../TrackParticleToPixelClusterAssociationTool.h"
#include "../TrackParticleToPixelClusterAssociationToolLight.h"
#include "../ClusterAssocPixelFillerTool.h"


// SiHits
#include "../SiHitFillerTool.h"
#include "../SiHitTruthAssociationTool.h"

// TRTUncompressedHits
#include "../TRTUncompressedHitFillerTool.h"


DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackFitQualityFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CovarianceFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ImpactSigmaFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PerigeeFillerTool)
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PerigeeUnbiasedIPAtPVFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PerigeeBLPredictionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackStateOnSurfaceFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackTrackSummaryFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackInfoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PixelClusterFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackPIDFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackIsolationFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParametersFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ErrorMatrixErrorFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ErrorMatrixCovarianceFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleChargeFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, V0CandidateFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, V0ContainerGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VertexPositionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VertexTypeFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VertexKineFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VertexTrackAssociationFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VertexPurityFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, BeamSpotFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthTrackFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthVertexFillerTool)
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleMCClassifierFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PerigeeCovarianceAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TSOFitQualityAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VxTrackAtVertexTrackParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VxTrackAtVertexTrkTrackAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, V0CandidateVxCandidateAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkPrepRawDataGenParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TSOSPixelClusterAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParametersErrorMatrixAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleFitQualityAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticlePerigeeAtOOAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticlePerigeePairAtOOAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticlePerigeeAtPVAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticlePerigeeAtBSAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleTrackSummaryAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleInfoAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleTSOAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleTruthAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleDetailedTruthAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleLastParametersAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleParametersAtBLFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackFitQualityAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackPerigeeAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackPerigeeAtPVAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackPerigeeAtBSAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackTrackSummaryAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackInfoAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackTSOAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackTruthAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackDetailedTruthAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VertexFitQualityAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VertexCovarianceAssociationTool)
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VertexTrackAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VertexTrackParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PerigeePairOldPerigeeAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackBeamlineParametersAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthTrackGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthVertexGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PixelClusterCollectionGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, BeamSpotGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleDetailedTruthAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrkTrackParametersAtBLFillerTool)

// SCT tools
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TRTPhaseFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SCTByteStreamErrorFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SCTClusterCollectionGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SCTClusterFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SCTClusterToTrackParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SCTClusterSCTRDOAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SCTRDOCollectionGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SCTRDOFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SCTRDOClusterAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleToSCTClusterAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleToSCTClusterAssociationToolLight)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ClusterAssocFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SCTTrackParticleToMuonAssociationTool)

DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SiHitFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SiHitTruthAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SiHitContainerGetterTool)

DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TRTUncompressedHitFillerTool          )
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TRTUncompressedHitContainerGetterTool )


// Pixel tools
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PixelClusterPixelRDOAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PixelRDOCollectionGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PixelRDOFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PixelRDOClusterAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PixelClusterToTrackParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleToPixelClusterAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleToPixelClusterAssociationToolLight)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ClusterAssocPixelFillerTool)

