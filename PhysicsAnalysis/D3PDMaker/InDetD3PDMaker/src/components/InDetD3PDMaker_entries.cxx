#include "../ClusterAssocFillerTool.h"
#include "../ClusterAssocPixelFillerTool.h"
#include "../PixelClusterPixelRDOAssociationTool.h"
#include "../PixelClusterToTrackParticleAssociationTool.h"
#include "../PixelRDOClusterAssociationTool.h"
#include "../PixelRDOCollectionGetterTool.h"
#include "../PixelRDOFillerTool.h"
#include "../SCTByteStreamErrorFillerTool.h"
#include "../SCTClusterCollectionGetterTool.h"
#include "../SCTClusterFillerTool.h"
#include "../SCTClusterSCTRDOAssociationTool.h"
#include "../SCTClusterToTrackParticleAssociationTool.h"
#include "../SCTRDOClusterAssociationTool.h"
#include "../SCTRDOCollectionGetterTool.h"
#include "../SCTRDOFillerTool.h"
#include "../SCTTrackParticleToMuonAssociationTool.h"
#include "../SiHitFillerTool.h"
#include "../SiHitTruthAssociationTool.h"
#include "../TRTPhaseFillerTool.h"
#include "../TRTUncompressedHitFillerTool.h"
#include "../TSOSPixelClusterAssociationTool.h"
#include "../TrackParticleToPixelClusterAssociationTool.h"
#include "../TrackParticleToPixelClusterAssociationToolLight.h"
#include "../TrackParticleToSCTClusterAssociationTool.h"
#include "../TrackParticleToSCTClusterAssociationToolLight.h"
#include "../TrkPrepRawDataGenParticleAssociationTool.h"
#include "../TrkTrackGetterTool.h"
#include "../PixelClusterCollectionGetterTool.h"
#include "../PixelClusterFillerTool.h"
#include "../TrackBeamlineParametersAssociationTool.h"
#include "../TrackParticleLastParametersAssociationTool.h"
#include "../ErrorMatrixCovarianceFillerTool.h"
#include "../ErrorMatrixErrorFillerTool.h"
#include "../TrackParametersFillerTool.h"
#include "../TrkTrackPerigeeAssociationTool.h"
#include "../TrkTrackPerigeeAtPVAssociationTool.h"
#include "../TrkTrackPerigeeAtBSAssociationTool.h"
#include "../TrkTrackFitQualityAssociationTool.h"
#include "../TrkTrackTrackSummaryAssociationTool.h"
#include "../TrkTrackInfoAssociationTool.h"
#include "../TrkTrackTSOAssociationTool.h"
#include "../TrkTrackVertexAssociationTool.h"
#include "../TrkTrackTruthAssociationTool.h"
#include "../TrkTrackDetailedTruthAssociationTool.h"
#include "../TrkTrackParametersAtBLFillerTool.h"
#include "../TrackParticleParametersAtBLFillerTool.h"
#include "../V0CandidateFillerTool.h"
#include "../V0CandidateVxCandidateAssociationTool.h"
#include "../VertexFitQualityAssociationTool.h"
#include "../VxTrackAtVertexTrkTrackAssociationTool.h"
#include "../VertexTrackAssociationFillerTool.h"
#include "../VertexPurityFillerTool.h"
#include "../VxTrackAtVertexTrackParticleAssociationTool.h"
#include "../VertexPositionFillerTool.h"
#include "../VertexTypeFillerTool.h"
#include "../TrackParticleDetailedTruthAssociationTool.h"
#include "../TrackParticleTruthAssociationTool.h"
#include "../TrackParticleTSOAssociationTool.h"
#include "../TrackParametersErrorMatrixAssociationTool.h"
#include "../TrackParticleInfoAssociationTool.h"
#include "../TrackParticleTrackSummaryAssociationTool.h"
#include "../TrackTrackSummaryFillerTool.h"
#include "../TrackParticleFitQualityAssociationTool.h"
#include "../TrackFitQualityFillerTool.h"
#include "../TrackInfoFillerTool.h"
#include "../TSOFitQualityAssociationTool.h"
#include "../TrackStateOnSurfaceFillerTool.h"
#include "../TrackParticleGetterTool.h"
#include "../BeamSpotGetterTool.h"
#include "../BeamSpotFillerTool.h"
#include "../TruthTrackFillerTool.h"
#include "../TruthVertexFillerTool.h"
#include "../TruthTrackGetterTool.h"
#include "../TruthVertexGetterTool.h"


DECLARE_COMPONENT( D3PD::ClusterAssocFillerTool )
DECLARE_COMPONENT( D3PD::ClusterAssocPixelFillerTool )
DECLARE_COMPONENT( D3PD::PixelClusterPixelRDOAssociationTool )
DECLARE_COMPONENT( D3PD::PixelClusterToTrackParticleAssociationTool )
DECLARE_COMPONENT( D3PD::PixelRDOClusterAssociationTool )
DECLARE_COMPONENT( D3PD::PixelRDOCollectionGetterTool )
DECLARE_COMPONENT( D3PD::PixelRDOFillerTool )
DECLARE_COMPONENT( D3PD::SCTByteStreamErrorFillerTool )
DECLARE_COMPONENT( D3PD::SCTClusterCollectionGetterTool )
DECLARE_COMPONENT( D3PD::SCTClusterFillerTool )
DECLARE_COMPONENT( D3PD::SCTClusterSCTRDOAssociationTool )
DECLARE_COMPONENT( D3PD::SCTClusterToTrackParticleAssociationTool )
DECLARE_COMPONENT( D3PD::SCTRDOClusterAssociationTool )
DECLARE_COMPONENT( D3PD::SiHitFillerTool )
DECLARE_COMPONENT( D3PD::SiHitTruthAssociationTool )
DECLARE_COMPONENT( D3PD::TRTPhaseFillerTool )
DECLARE_COMPONENT( D3PD::TRTUncompressedHitContainerGetterTool )
DECLARE_COMPONENT( D3PD::TRTUncompressedHitFillerTool )
DECLARE_COMPONENT( D3PD::TSOSPixelClusterAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleToPixelClusterAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleToPixelClusterAssociationToolLight )
DECLARE_COMPONENT( D3PD::TrackParticleToSCTClusterAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleToSCTClusterAssociationToolLight )
DECLARE_COMPONENT( D3PD::TrkPrepRawDataGenParticleAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackGetterTool )
DECLARE_COMPONENT( D3PD::PixelClusterCollectionGetterTool )
DECLARE_COMPONENT( D3PD::PixelClusterFillerTool )
DECLARE_COMPONENT( D3PD::TrackBeamlineParametersAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleLastParametersAssociationTool )
DECLARE_COMPONENT( D3PD::ErrorMatrixCovarianceFillerTool )
DECLARE_COMPONENT( D3PD::ErrorMatrixErrorFillerTool )
DECLARE_COMPONENT( D3PD::TrackParametersFillerTool )
DECLARE_COMPONENT( D3PD::TrkTrackPerigeeAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackPerigeeAtPVAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackPerigeeAtBSAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackFitQualityAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackTrackSummaryAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackInfoAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackTSOAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackTruthAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackVertexAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackDetailedTruthAssociationTool )
DECLARE_COMPONENT( D3PD::TrkTrackParametersAtBLFillerTool )
DECLARE_COMPONENT( D3PD::TrackParticleParametersAtBLFillerTool )
DECLARE_COMPONENT( D3PD::V0CandidateVxCandidateAssociationTool )
DECLARE_COMPONENT( D3PD::V0CandidateFillerTool )
DECLARE_COMPONENT( D3PD::V0ContainerGetterTool )
DECLARE_COMPONENT( D3PD::VertexFitQualityAssociationTool )
DECLARE_COMPONENT( D3PD::VxTrackAtVertexTrkTrackAssociationTool )
DECLARE_COMPONENT( D3PD::VertexTrackAssociationFillerTool )
DECLARE_COMPONENT( D3PD::VertexPurityFillerTool )
DECLARE_COMPONENT( D3PD::VxTrackAtVertexTrackParticleAssociationTool )
DECLARE_COMPONENT( D3PD::VertexPositionFillerTool )
DECLARE_COMPONENT( D3PD::VertexTypeFillerTool )
DECLARE_COMPONENT( D3PD::TrackParametersErrorMatrixAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleTSOAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleDetailedTruthAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleTruthAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleInfoAssociationTool )
DECLARE_COMPONENT( D3PD::TrackParticleTrackSummaryAssociationTool )
DECLARE_COMPONENT( D3PD::TrackTrackSummaryFillerTool )
DECLARE_COMPONENT( D3PD::TrackParticleFitQualityAssociationTool )
DECLARE_COMPONENT( D3PD::TrackFitQualityFillerTool )
DECLARE_COMPONENT( D3PD::TrackInfoFillerTool )
DECLARE_COMPONENT( D3PD::TSOFitQualityAssociationTool )
DECLARE_COMPONENT( D3PD::TrackStateOnSurfaceFillerTool )
DECLARE_COMPONENT( D3PD::TrackParticleGetterTool )
DECLARE_COMPONENT( D3PD::BeamSpotGetterTool )
DECLARE_COMPONENT( D3PD::BeamSpotFillerTool )
DECLARE_COMPONENT( D3PD::TruthTrackGetterTool )
DECLARE_COMPONENT( D3PD::TruthTrackFillerTool )
DECLARE_COMPONENT( D3PD::TruthVertexGetterTool )
DECLARE_COMPONENT( D3PD::TruthVertexFillerTool )
