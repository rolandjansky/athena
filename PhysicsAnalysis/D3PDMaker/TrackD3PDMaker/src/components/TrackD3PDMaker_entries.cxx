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


DECLARE_COMPONENT( D3PD::TrackFitQualityFillerTool )
DECLARE_COMPONENT( D3PD::CovarianceFillerTool )
DECLARE_COMPONENT( D3PD::ImpactSigmaFillerTool )
DECLARE_COMPONENT( D3PD::PerigeeFillerTool )
//DECLARE_COMPONENT( D3PD::PerigeeUnbiasedIPAtPVFillerTool )
DECLARE_COMPONENT( D3PD::PerigeeBLPredictionFillerTool )
DECLARE_COMPONENT( D3PD::TrackStateOnSurfaceFillerTool )
DECLARE_COMPONENT( D3PD::TrackTrackSummaryFillerTool )
DECLARE_COMPONENT( D3PD::TrackInfoFillerTool )
DECLARE_COMPONENT( D3PD::PixelClusterFillerTool )
DECLARE_COMPONENT( D3PD::TrackPIDFillerTool )
DECLARE_COMPONENT( D3PD::TrackIsolationFillerTool )
DECLARE_COMPONENT( D3PD::TrackParametersFillerTool )
DECLARE_COMPONENT( D3PD::ErrorMatrixErrorFillerTool )
DECLARE_COMPONENT( D3PD::ErrorMatrixCovarianceFillerTool )
DECLARE_COMPONENT( D3PD::TrackParticleChargeFillerTool )
DECLARE_COMPONENT( D3PD::V0CandidateFillerTool )
DECLARE_COMPONENT( D3PD::V0ContainerGetterTool )
DECLARE_COMPONENT( D3PD::VertexPositionFillerTool )
DECLARE_COMPONENT( D3PD::VertexTypeFillerTool )
DECLARE_COMPONENT( D3PD::VertexKineFillerTool )
DECLARE_COMPONENT( D3PD::VertexTrackAssociationFillerTool )
DECLARE_COMPONENT( D3PD::VertexPurityFillerTool )
DECLARE_COMPONENT( D3PD::BeamSpotFillerTool )
DECLARE_COMPONENT( D3PD::TruthTrackFillerTool )
DECLARE_COMPONENT( D3PD::TruthVertexFillerTool )
//DECLARE_COMPONENT( D3PD::TrackParticleMCClassifierFillerTool )
DECLARE_COMPONENT( D3PD::PerigeeCovarianceAssociationTool )
DECLARE_COMPONENT( D3PD::TSOFitQualityAssociationTool )
DECLARE_COMPONENT( D3PD::VxTrackAtVertexTrackParticleAssociationTool )
DECLARE_COMPONENT( D3PD::VxTrackAtVertexTrkTrackAssociationTool )
DECLARE_COMPONENT( D3PD::V0CandidateVxCandidateAssociationTool )
DECLARE_COMPONENT( D3PD::TrkPrepRawDataGenParticleAssociationTool )
DECLARE_COMPONENT( D3PD::TSOSPixelClusterAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParametersErrorMatrixAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleFitQualityAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticlePerigeeAtOOAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticlePerigeePairAtOOAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticlePerigeeAtPVAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticlePerigeeAtBSAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleTrackSummaryAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleInfoAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleTSOAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleVertexAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleTruthAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleDetailedTruthAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleLastParametersAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleParametersAtBLFillerTool )
DECLARE_COMPONENT( D3PD::TrkTrackFitQualityAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackPerigeeAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackPerigeeAtPVAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackPerigeeAtBSAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackTrackSummaryAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackInfoAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackTSOAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackVertexAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackTruthAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackDetailedTruthAssociationTool )
DECLARE_COMPONENT( D3PD::VertexFitQualityAssociationTool )
DECLARE_COMPONENT( D3PD::VertexCovarianceAssociationTool )
//DECLARE_COMPONENT( D3PD::VertexTrackAssociationTool )
DECLARE_COMPONENT( D3PD::VertexTrackParticleAssociationTool )
DECLARE_COMPONENT( D3PD::PerigeePairOldPerigeeAssociationTool )
DECLARE_COMPONENT( D3PD::TrackBeamlineParametersAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackGetterTool )
DECLARE_COMPONENT( D3PD::TrackParticleGetterTool )
DECLARE_COMPONENT( D3PD::TruthTrackGetterTool )
DECLARE_COMPONENT( D3PD::TruthVertexGetterTool )
DECLARE_COMPONENT( D3PD::PixelClusterCollectionGetterTool )
DECLARE_COMPONENT( D3PD::BeamSpotGetterTool )
DECLARE_COMPONENT( D3PD::TrackParticleDetailedTruthAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackParametersAtBLFillerTool )

// SCT tools
DECLARE_COMPONENT( D3PD::TRTPhaseFillerTool )
DECLARE_COMPONENT( D3PD::SCTByteStreamErrorFillerTool )
DECLARE_COMPONENT( D3PD::SCTClusterCollectionGetterTool )
DECLARE_COMPONENT( D3PD::SCTClusterFillerTool )
DECLARE_COMPONENT( D3PD::SCTClusterToTrackParticleAssociationTool )
DECLARE_COMPONENT( D3PD::SCTClusterSCTRDOAssociationTool )
DECLARE_COMPONENT( D3PD::SCTRDOCollectionGetterTool )
DECLARE_COMPONENT( D3PD::SCTRDOFillerTool )
DECLARE_COMPONENT( D3PD::SCTRDOClusterAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleToSCTClusterAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleToSCTClusterAssociationToolLight )
DECLARE_COMPONENT( D3PD::ClusterAssocFillerTool )
DECLARE_COMPONENT( D3PD::SCTTrackParticleToMuonAssociationTool )

DECLARE_COMPONENT( D3PD::SiHitFillerTool )
DECLARE_COMPONENT( D3PD::SiHitTruthAssociationTool )
DECLARE_COMPONENT( D3PD::SiHitContainerGetterTool )

DECLARE_COMPONENT( D3PD::TRTUncompressedHitFillerTool )
DECLARE_COMPONENT( D3PD::TRTUncompressedHitContainerGetterTool )


// Pixel tools
DECLARE_COMPONENT( D3PD::PixelClusterPixelRDOAssociationTool )
DECLARE_COMPONENT( D3PD::PixelRDOCollectionGetterTool )
DECLARE_COMPONENT( D3PD::PixelRDOFillerTool )
DECLARE_COMPONENT( D3PD::PixelRDOClusterAssociationTool )
DECLARE_COMPONENT( D3PD::PixelClusterToTrackParticleAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleToPixelClusterAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleToPixelClusterAssociationToolLight )
DECLARE_COMPONENT( D3PD::ClusterAssocPixelFillerTool )

